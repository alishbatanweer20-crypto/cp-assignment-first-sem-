/* player.h - structure, constants, prototypes */

#define MAX_PLAYERS 100
#define FILE_NAME "player_records.txt"

struct Player
{
    int   id;
    char  name[30];
    int   matches;
    int   runs;
    int   wickets;
    float average;      /* runs / matches */
    int   role;         /* 1 = Batsman, 2 = Bowler, 3 = All-rounder */
};

int  login(void);
int  getInt(char prompt[], int min, int max);
int  findById(struct Player p[], int count, int id);
void swap(struct Player *a, struct Player *b);

void addPlayer(struct Player p[], int *count);
void displayAll(struct Player p[], int count);
void searchPlayer(struct Player p[], int count);
void updatePlayer(struct Player p[], int count);
void deletePlayer(struct Player p[], int *count);
void sortAndReport(struct Player p[], int count);

void loadFile(struct Player p[], int *count);
void saveFile(struct Player p[], int count);