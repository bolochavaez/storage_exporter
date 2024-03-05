CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = exporter

all: $(TARGET)

$(TARGET): exporter.c
	$(CC) $(CFLAGS) -o $(TARGET) arc_snapshot.c disk_snapshot.c all_data.c exporter_socket.c exporter.c 

clean:
	rm -f $(TARGET)
