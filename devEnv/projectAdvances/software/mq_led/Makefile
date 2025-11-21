CC      := gcc
CFLAGS  := -Wall -Wextra -O2
TARGET  := mq_led

all: $(TARGET)

$(TARGET): mq_led.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
