#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

typedef struct {
   char name[100];
   int value;
} arc_value;


typedef struct {
    char device_name[100];
    int reads_sec;
    int writes_sec;
} disk_snapshot;

typedef struct {
    int arc_hits;
    int arc_misses;
    int arc_c;
    int arc_c_min;
    int arc_c_max;
    int arc_prefetch_data_hits;
    int arc_prefetch_data_misses;
    disk_snapshot ** disk_data; 
} data_snapshot;


typedef struct {
    int major_number;
    int minor_number;
    char * device_name;
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
} disk_stats;




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
   
} arc_stats;

int get_list_value(char * name,  arc_value ** arc_stats_list){

    while (*arc_stats_list){
        if(!(strcmp(name,(*arc_stats_list)->name ))) return (*arc_stats_list)->value;
        arc_stats_list++;
        }

    return 0;
}

arc_stats * get_kstat_snapshot(){
    int type;
    int value;
    char property[100] = {0};
    arc_stats * stats = (arc_stats * ) malloc(sizeof(arc_stats));
    arc_value * values[150] = {NULL};
    FILE * arcstat_endpoint = fopen("/proc/spl/kstat/zfs/arcstats", "r");
    int index = 0;
    //skip thei first two trash lines
    fscanf(arcstat_endpoint, "%*[^\n]\n");
    fscanf(arcstat_endpoint, "%*[^\n]\n");
     while ( EOF != fscanf(arcstat_endpoint, "%s %d %d" , &property, &type, &value)){
        values[index] = (arc_value *)  malloc(sizeof(arc_value));
        values[index]->value = value;
        strcpy(values[index]->name, property);
        index ++;
    }
        stats->c = get_list_value("c", values);
        stats->hits= get_list_value("hits", values);
        stats->misses = get_list_value("misses", values);
        stats->demand_data_hits = get_list_value("demand_data_hits", values);
        stats->demand_data_misses = get_list_value("demand_data_misses", values);
        stats->demand_metadata_hits = get_list_value("demand_metadata_hits", values);
        stats->demand_metadata_misses = get_list_value("demand_metadata_misses", values);
        stats->prefetch_data_hits = get_list_value("prefetch_data_hits", values);
        stats->prefetch_data_misses = get_list_value("prefetch_data_misses", values);
        //stats->miss_p= get_list_value();
        //stats->dhit= get_list_value();
        //stats->dmis= get_list_value();
    fclose(arcstat_endpoint);
    return stats;
    return stats;
}



disk_stats **  get_diskstats_snapshot(){
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
    FILE * arcstat_endpoint = fopen("/proc/diskstats", "r");
    disk_stats ** disk_stat_list = malloc(sizeof(disk_stats*)); 
    disk_stat_list[0] = NULL;
    int last_index = 0; 
    int list_size = 1; 
    //skip thei first two trash lines
    fscanf(arcstat_endpoint, "%*[^\n]\n");
    fscanf(arcstat_endpoint, "%*[^\n]\n");
    
     while ( EOF != fscanf(arcstat_endpoint, "%d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &major_number, &minor_number, &device_name, &reads_success, &reads_merged, &sectors_read, &ms_reading, &writes_completed, &writes_merged, &sectors_written, &ms_writing, &ios_in_prog, &ms_doing_ios, &weighted_ms_doing_ios, &discards_success, &discards_merged, &sectors_discarded, &ms_discarding, &flush_success, &ms_flushing
 )){

        char * device_name_p = malloc(sizeof(char) * (strlen(device_name) + 1));
        strcpy(device_name_p, device_name);
        disk_stats * disk_stat_collection = malloc(sizeof(disk_stats));
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
        list_size ++;
        last_index ++;
        disk_stat_list = realloc(disk_stat_list, sizeof(disk_stats) * list_size );
        disk_stat_list[last_index - 1] = disk_stat_collection;
        disk_stat_list[last_index] = NULL;
     
    }
   
    fclose(arcstat_endpoint);
    return disk_stat_list;
}

data_snapshot * calculate(arc_stats* prev, arc_stats* curr, disk_stats ** disk_prev, disk_stats ** disk_curr){
        int val;
        regex_t regex;
        int reti;
        char msgbuf[100];
        disk_snapshot ** all_disk_stats = (disk_snapshot**) malloc(sizeof(disk_snapshot*));
        //disk_snapshot[0] = NULL;
        int list_size = 1;
        int last_list_index = 0;

        reti = regcomp(&regex,"sd[a-z]$",REG_EXTENDED); 
 
        data_snapshot *  data;
        data = (data_snapshot *) malloc(sizeof(data_snapshot));


        while(*disk_prev){
            if(!regexec(&regex, (*disk_prev)->device_name, 0,NULL,0)){
                 list_size ++;
                 last_list_index ++;
                 all_disk_stats = realloc(all_disk_stats, sizeof(disk_snapshot*) * list_size);
                 all_disk_stats[last_list_index] = NULL;
                 all_disk_stats[last_list_index - 1] = (disk_snapshot*) malloc(sizeof(disk_snapshot));
                 strcpy(all_disk_stats[last_list_index - 1]->device_name, (*disk_prev)->device_name);
                 all_disk_stats[last_list_index - 1]->writes_sec = ((*disk_curr)->sectors_written - (*disk_prev)->sectors_written) / 2;
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

int print_data(data_snapshot * data){
    printf("{\n");
    printf("\"arc_data\": {\n");
    printf("\"hits/sec\" : %d, \n", data->arc_hits);
    printf("\"misses/sec\": %d, \n", data->arc_misses);
    printf("\"c\" : %d, \n", data->arc_c);
    printf("\"c_min\" : %d, \n", data->arc_c_min);
    printf("\"c_max\" : %d, \n", data->arc_c_max);
    printf("}\n");

    printf("\"disk_stats\":{\n");
    disk_snapshot ** disk_index = data->disk_data;
    while(*(disk_index)){
         printf("\"%s\":{\n" , (*disk_index)->device_name);
         printf("\"kbsec_write\":%d\n" , (*disk_index)->reads_sec);
         printf("\"kbsec_read\":%d\n" , (*disk_index)->writes_sec);
         printf("}\n");
         disk_index++;
    }
    printf("}\n");
    printf("}\n");
    return 1;
}

int delete_data_buffer(data_snapshot * data){
    disk_snapshot ** disk_index = data->disk_data;
    while(*(disk_index)){
         free(*disk_index);
         disk_index ++;
    }
    free(data->disk_data);
    free(data);
    return 1;
}

int delete_arc_stats(arc_stats * arcstats){
    free(arcstats);

}

int delete_disk_stats(disk_stats ** disk_stat_list){
    disk_stats ** disk_index = disk_stat_list;
    while(*disk_index){
        free((*disk_index)->device_name);
        free(*disk_index);
        disk_index ++;
    }
    free(disk_stat_list);
}

int main(){
    data_snapshot * data_buffer;
    arc_stats * arcstat_prev;
    arc_stats * arcstat_curr;

    disk_stats ** disk_info_prev; 
    disk_stats ** disk_info_curr; 

    //initial snapshot
    disk_info_prev = get_diskstats_snapshot();
    arcstat_prev = get_kstat_snapshot();
    sleep(1);
    disk_info_curr = get_diskstats_snapshot();
    arcstat_curr = get_kstat_snapshot();
    for(int i =0; i < 10; i++){
         delete_arc_stats(arcstat_prev);
         delete_disk_stats(disk_info_prev);
         disk_info_prev = disk_info_curr;
         arcstat_prev = arcstat_curr;
         sleep(1);
         disk_info_curr = get_diskstats_snapshot();
         arcstat_curr = get_kstat_snapshot();
         data_buffer = calculate(arcstat_prev, arcstat_curr, disk_info_prev, disk_info_curr);
         print_data(data_buffer);
         delete_data_buffer(data_buffer);
     }
    return 1; 
}
