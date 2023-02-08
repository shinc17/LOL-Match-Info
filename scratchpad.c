#include "riotapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Example of function usage. This will repeatedly prompt the user for a username,
 * grab their last five matches, and print the ID of the most recent match.
 * Almost 100% sure it's vulnerable to buffer overflow but let's not talk about it.
 */
int main(void)
{
	printf("Entering main...\n");
	const char * buffer;
	char *user;
	static char puuid[80];
	char **matches;

	char username[17] = "";

	int *kda;
	double match_length;
	double match_date;
	int cs;
	int multikill;

	json_object *player;

	
	do
	{
		
		printf("Enter username: ");
		fgets(username, sizeof username, stdin);
		
		buffer = get_summoner_puuid(username);
		strcpy(puuid, buffer);
		
		printf("PUUID: %s\n", puuid);
		matches = get_last_matches(puuid);
		
		printf("First match: %s\n", matches[0]);
		player = get_player_info(puuid, matches[0]);

		kda = get_match_kda(player);
		printf("KDA: %d/%d/%d\n", kda[0], kda[1], kda[2]);

		match_length = get_match_time(matches[0]);
		printf("Length: %f minutes\n", match_length / 60);

		cs = get_match_creepscore(player);
		printf("CS: %d\n", cs);

		match_date = get_match_date(matches[0]);
		print_match_date(match_date);

		multikill = get_highest_multikill(player);
		printf("Highest multikill: %d\n", multikill);

		printf("Wards placed: %d\n", get_match_wardscore(player));
		free(matches);

	} while (1 == 1);

	return 0;
}
