# WeatherShield Roadmap (8 semanas)

> Objetivo: entregar prototipo funcional, evidencia de pruebas y documentación completa conforme a la rúbrica de Embedded Linux Capstone.

## Vista general

| Fase                 | Semanas | Foco                                                              | Entregables clave                                                                           |
| -------------------- | ------- | ----------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| Discover & Frame     | 1       | Validar problema, stakeholders, métricas de éxito.                | Actualización `design/prjDesign.md`, entrevistas documentadas, versión 0 de requisitos.     |
| Plan & Prepare       | 2       | Confirmar BOM, preparar ambiente, definir arquitectura detallada. | `hardware/bom.md`, wiring-map preliminar, pipeline CI básico.                               |
| Build Core Telemetry | 3-4     | Implementar adquisición digital + buffer + dashboard mínimo.      | Código sensores digitales, servicio MQTT local, dashboard básico, pruebas unitarias > 50 %. |
| Extend & Automate    | 5       | Integrar sensores analógicos, reglas y actuadores.                | Calibración MQ-2/pluviómetro, reglas automáticas, override manual, evidencia IT-02/IT-04.   |
| Harden & Test        | 6-7     | Pruebas de resiliencia, energía, seguridad, tuning UI.            | Resultados IT-03/05/06/07, ajustes de UX, cobertura ≥ 70 %, reporte de seguridad.           |
| Deploy & Wrap        | 8       | Pruebas en campo, documentación final, demo.                      | Informe final, video demo, user guide, registro de lecciones aprendidas.                    |

## Backlog priorizado (MVP)

1. Lectura continua de BME280/BH1750/DS18B20 con almacenamiento local (FR-01).
2. Publicación MQTT + buffer + reconciliación (FR-04, NFR-02).
3. Alertas humo y movimiento con sirena y notificación (FR-02, FR-03, FR-05).
4. Dashboard web con visualización en tiempo real e histórico básico (FR-07).
5. Monitoreo y alerta de batería (FR-08).

## Hitos y criterios de aceptación

- **Hito A (Semana 2):** Arquitectura y BOM validadas, plan de pruebas definido.
  - _Aceptación:_ Stakeholder aprueba requisitos y cronograma.
- **Hito B (Semana 4):** Telemetría digital integrada, dashboard básico y pruebas unitarias > 50 %.
  - _Aceptación:_ Demo interna mostrando flujo sensor → dashboard.
- **Hito C (Semana 6):** Actuadores y reglas funcionando, evidencias IT-01 a IT-04 registradas.
  - _Aceptación:_ Revisión técnica con docente, sin issues críticos abiertos.
- **Hito D (Semana 8):** Pruebas de campo documentadas, documentación final completa.
  - _Aceptación:_ Entrega de reporte final, video y repositorio listo.

## Gestión de trabajo

- Sprint semanal con daily asíncrono (Slack) y revisión los viernes.
- Uso de tablero Kanban con columnas _To Do / In Progress / Testing / Done_.
- Medir avance por historias cerradas y cumplimiento de evidencia de pruebas.

## Dependencias externas

- Disponibilidad de sitio piloto en semanas 6-8.
- Compra de componentes (lead time máximo 7 días).
- Acceso a red 4G de respaldo para pruebas de conectividad.

## Riesgos del cronograma

- _Retraso en hardware:_ Mitigación con plan B (sensores alternos) y pruebas en banco si no llega hardware.
- _Condiciones climáticas adversas:_ Preparar simulaciones indoor para validar requisitos críticos.
- _Sobrecarga académica:_ Planificar bloques de trabajo en calendario y reservar tiempo buffer en semanas 6 y 8.
