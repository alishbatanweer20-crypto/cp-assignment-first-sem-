/* player.h - structure, constants, prototypes */  // a note: the name of this file

#define MAX_PLAYERS 100                   // whenever I write MAX_PLAYERS, it means 100 (we can keep 100 players)
#define FILE_NAME "player_records.txt"    // FILE_NAME means the name of our notebook file

struct Player                             // we are making a "player card" design
{                                         // open the card box
    int   id;                             // the player's own number
    char  name[30];                       // the player's name: up to 29 letters
    int   matches;                        // how many matches he played
    int   runs;                           // how many runs he made
    int   wickets;                        // how many wickets he took
    float average;                        // runs divided by matches (has a dot)
    int   role;                           // 1 = Batsman, 2 = Bowler, 3 = All-rounder
};                                        // close the card box (semicolon is needed here)

int  login(void);                         // tells everyone: a function "login" exists, it gives back a number
int  getInt(char prompt[], int min, int max);              // "getInt" asks a question and gives back a number
int  findById(struct Player p[], int count, int id);       // "findById" finds where a player sits
void swap(struct Player *a, struct Player *b);             // "swap" changes places of two players

void addPlayer(struct Player p[], int *count);             // adds a player
void displayAll(struct Player p[], int count);             // shows all players
void searchPlayer(struct Player p[], int count);           // looks for a player
void updatePlayer(struct Player p[], int count);           // changes a player's numbers
void deletePlayer(struct Player p[], int *count);          // removes a player
void sortAndReport(struct Player p[], int count);          // puts players in order and tells the best ones

void loadFile(struct Player p[], int *count);              // reads players from the notebook file
void saveFile(struct Player p[], int count);               // writes players into the notebook file