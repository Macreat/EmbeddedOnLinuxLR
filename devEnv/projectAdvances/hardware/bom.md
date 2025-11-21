# WeatherShield Bill of Materials (BOM)

| Item                                                  | Cantidad | Estado      | Especificación / Notas                                    | Proveedor objetivo       |
| ----------------------------------------------------- | -------- | ----------- | --------------------------------------------------------- | ------------------------ |
| Raspberry Pi 3 Model B (4 GB)                         | 1        | Disponible  | Nodo principal, requiere disipadores y carcasa ventilada. | Stock personal / CanaKit |
| Tarjeta microSD 32 GB A2                              | 1        | Disponible  | Sistema operativo y logs.                                 | Kingston / Sandisk       |
| Batería AGM 9V                                        | 1        | Pendiente   | Autonomía 12 h; evaluar SLA vs LiFePO4.                   | LTH / Trojan             |
| Módulo MQ-2 + carcasa ventilada                       | 1        | Disponible  | Detecta humo/GLP/Gasolina, requiere calibración.          | Keyestudio               |
| Sensor MQ-135 Gas sensor                              | 1        | Disponible  | Luz en lux, rango 0-65535.                                | DFRobot                  |
| Sensor DS18B20 encapsulado                            | 1        | Por Revisar | Temp exterior resistente agua. (temp y humedad)           | Adafruit                 |
| Fotocelda                                             | 1        | Por Revisar | Luz en lux, rango 0-65535.                                |                          |
| SENSOR DE FLAMA                                       | 1        | Por revisar | Detectar fuego y abrir la rejilla (actuador)              |                          |
| Electroválvula / bomba 9 V                            | 1        | Por revisar | Riego de emergencia, caudal 5 L/min. ()                   | RainBird                 |
| LED (actuador)                                        | 1        | Disponible  | Luz en lux, rango 0-65535.                                | DFRobot                  |
| MOTOR (servomotor//motor de paso) + puente (actuador) | 1        | Disponible  | Ventilación con rejilla                                   | DFRobot                  |
| Ventilador 5 V (actuador)                             | 1        | Pendiente   | Refrigeración gabinete o sistema.                         | Noctua                   |
| (SIRENA->Buzzer) + estrobo 9 V                        | 1        | Pendiente   | Alerta intrusión/humo/alarmas.                            | SECO-LARM                |
| Board relés 4 canales 5 V                             | 1        | Pendiente   | Control aspersor, buzzer, ventilador, reserva.            | Songle                   |
| Gabinete respiraderos                                 | 1        | Disponibl   | Protección clima, espacio sensores, sistema               | Hammond                  |
| Cableado + conectores                                 | varios   | Pendiente   | Evitar ruido en señales analógicas.                       | RS Components            |
| Tornillería, soportes, sellos                         | varios   | Pendiente   | Montaje interno/externo.                                  | Local                    |

## Acciones hardware próximas

- Confirmar disponibilidad de componentes pendientes (UPS, batería, relés) y colocar orden semana 1.
- Diseñar `hardware/wiring-map` y lista de corte de cables (Molex/JST) en semana 2.
- Preparar jig de pruebas para calibración MQ-2 y pluviómetro (semana 4).
- Evaluar protección adicional (spray conformal, tamaño desecante) antes de despliegue campo.
