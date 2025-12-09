# WeatherShield Bill of Materials (BOM)

| Item                                                  | Cantidad | Estado                     | Especificación / Notas                                    | Proveedor objetivo       |
| ----------------------------------------------------- | -------- | -------------------------- | --------------------------------------------------------- | ------------------------ |
| Raspberry Pi 3 Model B (4 GB)                         | 1        | Disponible                 | Nodo principal, requiere disipadores y carcasa ventilada. | Stock personal / CanaKit |
| Tarjeta microSD 32 GB A2                              | 1        | Disponible                 | Sistema operativo y logs.                                 | Kingston / Sandisk       |
| Batería AGM 9V                                        | 1        | NO IMPLEMENTADO            | Autonomía 12 h; evaluar SLA vs LiFePO4.                   | LTH / Trojan             |
| Módulo MQ-2 + carcasa ventilada                       | 1        | Disponible                 | Detecta humo/GLP/Gasolina, requiere calibración.          | Telemundo                |
| Sensor MQ-135 Gas sensor                              | 1        | Disponible                 | Luz en lux, rango 0-65535.                                | Telemundo                |
| Sensor DS18B20 encapsulado                            | 1        | Por Revisar                | Temp exterior resistente agua. (temp y humedad)           | Telemundo                |
| Fotocelda                                             | 1        | Por Revisar                | Luz en lux, rango 0-65535.                                | Telemundo                |
| SENSOR DE FLAMA                                       | 1        | Disponible                 | Detectar fuego y abrir la rejilla (actuador)              | Telemundo                |
| Electroválvula / bomba 9 V                            | 1        | Disponible NO IMPLEMENTADO | Riego de emergencia, caudal 5 L/min. ()                   | Telemundo                |
| LED (actuador)                                        | 1        | Disponible                 | Luz en lux, rango 0-65535.                                | Telemundo                |
| MOTOR (servomotor//motor de paso) + puente (actuador) | 1        | Disponible NO IMPLEMENTADO | Ventilación con rejilla                                   | Telemundo                |
| Ventilador 5 V (actuador)                             | 1        | Disponible NO IMPLEMENTADO | Refrigeración gabinete o sistema.                         | Telemundo                |
| (SIRENA->Buzzer) + estrobo 9 V                        | 1        | Dipsonible                 | Alerta intrusión/humo/alarmas.                            | Telemundo                |
| Cableado + conectores                                 | varios   | Pendiente                  | Evitar ruido y ejercer estabilidad                        | Telemundo                |

## Acciones hardware próximas

- Confirmar disponibilidad de componentes pendientes (UPS, batería, relés) y colocar orden semana 1.
- Diseñar `hardware/wiring-map` y lista de corte de cables (Molex/JST) en semana 2.
- Preparar jig de pruebas para implemetación de sensores (semana 3)
- Evaluar protección adicional (spray conformal, tamaño desecante) antes de despliegue campo.
