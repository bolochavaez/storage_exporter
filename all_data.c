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

#include "arc_snapshot.h"
#include "disk_snapshot.h"
#include "exporter_socket.h"

all_data_stats *calculate(arc_snapshot *prev, arc_snapshot *curr,
                          disk_snapshot **disk_prev,
                          disk_snapshot **disk_curr) {
  int val;
  regex_t regex;
  int reti;
  char msgbuf[100];
  disk_data_stats **all_disk_stats =
      (disk_data_stats **)malloc(sizeof(disk_data_stats *));
  if (all_disk_stats == NULL){
    perror("couldnt allocate memory: ");
    exit(1);
  }


  // disk_data_stats[0] = NULL;
  int list_size = 1;
  int last_list_index = 0;

  reti = regcomp(&regex, "sd[a-z]$", REG_EXTENDED);

  all_data_stats *data;
  data = (all_data_stats *)malloc(sizeof(all_data_stats));
  if (data == NULL){
    perror("couldnt allocate memory: ");
    exit(1);
  }

  while (*disk_prev) {
    if (!regexec(&regex, (*disk_prev)->device_name, 0, NULL, 0)) {
      list_size++;
      last_list_index++;
      all_disk_stats =
          realloc(all_disk_stats, sizeof(disk_data_stats *) * list_size);
      all_disk_stats[last_list_index] = NULL;
      all_disk_stats[last_list_index - 1] =
          (disk_data_stats *)malloc(sizeof(disk_data_stats));

  if (all_disk_stats[last_list_index - 1] == NULL){
    perror("couldnt allocate memory: ");
    exit(1);
  }


      strcpy(all_disk_stats[last_list_index - 1]->device_name,
             (*disk_prev)->device_name);
      all_disk_stats[last_list_index - 1]->writes_sec =
          ((*disk_curr)->sectors_written - (*disk_prev)->sectors_written) / 2;
    }
    disk_prev++;
    disk_curr++;
  }

  data->arc_hits = (prev->hits - curr->hits);
  data->arc_misses = (prev->misses - curr->misses);
  data->arc_c = curr->c;
  data->disk_data = all_disk_stats;
  return data;
}

int print_data(all_data_stats *data) {
  printf("{\n");
  printf("\"arc_data\": {\n");
  printf("\"hits/sec\" : %d, \n", data->arc_hits);
  printf("\"misses/sec\": %d, \n", data->arc_misses);
  printf("\"c\" : %d, \n", data->arc_c);
  printf("\"c_min\" : %d, \n", data->arc_c_min);
  printf("\"c_max\" : %d \n", data->arc_c_max);
  printf("},\n");

  printf("\"disk_stats\":{\n");
  disk_data_stats **disk_index = data->disk_data;
  while (*(disk_index)) {
    printf("\"%s\":{\n", (*disk_index)->device_name);
    printf("\"kbsec_write\":%d,\n", (*disk_index)->reads_sec);
    printf("\"kbsec_read\":%d\n", (*disk_index)->writes_sec);
    printf("}");
    disk_index++;
    if (*(disk_index))
      printf(",");
    printf("\n");
  }
  printf("}\n");
  printf("}\n");
  return 1;
}

int print_data_string(char *string, all_data_stats *data) {

  string += sprintf(string, "{\n");
  string += sprintf(string, "\"arc_data\": {\n");
  string += sprintf(string, "\"hits/sec\" : %d, \n", data->arc_hits);
  string += sprintf(string, "\"misses/sec\": %d, \n", data->arc_misses);
  string += sprintf(string, "\"c\" : %d, \n", data->arc_c);
  string += sprintf(string, "\"c_min\" : %d, \n", data->arc_c_min);
  string += sprintf(string, "\"c_max\" : %d \n", data->arc_c_max);
  string += sprintf(string, "},\n");

  string += sprintf(string, "\"disk_stats\":{\n");
  disk_data_stats **disk_index = data->disk_data;
  while (*(disk_index)) {
    string += sprintf(string, "\"%s\":{\n", (*disk_index)->device_name);
    string +=
        sprintf(string, "\"kbsec_write\":%d,\n", (*disk_index)->reads_sec);
    string += sprintf(string, "\"kbsec_read\":%d\n", (*disk_index)->writes_sec);
    string += sprintf(string, "}");
    disk_index++;
    if (*(disk_index))
      string += sprintf(string, ",");
    string += sprintf(string, "\n");
  }
  string += sprintf(string, "}\n");
  string += sprintf(string, "}\n");
  return 1;
}

