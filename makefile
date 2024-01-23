CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = exporter

all: $(TARGET)

$(TARGET): exporter.c
	$(CC) $(CFLAGS) -o $(TARGET) exporter.c

clean:
	rm -f $(TARGET)
