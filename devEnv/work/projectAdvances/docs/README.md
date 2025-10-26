# WeatherShield - Guía de Trabajo según Rúbrica

Este README resume cómo organizamos el proyecto para cumplir cada criterio del profesor Juan Bernardo Gómez-Mendoza. Sirve como referencia rápida para funcionamiento, objetivos y entregables.

## 1. Funcionamiento general del sistema
- Propósito: estación meteorológica y de seguridad para invernaderos urbanos con Raspberry Pi 4/3B+.
- Flujo principal:
  1. Sensores digitales (BME280, BH1750, DS18B20) y analógicos (MQ-2, pluviómetro, sensor UV) entregan datos a la Raspberry Pi.
  2. El servicio Python `weather-node` normaliza las lecturas, las persiste en SQLite y evalúa reglas de umbral.
  3. Ante eventos críticos activa actuadores (relés para sirena, aspersores, ventiladores) y envía alertas (Telegram, email, dashboard, sirena física).
  4. Cuando hay conectividad, publica los datos en MQTT -> TimescaleDB -> dashboard React/Plotly para visualización histórica y control manual.
  5. Monitorea energía (UPS HAT + batería 12V 7Ah) y aplica modo ahorro si el nivel cae debajo de 20%.

## 2. Objetivos clave por rúbrica
- **1.1 Problema y requisitos (20%)**: Documentar motivación, arquitectura y requisitos medibles. Evidencia: `design/prjDesign.md`, `docs/requirements.md`, `hardware/bom.md`.
- **1.2 Plan de verificación (20%)**: Diseñar pruebas unitarias automatizadas con cobertura >= 70% y trazabilidad de bugs. Evidencia: `docs/verification-plan.md`, carpeta `tests/`, pipeline CI.
- **1.3 Pruebas de integración (40%)**: Ejecutar casos hardware-in-the-loop con registros, análisis y corrección de fallos. Evidencia: IT-01..IT-07 (ver plan), logs en `tests/evidence/`.
- **1.4 Documentación final (20%)**: Mantener README, guías de usuario/despliegue y changelog coherentes. Evidencia: `wsp/prj/README.md`, guías por generar (`docs/user-guide.md`, `docs/deployment.md`, `CHANGELOG.md`).
- **1.5 Requisitos de entrega**: Repositorio público, scripts reproducibles, evidencia accesible y demo final. Evidencia: scripts en `scripts/` (pendiente), demo en `advances/`, matriz `docs/rubric-traceability.md`.

## 3. Flujo de trabajo por semanas
- Semanas 1-2: Validar necesidades con stakeholder, cerrar BOM, configurar entorno y CI.
- Semanas 3-4: Construir telemetría digital + buffer + dashboard mínimo, alcanzar >50% cobertura unitaria.
- Semana 5-6: Integrar sensores analógicos, motor de reglas, actuadores y alertas con evidencia IT-02/IT-04/IT-05.
- Semana 7: Pruebas de resiliencia (energía, conectividad, seguridad), limpieza de issues, documentación de guías.
- Semana 8: Pruebas en campo, demo grabada, reporte final y verificación de trazabilidad completa.

## 4. Roles y responsabilidades
- Stakeholder (coordinador de seguridad): valida requisitos, aprueba métricas y evalúa alertas.
- Equipo técnico (Mateo + colaboradores): desarrollo hardware/software, ejecución de pruebas, documentación.
- Mentor/Docente: revisa hitos semanales, retroalimenta el cumplimiento de la rúbrica.

## 5. Herramientas y ambientes
- Hardware: Raspberry Pi 4/3B+, UPS HAT, batería 12V 7Ah, sensores listados en `hardware/bom.md`, actuadores (sirena, aspersores).
- Software: Raspberry Pi OS Lite 64-bit, Python 3.11, gpiozero, paho-mqtt, FastAPI, React/Plotly, Mosquitto, TimescaleDB/PostgreSQL.
- DevOps: Git/GitHub, GitHub Actions o runner en Pi, pytest + coverage.py, Ansible, Docker para backend, Prometheus/Grafana para monitoreo.
- Gestión: tablero Kanban (GitHub Projects/Trello), sesiones semanales de revisión, bitácora de riesgos en `docs/risk-register.md`.

## 6. Checklist de cumplimiento
- [ ] Requisitos actualizados y trazados en `docs/requirements.md` y `docs/verification-plan.md`.
- [ ] BOM completa y hardware listo (`hardware/bom.md`).
- [ ] Cobertura unitaria >= 70% con reportes almacenados en `tests/results/`.
- [ ] Evidencia IT-01..IT-07 en `tests/evidence/IT-0X` con enlaces en `docs/rubric-traceability.md`.
- [ ] Documentación final (README, guías, changelog, reporte de lecciones) publicada y enlazada.
- [ ] Demo final y reporte reflexivo listos para entrega según rúbrica.

Mantén este README actualizado a medida que avances; cualquier cambio en alcance o estrategia debe reflejarse aquí y en la matriz de trazabilidad.


## 7. Recursos visuales
- Diagrama comunicación: `diagrams/communication-block-diagram.png`
- Diagrama hardware: `diagrams/hardware-block-diagram.png`
- Diagrama software: `diagrams/software-block-diagram.png`
