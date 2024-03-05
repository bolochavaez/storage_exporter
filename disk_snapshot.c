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


disk_snapshot **get_diskstats_snapshot() {
  int major_number;
  int minor_number;
  char device_name[100] = {0};
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
  FILE *arcstat_endpoint = fopen("/proc/diskstats", "r");
  if (arcstat_endpoint == NULL) {
      perror("Could not open /proc/diskstats: ");
      exit(1);
  }


  disk_snapshot **disk_stat_list = malloc(sizeof(disk_snapshot *));
  if (disk_stat_list == NULL){
    perror("couldnt allocate memory: ");
      exit(1);
  }

  disk_stat_list[0] = NULL;
  int last_index = 0;
  int list_size = 1;
  // skip thei first two trash lines
  fscanf(arcstat_endpoint, "%*[^\n]\n");
  fscanf(arcstat_endpoint, "%*[^\n]\n");

  while (EOF !=
         fscanf(arcstat_endpoint,
                "%d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                &major_number, &minor_number, &device_name, &reads_success,
                &reads_merged, &sectors_read, &ms_reading, &writes_completed,
                &writes_merged, &sectors_written, &ms_writing, &ios_in_prog,
                &ms_doing_ios, &weighted_ms_doing_ios, &discards_success,
                &discards_merged, &sectors_discarded, &ms_discarding,
                &flush_success, &ms_flushing)) {

    char *device_name_p = malloc(sizeof(char) * (strlen(device_name) + 1));
  if (device_name_p  == NULL){
    perror("couldnt allocate memory: ");
      exit(1);
  }


    strcpy(device_name_p, device_name);
    disk_snapshot *disk_stat_collection = malloc(sizeof(disk_snapshot));
  if (disk_stat_collection == NULL){
    perror("couldnt allocate memory: ");
      exit(1);
  }

    disk_stat_collection->major_number = major_number;
    disk_stat_collection->minor_number = minor_number;
    disk_stat_collection->device_name = device_name_p;
    disk_stat_collection->reads_success = reads_success;
    disk_stat_collection->reads_merged = reads_merged;
    disk_stat_collection->sectors_read = sectors_read;
    disk_stat_collection->ms_reading = ms_reading;
    disk_stat_collection->writes_completed = writes_completed;
    disk_stat_collection->writes_merged = writes_merged;
    disk_stat_collection->sectors_written = sectors_written;
    disk_stat_collection->ms_writing = ms_writing;
    disk_stat_collection->ios_in_prog = ios_in_prog;
    disk_stat_collection->ms_doing_ios = ms_doing_ios;
    disk_stat_collection->weighted_ms_doing_ios = weighted_ms_doing_ios;
    disk_stat_collection->discards_success = discards_success;
    disk_stat_collection->discards_merged = discards_merged;
    disk_stat_collection->sectors_discarded = sectors_discarded;
    disk_stat_collection->ms_discarding = ms_discarding;
    disk_stat_collection->flush_success = flush_success;
    disk_stat_collection->ms_flushing = ms_flushing;
    list_size++;
    last_index++;
    disk_stat_list = realloc(disk_stat_list, sizeof(disk_snapshot) * list_size);
    disk_stat_list[last_index - 1] = disk_stat_collection;
    disk_stat_list[last_index] = NULL;
  }

  fclose(arcstat_endpoint);
  return disk_stat_list;
}
