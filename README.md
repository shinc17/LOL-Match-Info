# cli.gg

## A text-based C program to grab and display user and match information from League of Legends.

## Usage:

### Installation (Only if you want to work on your own machine, otherwise skip this step):

> Note: cURL must be installed on your system. If you are getting errors, make sure it's installed and updated. (https://help.ubidots.com/en/articles/2165289-learn-how-to-install-run-curl-on-windows-macosx-linux)

1. Install and compile `json-c` onto the system you're using (https://github.com/json-c/json-c) (follow the instructions in the repo for `cmake`ing the library) 
2. Install `git` and clone this repo, or download the files directly as a zip from github
3. Copy `riotapi.c`, and `riotapi.h` into your current working directory
4. Create a new C file
5. At the top of the C file, add the following lines of code:

```c
#include "riotapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
```

5. Sign up for a Riot Games account, then log into https://developer.riotgames.com/
6. Generate an API key, then copy it into the API_KEY variable in the `riotapi.c` file

### Working on the Ubuntu Machine

1. On the cli.gg github page, create a new branch (main > type `<branch_name>` > create branch)
2. SSH into the Ubuntu box: `ssh cnit315@cnit315project01.cit.lcl`
3. CD into the GH repo: `cd cli.gg-main/github/cli.gg`
4. Switch to your new branch (remember to do this every time you log into the machine!): `git checkout <branch_name>`
5. Type `git pull` to update if any changes have been made
6. Create a new C file
7. At the top of the C file, add the following lines of code:

```c
#include "riotapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
```

8. Sign up for a Riot Games account, then log into https://developer.riotgames.com/
9. Generate an API key, then copy it into the API_KEY variable in the `riotapi.c` file

#### Once you're done and want to commit:

1. Type `git add .`
2. Type `git commit -m "<summarize changes here>"
3. Type `git push` and provide authentication token


### Working with `riotapi.h`

Once everything is set up, you should be able to use functions defined in `riotapi.h`. The following is a list of the functions and how to call them and assign the result to variables. I'll continue to update this README as I add more functions to `riotapi.h`

#### `get_summoner_username(char * summoner_name)`

Returns a string of the user's username, given their username (kinda pointless, just use this for testing connectivity to the API)

Example:

```c
char * user;
user = get_summoner_username("MurphyTheTurtle");
/* user now holds "MurphyTheTurtle"
```

#### `get_summoner_puuid(char * summoner_name)`

Returns a string of the user's unique ID. This is important because many of the API endpoints require their ID (rather than username). The ID _must_ be declared statically, and then copied over using `strcpy` (see example).

Example:

```c
static char puuid[20];
const char * buffer;

buffer = get_summoner_puuid("MurphyTheTurtle");
strcpy(puuid, buffer);
/* This process should be repeated if you want a different ID: */
buffer = get_summoner_puuid("C9 Sneaky");
strcpy(puuid, buffer);
```

#### `char * get_recent_match(char* puuid, int recency)`

Returns the ID of a given user's match based on how recent it was (i.e. 1 is their most recent match).

Example:

```c
char * match;
match = get_recent_match(puuid, 1); /* assuming puuid is already defined and valid */
```

#### `char ** get_last_matches(char* puuid)`

Returns a user's last five matches as a char array, given the user's ID. Mostly a helper function. **The first match starts at index 1**. Always remember to call `free` on your `matches` array after you are done using it to avoid memory leaks!

> Note: If the given user does not have any recent matches, it will result in a segmentation fault. This will need to be addressed.
> TODO: Handle users with no match history

Example:

```c
int i;

char ** matches;
matches = get_last_matches(puuid) /* assuming puuid is already defined and valid */

for (i = 1; i < 6; i++)
{
    printf("%s\n", matches[i]);
}

free(matches);
```

#### `json_object * get_player_info(char* puuid, char* match_id);`

Returns a JSON object of a player from a match given their ID and a valid match ID. This is an important call because all subsequent queries for player data require a pointer to this JSON object. 

```c
json_object * player;
player = get_player_info(puuid, matches[0]); /* assuming puuid and matches are already defined and valid */

```

#### `int * get_match_kda(json_object * player)`

Returns a user's KDA ratio (kills, deaths, assists) in the form of an int array, given the user's ID and a valid match ID. The array is returned in the form:
`[kills, deaths, assists]`. 

Example: 

```c
int * kda;

kda = get_match_kda(player); /* Assuming player is already a valid JSON object */
printf("KDA: %d/%d/%d\n", kda[0], kda[1], kda[2]);
```

#### `double get_match_time(char* match_id);`

Returns the length of a match in seconds given the match ID. 

> Note: The length is returned in seconds. Convert the result on your own (i.e. divide by 60 to get the minutes)

Example:

```c 
double length;

length = get_match_time(matches[0]); /* Assuming matches is already defined and valid */
printf("Length: %f minutes\n", match_length / 60);
```

#### `int get_match_creepscore(json_object * player);`

Returns the number of creeps a player killed given the user's ID and a valid match ID. 

Example:

```c
int cs;

cs = get_match_creepscore(player); /* Assuming player is already a valid JSON object */
printf("CS: %d\n", cs);

```

#### `double get_match_date(char * match_id);`

Returns the date of a match given the match ID. 

> Note: The return value is a `double` known as a Unix Time Stamp. It is specifically the number of seconds that have passed since January 1st, 1970. Therefore, I also included a helper function called `print_match_date(double milliseconds)` which you can pass the result into to print it. The function converts it to seconds, then takes the Unix Time Stamp and calls some functions from `time.h` to convert it to to the format: `[Day of the week] [Year]-[Month]-[Day] [Timezone]`. 

Example:

```c
double match_date;

match_length = get_match_date(matches[0]); 
print_match_date(match_length); /* Example output: Sat 2021-10-30 EDT */
```

#### Compiling

I've been compiling with the following line of code:
`gcc scratchpad.c riotapi.c -lcurl -ljson-c -ansi -std=c99 -pedantic`

However, I'm not sure if this is what we are supposed to use for this class, so it might change. Make sure you are included `-lcurl`, `-ljson-c` and `riotapi.c` so it is properly linked.
