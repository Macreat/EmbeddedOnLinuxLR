/* ESP32 MQTT: Actuador de LEDs y Botón de Control
   - GPIO 18, 19, 21: Salidas LEDs (Obedecen a sistema/estado)
   - GPIO 23: Entrada Botón (Envía eventos a sistema/evento)
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"

// --- CONFIGURACIÓN DE PINES ---
#define GPIO_LED_RED 18
#define GPIO_LED_GREEN 19
#define GPIO_LED_BLUE 21
#define GPIO_BUTTON 23

#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_LED_RED) | (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_BLUE))
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_BUTTON)

// --- WIFI & MQTT ---
#define EXAMPLE_ESP_WIFI_SSID "WMACREAT"
#define EXAMPLE_ESP_WIFI_PASS "macreat1307"
#define MQTT_BROKER_URL "mqtt://3.134.86.43:1883"

#define TOPIC_SUB_STATE "sistema/estado" // Recibimos órdenes
#define TOPIC_PUB_EVENT "sistema/evento" // Enviamos eventos

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const char *TAG = "ESP32_FULL";
static int s_retry_num = 0;
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

static esp_mqtt_client_handle_t mqtt_client = NULL;

// --- VARIABLES DEL BOTÓN ---
static QueueHandle_t gpio_evt_queue = NULL;
static esp_timer_handle_t click_timer = NULL;
static bool waiting_for_double = false;

// --- CONTROL DE LEDS ---
void set_remote_leds(int r, int g, int b)
{
    gpio_set_level(GPIO_LED_RED, r);
    gpio_set_level(GPIO_LED_GREEN, g);
    gpio_set_level(GPIO_LED_BLUE, b);
    ESP_LOGI(TAG, "LEDs -> R:%d G:%d B:%d", r, g, b);
}

// --- LOGICA DEL TIMER (Distingue 1 vs 2 clicks) ---
void click_timer_callback(void *arg)
{
    // Timeout: Pasó el tiempo y no hubo segundo click
    waiting_for_double = false;
    if (mqtt_client)
    {
        ESP_LOGI(TAG, "CLICK SIMPLE -> Enviando EVENT_CLICK_1");
        esp_mqtt_client_publish(mqtt_client, TOPIC_PUB_EVENT, "EVENT_CLICK_1", 0, 0, 0);
    }
}

// --- INTERRUPCIÓN GPIO (ISR) ---
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// --- TAREA QUE PROCESA EL BOTÓN ---
static void button_task(void *arg)
{
    uint32_t io_num;
    uint32_t last_time = 0;

    // Timer de 600ms para esperar doble click
    const esp_timer_create_args_t timer_args = {.callback = &click_timer_callback, .name = "click_timer"};
    esp_timer_create(&timer_args, &click_timer);

    while (1)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            // Debounce 200ms
            if (now - last_time < 200)
                continue;
            last_time = now;

            if (waiting_for_double)
            {
                // SEGUNDO CLICK DETECTADO
                esp_timer_stop(click_timer);
                waiting_for_double = false;
                ESP_LOGI(TAG, "DOBLE CLICK -> Enviando EVENT_CLICK_2");
                if (mqtt_client)
                    esp_mqtt_client_publish(mqtt_client, TOPIC_PUB_EVENT, "EVENT_CLICK_2", 0, 0, 0);
            }
            else
            {
                // PRIMER CLICK
                waiting_for_double = true;
                esp_timer_start_once(click_timer, 600000); // Esperar 600ms (us = ms*1000)
            }
        }
    }
}

// --- MANEJADOR MQTT ---
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Conectado MQTT. Suscribiendo...");
        esp_mqtt_client_subscribe(mqtt_client, TOPIC_SUB_STATE, 0);
        // Parpadeo inicio
        set_remote_leds(1, 1, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        set_remote_leds(0, 0, 0);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Recibido: %.*s", event->data_len, event->data);
        if (strncmp(event->data, "ALARM", event->data_len) == 0)
            set_remote_leds(1, 0, 0); // ROJO
        else if (strncmp(event->data, "WARNING", event->data_len) == 0)
            set_remote_leds(0, 0, 1); // AZUL
        else if (strncmp(event->data, "NORMAL", event->data_len) == 0)
            set_remote_leds(0, 1, 0); // VERDE
        break;
    default:
        break;
    }
}

// --- INICIALIZACIÓN ---
void init_gpios()
{
    // Config LEDs
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    gpio_config(&io_conf);

    // Config Botón
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_BUTTON, gpio_isr_handler, (void *)GPIO_BUTTON);

    set_remote_leds(0, 0, 0);
}

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

// --- WIFI ESTÁNDAR ---
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
        }
        else
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
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
    wifi_config_t wifi_config = {.sta = {.ssid = EXAMPLE_ESP_WIFI_SSID, .password = EXAMPLE_ESP_WIFI_PASS, .threshold.authmode = WIFI_AUTH_WPA2_PSK}};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
}

void app_main(void)
{
    nvs_flash_init();
    init_gpios();
    wifi_init_sta();
    mqtt_app_start();
}