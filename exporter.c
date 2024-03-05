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
  char device_name[100];
  int reads_sec;
  int writes_sec;
} disk_data_stats;

typedef struct {
  int arc_hits;
  int arc_misses;
  int arc_c;
  int arc_c_min;
  int arc_c_max;
  int arc_prefetch_data_hits;
  int arc_prefetch_data_misses;
  disk_data_stats **disk_data;
} all_data_stats;

int get_list_value(char *name, arc_value **arc_stats_list) {

  while (*arc_stats_list) {
    if (!(strcmp(name, (*arc_stats_list)->name)))
      return (*arc_stats_list)->value;
    arc_stats_list++;
  }

  return 0;
}


int delete_arc_stats(arc_snapshot *arcstats) { free(arcstats); }

int delete_disk_snapshot(disk_snapshot **disk_stat_list) {
  disk_snapshot **disk_index = disk_stat_list;
  while (*disk_index) {
    free((*disk_index)->device_name);
    free(*disk_index);
    disk_index++;
  }
  free(disk_stat_list);
}

int main(int argc, char *argv[]) {
  if (argc < 2){
  printf("you need to supply a destination");
  exit(1);
  }

  int socket_fd = get_exporter_socket(argv[1]);
  all_data_stats *data_buffer;
  arc_snapshot *arcstat_prev;
  arc_snapshot *arcstat_curr;

  // set aside  a 1 MB message size.
  char msg_buffer[1000000] = {0};
  disk_snapshot **disk_info_prev;
  disk_snapshot **disk_info_curr;

  // initial snapshot
  disk_info_prev = get_diskstats_snapshot();
  arcstat_prev = get_kstat_snapshot();
  sleep(1);
  disk_info_curr = get_diskstats_snapshot();
  arcstat_curr = get_kstat_snapshot();

  for (int i = 0; i < 1; i++) {
    delete_arc_stats(arcstat_prev);
    delete_disk_snapshot(disk_info_prev);
    disk_info_prev = disk_info_curr;
    arcstat_prev = arcstat_curr;
    sleep(1);
    disk_info_curr = get_diskstats_snapshot();
    arcstat_curr = get_kstat_snapshot();
    data_buffer =
        calculate(arcstat_prev, arcstat_curr, disk_info_prev, disk_info_curr);
    print_data_string(msg_buffer, data_buffer);
    delete_data_buffer(data_buffer);

    send_stats(msg_buffer, socket_fd);
    close(socket_fd);
  }
  return 1;
}
