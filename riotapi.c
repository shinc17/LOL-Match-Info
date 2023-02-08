#include "riotapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <json-c/json.h>

char API_KEY[43] = "RGAPI-d2014430-8f5b-491d-b8fe-aba2e1720b39";

struct MemoryStruct
{
	char * memory;
	size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char * ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr)
	{
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

char * QUERY_API(char *URL)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk.size = 0;			  /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, URL);

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	/* some servers don't like requests that are made without a user-agent
        field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);

	/* check for errors */
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		return "";
	}
	else
	{
		int result;
		return chunk.memory;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();
}

char *BUILD_BASE_URL(char *API_ENDPOINT, char *PARAM1)
{
	char *URL = malloc(400);

	char BASE[70], PARAM[100];
	strcpy(BASE, API_ENDPOINT);
	strcpy(PARAM, PARAM1);
	strcat(URL, BASE);
	strcat(URL, PARAM);
	return URL;
}

void addChar(char string[], char c)
{
	int length;
	length = strlen(string);
	string[length] = c;
	string[length + 1] = '\0';
}

void pretty_print_json(char *JSON)
{
	struct json_object *jobj;
	jobj = json_tokener_parse(JSON);
	printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

char *FILL_BLANKS(char *summoner_name)
{
	int i;
	char buffer[40] = "";
	for (i = 0; i < strlen(summoner_name); i++)
	{
		if (summoner_name[i] == ' ' || summoner_name[i] == '\n')
		{
			strcat(buffer, "%20");
		}
		else
		{
			addChar(buffer, summoner_name[i]);
		}
	}
	char * bufferPtr = buffer;
	return bufferPtr;
}

const char *get_summoner_username(char *summoner_name)
{
	char * formatted_username = FILL_BLANKS(summoner_name);
	char API_BASE[10] = "";
	char * URL;

	URL = BUILD_BASE_URL("https://na1.api.riotgames.com/lol/summoner/v4/summoners/by-name/", formatted_username);
	free(formatted_username);
	strcpy(API_BASE, "?api_key=");
	strcat(URL, API_BASE);
	strcat(URL, API_KEY);

	char *DATA = QUERY_API(URL);
	free(URL);
	struct json_object *jobj;
	jobj = json_tokener_parse(DATA);
	const char *username = json_object_get_string(json_object_object_get(jobj, "name"));
	return username;
}

const char *get_summoner_puuid(char *summoner_name)
{
	char * formatted_username = FILL_BLANKS(summoner_name);
	char API_BASE[10] = "";
	char URL[275] = "";

	strcat(URL, "https://na1.api.riotgames.com/lol/summoner/v4/summoners/by-name/");
	strcat(URL, formatted_username);
	strcpy(API_BASE, "?api_key=");
	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	char * DATA = QUERY_API(URL);
	struct json_object *jobj;
	jobj = json_tokener_parse(DATA);
	const char *puuid = json_object_get_string(json_object_object_get(jobj, "puuid"));
	return puuid;
}

const char *get_recent_match(char *puuid, int recency)
{
	char API_BASE[10] = "";
	char API_OPTIONS[30] = "";
	char PUUID[80] = "";
	strcpy(PUUID, puuid);
	char * URL = "";

	URL = BUILD_BASE_URL("https://americas.api.riotgames.com/lol/match/v5/matches/by-puuid/", PUUID);
	strcpy(API_BASE, "api_key=");
	strcpy(API_OPTIONS, "/ids?start=0&count=5");

	strcat(URL, API_OPTIONS);
	strcat(URL, "&");
	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	printf("%s\n", URL);

	char * DATA = QUERY_API(URL);
	printf("Getting new match...\n");

	free(URL);
	struct json_object *jobj;
	jobj = json_tokener_parse(DATA);
	const char *match = json_object_get_string(json_object_array_get_idx(jobj, recency));

	return match;
}

char **get_last_matches(char *puuid)
{
	int i;
	char API_BASE[10] = "";
	char API_OPTIONS[30] = "";
	char PUUID[80] = "";
	strcpy(PUUID, puuid);
	char URL[275] = "";
	strcat(URL, "https://americas.api.riotgames.com/lol/match/v5/matches/by-puuid/\0");
	strcat(URL, PUUID);
	strcpy(API_BASE, "api_key=");
	strcpy(API_OPTIONS, "/ids?start=0&count=5");

	strcat(URL, API_OPTIONS);
	strcat(URL, "&");
	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	printf("%s\n", URL);

	char * DATA = QUERY_API(URL);
	printf("Getting new matches...\n");

	struct json_object *jobj;
	jobj = json_tokener_parse(DATA);
	json_object *match_array = json_object_get(jobj);

	char ** matches;
	matches = malloc(5 * sizeof(char **));

	for (i = 0; i < 5; i++)
	{
		matches[i] = malloc((25) * sizeof(char **));
		const char *thisMatch = json_object_get_string(json_object_array_get_idx(match_array, i));
		printf("%s\n", thisMatch);
		strcpy(matches[i], thisMatch);
	}

	char **matchesPtr = matches;
	return matchesPtr;
}

json_object * get_player_info(char *puuid, char *match_id)
{
	char API_BASE[10] = "";
	char URL[275] = "";

	strcat(URL, "https://americas.api.riotgames.com/lol/match/v5/matches/");
	strcat(URL, match_id);
	strcpy(API_BASE, "?api_key=");

	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	printf("%s\n", URL);

	char * DATA = QUERY_API(URL);

	struct json_object *jobj;
	jobj = json_tokener_parse(DATA);
	json_object * info = json_object_object_get(jobj, "info");
	json_object * participants = json_object_object_get(info, "participants");
	int i;
	json_object * player;
	for (i = 0; i < 10; i++)
	{
		player = json_object_array_get_idx(participants, i);
		const char *thisId = json_object_get_string(json_object_object_get(player, "puuid"));
		if (strcmp(thisId, puuid) == 0)
		{
			return player;
		}
	}
	return player;
}

int * get_match_kda(json_object *player)
{
	int kda[3];
	int kills = atoi(json_object_get_string(json_object_object_get(player, "kills")));
	int deaths = atoi(json_object_get_string(json_object_object_get(player, "deaths")));
	int assists = atoi(json_object_get_string(json_object_object_get(player, "assists")));

	kda[0] = kills;
	kda[1] = deaths;
	kda[2] = assists;

	int * kdaPtr = kda;
	return kdaPtr;
}

double get_match_time(char *match_id)
{
	char API_BASE[10] = "";
	char URL[275] = "";

	strcat(URL, "https://americas.api.riotgames.com/lol/match/v5/matches/");
	strcat(URL, match_id);
	strcpy(API_BASE, "?api_key=");

	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	printf("%s\n", URL);

	char * DATA = QUERY_API(URL);


	json_object * jobj;
	jobj = json_tokener_parse(DATA);
	json_object * info = json_object_object_get(jobj, "info");
	double match_length;
	match_length = atof(json_object_get_string(json_object_object_get(info, "gameDuration")));
	return match_length;
}

int get_match_creepscore(json_object *player)
{
	int cs = json_object_get_int(json_object_object_get(player, "totalMinionsKilled"));
	return cs;
}
double get_match_date(char *match_id)
{
	char API_BASE[10] = "";
	char URL[275] = "";

	strcat(URL,"https://americas.api.riotgames.com/lol/match/v5/matches/"); 
	strcat(URL, match_id);
	strcpy(API_BASE, "?api_key=");

	strcat(URL, API_BASE);
	strcat(URL, API_KEY);
	printf("%s\n", URL);

	char * DATA = QUERY_API(URL);


	json_object * jobj;
	jobj = json_tokener_parse(DATA);
	json_object * info = json_object_object_get(jobj, "info");
	double game_date;
	game_date = json_object_get_double(json_object_object_get(info, "gameStartTimestamp"));
	return game_date;
}

void print_match_date(double milliseconds)
{
	time_t rawtime = milliseconds / 1000;
	struct tm ts;
	char buf[80];

	ts = * localtime(&rawtime);
	strftime(buf, sizeof(buf), "%a %Y-%m-%d %Z", &ts);
	printf("%s\n", buf);
}

int get_highest_multikill(json_object *player)
{
	int multikill = json_object_get_int(json_object_object_get(player, "largestMultiKill"));
	return multikill;
}

int get_match_wardscore(json_object* player)
{
	int wards = json_object_get_int(json_object_object_get(player, "wardsPlaced"));
	return wards;
}



