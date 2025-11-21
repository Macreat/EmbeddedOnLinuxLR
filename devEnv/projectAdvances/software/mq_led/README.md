# MQ sensor (salida digital) -> LED

Utilidad simple en C para Raspberry Pi 3 Model B (Debian) que lee el pin digital del sensor MQ y conmuta un LED. Usa la interfaz sysfs de GPIO (sin dependencias externas).

### Pines por defecto (BCM)
- MQ digital: GPIO23
- LED: GPIO24 (activo en alto)

### Compilacion
```bash
cd repository/EmbeddedOnLinuxLR/devEnv/projectAdvances/software/mq_led
make        # genera ./mq_led
```

### Ejecucion (requiere permisos de root o acceso a /sys/class/gpio)
```bash
sudo ./mq_led --mq-pin 23 --led-pin 24
```

Opciones utiles:
- `--mq-pin <N>`: GPIO BCM donde llega el pin digital del sensor.
- `--led-pin <N>`: GPIO BCM para el LED.
- `--led-active-low`: usar si el LED enciende con nivel bajo.
- `--edge <both|rising|falling>`: tipo de flanco a vigilar (por defecto both).

Al iniciar se configura el estado del LED segun el nivel actual del sensor y luego reacciona a cambios de nivel. Presiona `Ctrl+C` para salir; el programa libera los GPIO exportados.
