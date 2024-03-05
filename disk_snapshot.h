#include <assert.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
  int major_number;
  int minor_number;
  char *device_name;
  int reads_success;
  int reads_merged;
  int sectors_read;
  int ms_reading;
  int writes_completed;
  int writes_merged;
  int sectors_written;
  int ms_writing;
  int ios_in_prog;
  int ms_doing_ios;
  int weighted_ms_doing_ios;
  int discards_success;
  int discards_merged;
  int sectors_discarded;
  int ms_discarding;
  int flush_success;
  int ms_flushing;
} disk_snapshot;


disk_snapshot **get_diskstats_snapshot();
