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

typedef struct {
  char name[100];
  int value;
} arc_value;



arc_snapshot *get_kstat_snapshot();
