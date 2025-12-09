# WeatherShield Requirements & System Overview

## 1. Problem Statement

- **Dolor actual:** El invernadero piloto carece de monitoreo continuo de clima, humo e intrusiones; se depende de inspecciones manuales que no cubren horario nocturno ni generan registros históricos confiables.
- **Impacto:** Pérdidas recurrentes de cultivos por heladas o calor extremo, riesgo para personal en caso de humo/gases y falta de evidencia para auditorías de seguridad.
- **Propuesta:** Desplegar una estación meteorológica inteligente basada en Raspberry Pi que integre sensores ambientales y de seguridad, con respuesta automática, alertas en tiempo real y almacenamiento histórico.

## 2. System Overview

### 2.1 Alcance

- _Hardware:_ Raspberry Pi 4 (o 3B+), sensores digitales como el MQ‑135 (calidad de aire) , MQ‑2 (humo/GLP), Flame sensor KY‑026, DHT11/DHT22 (temperatura‑humedad) y relés para los actuadores como una sirena y electroválvula, UPS HAT con batería 12 V, panel solar opcional.
- _Software:_ Servicios en C/C++ (lgpio,gpiozero, paho-mqtt, FastAPI), almacenamiento local en SQLite, pipeline MQTT → TimescaleDB, dashboard web en React/Plotly, alertas vía Telefono/Email, automatización de despliegue con systemd y scripts.

### 2.2 Vistas de arquitectura

- **Hardware:** ver `..design/hardware.jpg` (diagramas mermaid).
- **Software:** microservicio de adquisición, controlador de actuadores, motor de reglas, sincronización con backend MQTT y dashboard web conectado a un servidor en AWS EC2.

### 2.3 Contexto operacional

1. Sensores reportan datos cada 60 s a la Raspberry Pi.
2. Un servicio local almacena y evalúa umbrales; activa actuadores cuando sea necesario.
3. Cuando hay conectividad, los datos se sincronizan al backend y se expone dashboard histórico.
4. Alertas se distribuyen vía notificador (Telegram/Email) y se exige acuse manual.

## 3. Functional Requirements (FR)

| ID    | Descripción                                                                     | Criterio de aceptación                                                    | Prioridad | Fuente    |
| ----- | ------------------------------------------------------------------------------- | ------------------------------------------------------------------------- | --------- | --------- |
| RF-01 | Medir concentración de gases inflamables (MQ-2) y tóxicos (MQ-135) cada 5 s.    | Datos válidos recibidos cada ≤5 s en consola o base de datos.             | Alta      | Seguridad |
| RF-02 | Medir temperatura y humedad mediante sensor DHT11.                              | Lecturas correctas en rango 0–50 °C y 20–90 % HR.                         | Alta      | Seguridad |
| RF-03 | Activar válvula de aspersores si temperatura > 50 °C o se detecta flama.        | Relé de válvula activo <1 s después de la detección.                      | Alta      | Seguridad |
| RF-04 | Activar sistema de ventilación si se detectan gases peligrosos (MQ-2 o MQ-135). | Ventilador encendido durante el evento de alerta.                         | Alta      | Seguridad |
| RF-05 | Emitir alarma sonora y visual ante cualquier condición de riesgo.               | Buzzer y luz encendidos ante detección de gas o fuego.                    | Alta      | Seguridad |
| RF-06 | Transmitir datos de sensores vía MQTT a un broker remoto.                       | Lecturas visibles en servidor MQTT o dashboard.                           | Media     | IoT       |
| RF-07 | Mantener operación del sistema al menos 15 min con batería de respaldo.         | Sensores y alertas operativas tras desconexión de alimentación principal. | Media     | Operación |

## 4. Non-Functional Requirements (NFR)

| ID     | Descripción                                                               | Criterio de aceptación                               | Prioridad | Fuente       |
| ------ | ------------------------------------------------------------------------- | ---------------------------------------------------- | --------- | ------------ |
| RNF-01 | El sistema debe ejecutarse bajo Linux Ubuntu en Raspberry Pi 3.           | Sistema operativo instalado y estable.               | Alta      | Plataforma   |
| RNF-02 | El código deberá estar implementado en lenguaje C con compilación nativa. | Proyecto compilable sin errores en GCC.              | Alta      | Desarrollo   |
| RNF-03 | Tiempo de respuesta máximo 1 s ante condiciones críticas.                 | Delay ≤ 1 s medido con timestamp en logs.            | Alta      | Seguridad    |
| RNF-04 | El sistema deberá registrar los datos localmente (CSV o SQLite).          | Archivo de registro actualizado periódicamente.      | Media     | Operación    |
| RNF-05 | El diseño deberá ser modular y escalable para nuevos sensores.            | Nuevos sensores integrables sin modificar el núcleo. | Media     | Arquitectura |

## 5. Interface Requirements

- **GPIO/SPI/I2C:** Documentar mapeo de pines en `hardware/wiring-map.xlsx` (pendiente).
- **MQTT Topics:** `weather/<site>/telemetry`, `weather/<site>/alerts`, `weather/<site>/cmd`.
- **API REST:** `/v1/readings`, `/v1/alerts`, `/v1/actuators`, `/v1/rules` (ver especificación OpenAPI futura).
- **Dashboard:** Roles `viewer`, `operator`, `admin` con permisos diferenciados.

## 6. Supuestos y Dependencias

- Disponibilidad de red Wi-Fi local o router LTE con plan de datos mínimo 1 GB/mes.
- Acceso físico para mantenimiento semanal y recalibración del MQ-2.
- Energía solar y/o batería dimensionadas para autonomía de 12 h; se realizará prueba de estrés.
- Software base en Raspberry Pi 3 modelB OS Lite 64-bit con Python 3.11 y compiladores GCC para C/C++.

## 7. Matriz de trazabilidad inicial

| Requirement         | Verificación prevista                                                  | Evidencia                                                           |
| ------------------- | ---------------------------------------------------------------------- | ------------------------------------------------------------------- |
| FR-01, FR-02, FR-03 | Plan de pruebas unitarias e integración (`docs/verification-plan.md`). | Logs de testeo, reportes Influx/Timescale, fotos/screen recordings. |
| FR-04, NFR-02       | Pruebas de desconexión y reconciliación.                               | Scripts de simulación + capturas de buffer a usar.                  |
| FR-05, FR-06        | Testing de reglas y overrides en staging.                              | Casos en Zephyr/Jira + videos.                                      |
| FR-07               | Validación UX con stakeholder y prueba de rendimiento.                 | Encuesta, reporte Lighthouse.                                       |
| NFR-03, NFR-07      | Pen-test ligero y revisión de políticas.                               | Informe de seguridad y checklist.                                   |
| NFR-04              | Cobertura unitaria e integración.                                      | Reporte coverage > 70 %.                                            |
