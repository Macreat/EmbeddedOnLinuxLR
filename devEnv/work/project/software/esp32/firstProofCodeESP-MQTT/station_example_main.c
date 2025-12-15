/* ESP32 MQTT Remote Actuator (3 Discrete LEDs)
   Based on ESP-IDF Station Example
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h" // <--- CAMBIO IMPORTANTE: Control GPIO nativo
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"

// --- CONFIGURACIÓN DE PINES FÍSICOS (LEDs DISCRETOS) ---
#define GPIO_LED_RED 18
#define GPIO_LED_GREEN 19
#define GPIO_LED_BLUE 21
// Máscara de bits para configurar los pines
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_LED_RED) | (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_BLUE))

// --- CONFIGURACIÓN WIFI Y MQTT ---
#define EXAMPLE_ESP_WIFI_SSID "WMACREAT"
#define EXAMPLE_ESP_WIFI_PASS "macreat1307"
#define MQTT_BROKER_URL "mqtt://3.134.86.43:1883"
#define TOPIC_SYSTEM_STATE "sistema/estado" // Tópico a escuchar

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const char *TAG = "ESP32_ACTUATOR";
static int s_retry_num = 0;
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static esp_mqtt_client_handle_t mqtt_client = NULL;

// --- FUNCIÓN PARA CONTROLAR LOS 3 PINES ---
// active_high: 1 para encender con HIGH, 0 para encender con LOW (depende de tu circuito)
void set_remote_leds(int r, int g, int b)
{
    gpio_set_level(GPIO_LED_RED, r);
    gpio_set_level(GPIO_LED_GREEN, g);
    gpio_set_level(GPIO_LED_BLUE, b);
    ESP_LOGI(TAG, "Actuadores -> R:%d G:%d B:%d", r, g, b);
}

// --- MANEJADOR DE EVENTOS MQTT ---
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Conectado al Broker AWS. Suscribiendo...");
        esp_mqtt_client_subscribe(mqtt_client, TOPIC_SYSTEM_STATE, 0);

        // Señal visual de conexión: Parpadeo rápido de los 3
        set_remote_leds(1, 1, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        set_remote_leds(0, 0, 0);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Mensaje recibido: %.*s", event->data_len, event->data);

        // Lógica de actuación basada en la decisión de la Raspberry Pi
        if (strncmp(event->data, "ALARM", event->data_len) == 0)
        {
            // ESTADO CRÍTICO -> SOLO ROJO
            set_remote_leds(1, 0, 0);
        }
        else if (strncmp(event->data, "WARNING", event->data_len) == 0)
        {
            // ESTADO PRECAUCIÓN -> AZUL (O Amarillo R+G si prefieres)
            set_remote_leds(0, 0, 1);
        }
        else if (strncmp(event->data, "NORMAL", event->data_len) == 0)
        {
            // ESTADO OK -> VERDE
            set_remote_leds(0, 1, 0);
        }
        else
        {
            // APAGAR TODO SI ES DESCONOCIDO
            set_remote_leds(0, 0, 0);
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Desconectado del Broker");
        break;
    default:
        break;
    }
}

// --- INICIALIZACIÓN DE GPIO ---
void init_led_gpios()
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Apagar todo al inicio
    set_remote_leds(0, 0, 0);
}

// --- INICIO MQTT ---
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URL,
        .credentials.username = "esp32",
        .credentials.authentication.password = "12345678",
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

// --- GESTIÓN WIFI (Estándar) ---
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Reintentando conexión WiFi...");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

void app_main(void)
{
    // Inicialización NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 1. Inicializar Pines GPIO (LEDs Físicos)
    init_led_gpios();

    // 2. Conectar WiFi
    wifi_init_sta();

    // 3. Iniciar MQTT
    mqtt_app_start();
}