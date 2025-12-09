# WeatherShield Verification & Validation Plan

## 1. Objetivo

Asegurar que el prototipo WeatherShield cumpla los requisitos funcionales y no funcionales definidos en `docs/requirements.md`, alineado con la rúbrica de la asignatura (secciones 1.2 y 1.3). El plan cubre pruebas unitarias, integración (hardware-in-the-loop) y evidencia requerida.

## 2. Estrategia general

- **Capas:**
  1. _Unitarias_ (Python/C) sobre drivers, servicios y lógica de reglas. [x]
  2. _Integración_ en laboratorio (sensores reales + simuladores). [x]
  3. _End-to-end_ en campo piloto con escenarios realistas. [x]
- **Herramientas:** pytest, coverage.py, MQTT test harness (mosquitto_pub/sub), Node-RED para simulación.
- **Automatización:** GitHub Actions/Runner en Raspberry Pi para CI, scripts Ansible para despliegue de pruebas en edge, registros en formato JUnit XML.

## 3. Matriz de pruebas unitarias

| Módulo                 | Objetivo                               | Técnica                                                | Herramienta                 | Métrica                            |
| ---------------------- | -------------------------------------- | ------------------------------------------------------ | --------------------------- | ---------------------------------- |
| `sensors.bme280/lgpio` | Validar lectura y conversión de datos  | Tests con mocks e inyección de datos calibrados        | check/mocka + unittest.mock | Cobertura > 90 %                   |
| `sensors./MQ...`       | Normalizar curva de calibración        | Pruebas parametrizadas con curvas generadas            | check/mocka + numpy         | Error < 5 % respecto curva teórica |
| `buffer.storage`       | Persistencia y reconciliación de datos | TDD con SQLite en memoria                              | check/mocka + sqlite3       | 0 pérdidas en 10k registros        |
| `rules.engine`         | Evaluación de reglas y cooldown        | Tests de caja negra con fixtures                       | check/mocka                 | 100 % casos críticos cubiertos     |
| `actuators.controller` | Manejo de GPIO/relés seguro            | Tests con `gpiozero/lgpio` mock y simulación de fallos | check/mocka                 | Cobertura > 85 %                   |
| `alerts.notifier`      | Envío de mensajes y reintentos         | Tests asíncronos con cliente fake                      | check/mocka-asyncio         | 100 % reintentos dentro de SLA     |

**Criterios de salida unitarios:**  
Cobertura global ≥ 70 %, sin fallos críticos, issues documentados en tracker y plan de resolución antes de integración.

## 4. Plan de pruebas de integración

| TEST ID | REQ ID | Objetivo                                        | Procedimiento                                         | Resultado esperado                             | Prioridad |
| ------- | ------ | ----------------------------------------------- | ----------------------------------------------------- | ---------------------------------------------- | --------- |
| TC-01   | RF-01  | Verificar lectura de gases MQ-2 y MQ-135        | Encender sistema y observar datos en consola cada 5 s | Lecturas válidas con valores distintos de cero | Alta      |
| TC-02   | RF-03  | Activar válvula ante flama o temperatura >50 °C | Aplicar calor con soplete y medir tiempo de reacción  | Relé activa en <1 s                            | Alta      |
| TC-03   | RF-05  | Validar alarma sonora y visual                  | Inyectar señal de gas MQ-2                            | Buzzer y LED encendidos                        | Alta      |
| TC-04   | RF-06  | Verificar envío MQTT                            | Observar mensajes publicados en broker remoto         | Datos recibidos con timestamp correcto         | Media     |
| TC-05   | RF-07  | Validar autonomía                               | Desconectar fuente principal y medir tiempo operativo | Sistema activo ≥15 min                         | Media     |

**Criterios de salida integración:**

- 100 % de pruebas ejecutadas.
- Evidencia almacenada en `../tests/evidence/<id>/`.
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

- Guardar resultados de testeo utinario y coverage (`.xml/.html`) en `../tests/results/`.
- Capturas, videos y logs comprimidos por ID de prueba.
- Actualizar `docs/rubric-traceability.md` con enlaces a evidencia conforme se genere.
