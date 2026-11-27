// player.h - player card design, constants and function names

#define MAX_PLAYERS 100                   // we can keep 100 players
#define FILE_NAME "player_records.txt"    // the name of our notebook file

struct Player                             // the player card design
{
    int   id;                             // the player's own number
    char  name[30];                       // the player's name (up to 29 letters)
    int   matches;                        // how many matches he played
    int   runs;                           // how many runs he made
    int   wickets;                        // how many wickets he took
    float average;                        // runs divided by matches
    int   role;                           // 1 = Batsman, 2 = Bowler, 3 = All-rounder
};

int  login(void);                                          // checks user name and password
int  getInt(char prompt[], int min, int max);              // asks a question, gives back a good number
int  findById(struct Player p[], int count, int id);       // finds where a player sits
void swap(struct Player *a, struct Player *b);             // two players change seats

void addPlayer(struct Player p[], int *count);             // adds a player
void displayAll(struct Player p[], int count);             // shows all players
void searchPlayer(struct Player p[], int count);           // looks for a player
void updatePlayer(struct Player p[], int count);           // changes a player's numbers
void deletePlayer(struct Player p[], int *count);          // removes a player
void sortAndReport(struct Player p[], int count);          // puts players in order and shows the best ones

void loadFile(struct Player p[], int *count);              // reads players from the notebook file
void saveFile(struct Player p[], int count);               // writes players into the notebook file