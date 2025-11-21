# WeatherShield Requirements & System Overview

## 1. Problem Statement

- **Dolor actual:** El invernadero piloto carece de monitoreo continuo de clima, humo e intrusiones; se depende de inspecciones manuales que no cubren horario nocturno ni generan registros históricos confiables.
- **Impacto:** Pérdidas recurrentes de cultivos por heladas o calor extremo, riesgo para personal en caso de humo/gases y falta de evidencia para auditorías de seguridad.
- **Propuesta:** Desplegar una estación meteorológica inteligente basada en Raspberry Pi que integre sensores ambientales y de seguridad, con respuesta automática, alertas en tiempo real y almacenamiento histórico.

## 2. System Overview

### 2.1 Alcance

- _Hardware:_ Raspberry Pi 4 (o 3B+), sensores BME280, BH1750, DS18B20, MQ-2, PIR HC-SR501, pluviómetro de balancín, sensor UV analógico, módulo MCP3008, relés para sirena y electroválvula, UPS HAT con batería 12 V, panel solar opcional.
- _Software:_ Servicios en Python (gpiozero, paho-mqtt, FastAPI), almacenamiento local en SQLite, pipeline MQTT → TimescaleDB, dashboard web en React/Plotly, alertas vía Telegram/Email, automatización de despliegue con systemd y scripts Ansible.
- _Usuarios clave:_ Coordinador de seguridad (operación y respuesta), agrónomo (análisis histórico), técnico TI (mantenimiento y despliegue).

### 2.2 Vistas de arquitectura

- **Hardware:** ver `wsp/prj/design/prjDesign.md` (diagramas mermaid).
- **Software:** microservicio de adquisición, controlador de actuadores, motor de reglas, sincronización con backend MQTT y dashboard web.

### 2.3 Contexto operacional

1. Sensores reportan datos cada 60 s a la Raspberry Pi.
2. Un servicio local almacena y evalúa umbrales; activa actuadores cuando sea necesario.
3. Cuando hay conectividad, los datos se sincronizan al backend y se expone dashboard histórico.
4. Alertas se distribuyen vía notificador (Telegram/Email) y se exige acuse manual.

## 3. Functional Requirements (FR)

| ID    | Descripción                                                                  | Criterio de aceptación                                                                          | Prioridad | Fuente                   |
| ----- | ---------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------- | --------- | ------------------------ |
| FR-01 | Capturar temperatura, humedad relativa, presión y luminosidad cada 60 s.     | 95 % de lecturas entregadas con precisión ±1 °C y ±2 % HR comparadas contra instrumento patrón. | Alta      | Stakeholder / DOE clima. |
| FR-02 | Detectar humo/gases combustibles y generar alerta en < 30 s.                 | Escenario de prueba con humo generará alerta sonora y notificación digital con sello de tiempo. | Alta      | Seguridad.               |
| FR-03 | Detectar movimiento en perímetro y registrar evento con imagen opcional.     | PIR activa sirena y genera evento en dashboard con estado reconocido/no reconocido.             | Media     | Seguridad.               |
| FR-04 | Sincronizar datos locales al backend vía MQTT cuando exista conectividad.    | Al recuperar Wi-Fi, datos pendientes (<48 h) se envían y se marcan como entregados.             | Alta      | Stakeholder.             |
| FR-05 | Automatizar actuadores (aspersores, sirena) basados en reglas configurables. | Reglas se pueden editar vía dashboard/API; ejecución se documenta con bitácora.                 | Media     | Seguridad / Agrónomo.    |
| FR-06 | Permitir override manual de actuadores desde dashboard seguro.               | Comandos manuales requieren autenticación 2FA y se ejecutan < 5 s.                              | Media     | Coordinador.             |
| FR-07 | Proveer dashboard web con histórico, tendencias y estado en tiempo real.     | Dashboard presenta variables clave (<5 s retraso) y permite exportación CSV diaria.             | Media     | Agrónomo.                |
| FR-08 | Registrar nivel de batería/energía y emitir alerta < 20 % de carga.          | Dashboard y alertas muestran porcentaje restante y tiempo estimado.                             | Alta      | Operación.               |

## 4. Non-Functional Requirements (NFR)

| ID     | Categoría           | Descripción                                                                    | Métrica                                                    |
| ------ | ------------------- | ------------------------------------------------------------------------------ | ---------------------------------------------------------- |
| NFR-01 | Disponibilidad      | Uptime del nodo > 95 % (medido por heartbeat de 5 min).                        | ≥ 95 %.                                                    |
| NFR-02 | Resiliencia         | Buffer local soporta 48 h de datos sin pérdida.                                | ≥ 48 h @ muestreo 60 s.                                    |
| NFR-03 | Seguridad           | Autenticación con 2FA y cifrado TLS para API y MQTT remoto.                    | 100 % endpoints con TLS.                                   |
| NFR-04 | Mantenibilidad      | Código cubierto por ≥ 70 % tests unitarios; documentación técnica actualizada. | Cobertura ≥ 70 %, issues < 5 abiertas antes de despliegue. |
| NFR-05 | Tiempo de respuesta | Alertas distribuidas en < 30 s desde detección.                                | ≤ 30 s en pruebas.                                         |
| NFR-06 | Operación ambiental | Enclosure IP54, operación entre -10 °C y 45 °C con HR 10-90 %.                 | Validado por pruebas climáticas.                           |
| NFR-07 | Privacidad          | Datos personales anonimizados; logs cumplen política de retención (90 días).   | Auditoría interna sin hallazgos.                           |

## 5. Interface Requirements

- **GPIO/SPI/I2C:** Documentar mapeo de pines en `hardware/wiring-map.xlsx` (pendiente).
- **MQTT Topics:** `weather/<site>/telemetry`, `weather/<site>/alerts`, `weather/<site>/cmd`.
- **API REST:** `/v1/readings`, `/v1/alerts`, `/v1/actuators`, `/v1/rules` (ver especificación OpenAPI futura).
- **Dashboard:** Roles `viewer`, `operator`, `admin` con permisos diferenciados.

## 6. Supuestos y Dependencias

- Disponibilidad de red Wi-Fi local o router LTE con plan de datos mínimo 1 GB/mes.
- Acceso físico para mantenimiento semanal y recalibración del MQ-2.
- Energía solar y batería dimensionadas para autonomía de 12 h; se realizará prueba de estrés.
- Software base en Raspberry Pi OS Lite 64-bit (kernel 6.x) con Python 3.11.

## 7. Matriz de trazabilidad inicial

| Requirement         | Verificación prevista                                                  | Evidencia                                                           |
| ------------------- | ---------------------------------------------------------------------- | ------------------------------------------------------------------- |
| FR-01, FR-02, FR-03 | Plan de pruebas unitarias e integración (`docs/verification-plan.md`). | Logs de pytest, reportes Influx/Timescale, fotos/screen recordings. |
| FR-04, NFR-02       | Pruebas de desconexión y reconciliación.                               | Scripts de simulación + capturas de buffer SQLite.                  |
| FR-05, FR-06        | Testing de reglas y overrides en staging.                              | Casos en Zephyr/Jira + videos.                                      |
| FR-07               | Validación UX con stakeholder y prueba de rendimiento.                 | Encuesta, reporte Lighthouse.                                       |
| FR-08, NFR-01       | Test de energía y heartbeat.                                           | Logs de monitor de batería, métricas Prometheus.                    |
| NFR-03, NFR-07      | Pen-test ligero y revisión de políticas.                               | Informe de seguridad y checklist.                                   |
| NFR-04              | Cobertura unitaria e integración.                                      | Reporte coverage.py > 70 %.                                         |
| NFR-05              | Prueba end-to-end de alertas (MQ-2 y PIR).                             | Cronómetro y logs con sello UTC.                                    |
