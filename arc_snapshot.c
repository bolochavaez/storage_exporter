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
  int hits;
  int misses;
  int demand_data_hits;
  int demand_data_misses;
  int demand_metadata_hits;
  int demand_metadata_misses;
  int prefetch_data_hits;
  int prefetch_data_misses;
  int prefetch_metadata_hits;
  int prefetch_metadata_misses;
  int mru_hits;
  int mru_ghost_hits;
  int mfu_hits;
  int mfu_ghost_hits;
  int deleted;
  int mutex_miss;
  int access_skip;
  int evict_skip;
  int evict_not_enough;
  int evict_l2_cached;
  int evict_l2_eligible;
  int evict_l2_eligible_mfu;
  int evict_l2_eligible_mru;
  int evict_l2_ineligible;
  int evict_l2_skip;
  int hash_elements;
  int hash_elements_max;
  int hash_collisions;
  int hash_chains;
  int hash_chain_max;
  int p;
  int c;
  int c_min;
  int c_max;

} arc_snapshot;

arc_snapshot *get_kstat_snapshot() {
  int type;
  int value;
  char property[100] = {0};
  arc_snapshot *stats = (arc_snapshot *)malloc(sizeof(arc_snapshot));

  if (stats == NULL){
    perror("couldnt allocate memory: ");
    exit(1);
  }

  arc_value *values[150] = {NULL};
  FILE *arcstat_endpoint = fopen("/proc/spl/kstat/zfs/arcstats", "r");
  if (arcstat_endpoint == NULL) {
      perror("Could not open /proc/spl/kstat/zfs/arcstats arcstat: ");
      exit(1);
  }


  int index = 0;
  // skip thei first two trash lines
  fscanf(arcstat_endpoint, "%*[^\n]\n");
  fscanf(arcstat_endpoint, "%*[^\n]\n");
  while (EOF !=
         fscanf(arcstat_endpoint, "%s %d %d", &property, &type, &value)) {
    values[index] = (arc_value *)malloc(sizeof(arc_value));
      if (values[index] == NULL){
        perror("couldnt allocate memory: ");
      exit(1);
      }
    
    values[index]->value = value;
    strcpy(values[index]->name, property);
    index++;
  }
  stats->c = get_list_value("c", values);
  stats->hits = get_list_value("hits", values);
  stats->misses = get_list_value("misses", values);
  stats->demand_data_hits = get_list_value("demand_data_hits", values);
  stats->demand_data_misses = get_list_value("demand_data_misses", values);
  stats->demand_metadata_hits = get_list_value("demand_metadata_hits", values);
  stats->demand_metadata_misses =
      get_list_value("demand_metadata_misses", values);
  stats->prefetch_data_hits = get_list_value("prefetch_data_hits", values);
  stats->prefetch_data_misses = get_list_value("prefetch_data_misses", values);
  // stats->miss_p= get_list_value();
  // stats->dhit= get_list_value();
  // stats->dmis= get_list_value();
  fclose(arcstat_endpoint);
  return stats;
}
