# WeatherShield Verification & Validation Plan

## 1. Objetivo

Asegurar que el prototipo WeatherShield cumpla los requisitos funcionales y no funcionales definidos en `docs/requirements.md`, alineado con la rúbrica de la asignatura (secciones 1.2 y 1.3). El plan cubre pruebas unitarias, integración (hardware-in-the-loop) y evidencia requerida.

## 2. Estrategia general

- **Capas:**
  1. _Unitarias_ (Python) sobre drivers, servicios y lógica de reglas.
  2. _Integración_ en laboratorio (sensores reales + simuladores).
  3. _End-to-end_ en campo piloto con escenarios realistas.
- **Herramientas:** pytest, pytest-asyncio, tox, coverage.py, MQTT test harness (mosquitto_pub/sub), Node-RED para simulación, Prometheus/Grafana para métricas.
- **Automatización:** GitHub Actions/Runner en Raspberry Pi para CI, scripts Ansible para despliegue de pruebas en edge, registros en formato JUnit XML.

## 3. Matriz de pruebas unitarias

| Módulo                 | Objetivo                               | Técnica                                          | Herramienta            | Métrica                            | Responsable |
| ---------------------- | -------------------------------------- | ------------------------------------------------ | ---------------------- | ---------------------------------- | ----------- |
| `sensors.bme280`       | Validar lectura y conversión de datos  | Tests con mocks e inyección de datos calibrados  | pytest + unittest.mock | Cobertura > 90 %                   |             |
| `sensors.mq2`          | Normalizar curva de calibración        | Pruebas parametrizadas con curvas generadas      | pytest + numpy         | Error < 5 % respecto curva teórica |             |
| `buffer.storage`       | Persistencia y reconciliación de datos | TDD con SQLite en memoria                        | pytest + sqlite3       | 0 pérdidas en 10k registros        |             |
| `rules.engine`         | Evaluación de reglas y cooldown        | Tests de caja negra con fixtures                 | pytest                 | 100 % casos críticos cubiertos     |             |
| `actuators.controller` | Manejo de GPIO/relés seguro            | Tests con `gpiozero` mock y simulación de fallos | pytest                 | Cobertura > 85 %                   |             |
| `alerts.notifier`      | Envío de mensajes y reintentos         | Tests asíncronos con cliente fake                | pytest-asyncio         | 100 % reintentos dentro de SLA     |             |

**Criterios de salida unitarios:**  
Cobertura global ≥ 70 %, sin fallos críticos, issues documentados en tracker y plan de resolución antes de integración.

## 4. Plan de pruebas de integración

| ID                           | Propósito                                            | Requisitos cubiertos | Setup                                                         | Evidencia esperada                                                    |
| ---------------------------- | ---------------------------------------------------- | -------------------- | ------------------------------------------------------------- | --------------------------------------------------------------------- |
| IT-01 Sensores digitales     | Validar adquisición continua BME280, BH1750, DS18B20 | FR-01, NFR-01        | Bancada con cámara climática y logging cada 60 s.             | Gráficas comparativas vs sensor patrón, logs sincronizados.           |
| IT-02 Sensores analógicos    | Calibración MQ-2 y pluviómetro via MCP3008           | FR-02, FR-01         | Cámara de humo controlada, simulador de lluvia.               | Curvas de calibración, reporte error < ±5 %.                          |
| IT-03 Buffer y reconexión    | Verificar almacenamiento 48 h y sincronización MQTT  | FR-04, NFR-02        | Desconexión controlada (apagar router) durante 12 h.          | Comparación registros SQLite vs DB final, confirmación de no pérdida. |
| IT-04 Reglas y actuadores    | Evaluar triggers automáticos y overrides manuales    | FR-05, FR-06, FR-02  | Aspersor y sirena conectados a bancada, dashboard en staging. | Video + logs actuadores con sello, confirmación override.             |
| IT-05 Alertas E2E            | Notificaciones humo/movimiento en < 30 s             | FR-02, FR-03, NFR-05 | Simulación intrusión + humo, red LTE backup.                  | Capturas de pantallas, logs con latencias, acuse de recibo.           |
| IT-06 Resiliencia energética | Autonomía 12 h y monitoreo baterías                  | FR-08, NFR-01        | Banco de pruebas con carga controlada, monitor voltaje.       | Reporte de descarga, curvas SOC, alarmas enviadas.                    |
| IT-07 Seguridad de red       | TLS, autenticación y hardening base                  | NFR-03, NFR-07       | Escaneo con Nessus lite, pruebas de acceso no autorizado.     | Reporte sin hallazgos críticos, checklist CIS cumplido.               |

**Criterios de salida integración:**

- 100 % de pruebas ejecutadas.
- Evidencia almacenada en `wsp/prj/tests/evidence/<id>/`.
- Issues documentados con plan de mitigación antes de despliegue en campo.

## 5. Cronograma de verificación

| Semana | Actividad                                                                     | Entregable                                                   |
| ------ | ----------------------------------------------------------------------------- | ------------------------------------------------------------ |
| 1-2    | Configurar harness de testing, escribir pruebas unitarias sensores digitales. | Reporte cobertura inicial, logs pytest.                      |
| 3-4    | Completar unit testing de reglas y actuadores, iniciar IT-01/IT-02.           | Evidencia calibración, actualización `verification-plan.md`. |
| 5-6    | Ejecutar IT-03, IT-04 y validar alertas.                                      | Logs MQTT, videos override.                                  |
| 7      | Pruebas de resiliencia energética y seguridad de red.                         | Reportes IT-06, IT-07, checklist hardening.                  |
| 8      | Pruebas en campo, consolidación de evidencia y reporte final.                 | Informe de verificación para entrega final.                  |

## 6. Gestión de defectos

- Registrar defectos en board Kanban (GitHub Projects / Jira) con severidad, prioridad y estado.
- Cada fallo debe conectar requisito ↔ prueba ↔ evidencia ↔ ticket.
- Métrica objetivo: cerrar defectos críticos antes de pasar a la siguiente fase de pruebas.

## 7. Evidencia y trazabilidad

- Guardar resultados de pytest (`.xml`) y coverage (`.xml/.html`) en `wsp/prj/tests/results/`.
- Capturas, videos y logs comprimidos por ID de prueba.
- Actualizar `docs/rubric-traceability.md` con enlaces a evidencia conforme se genere.
