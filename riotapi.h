#ifndef MEM_GUARD_H_
#define MEM_GUARD_H_

#include <json-c/json_object.h>

const char * get_summoner_username(char* summoner_name);
const char * get_summoner_puuid(char* summoner_name);
const char * get_recent_match(char* puuid, int recency);
char ** get_last_matches(char* puuid);
json_object * get_player_info(char* puuid, char* match_id);
int * get_match_kda(json_object* player);
double get_match_time(char* match_id);
int get_match_creepscore(json_object* player);
char ** get_summoner_spells(json_object* player);
int get_highest_multikill(json_object* player);
int get_match_wardscore(json_object* player);
char ** get_match_items(json_object* player);
int get_match_outcome(json_object* player);
double get_match_date(char * match_id);
void print_match_date(double milliseconds);

#endif

