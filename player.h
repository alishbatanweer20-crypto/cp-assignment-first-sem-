/*
 * player.h
 * ---------------------------------------------------------------------------
 * Cricket Player Stats Management System  (CS-115, Lab 12 - Header Files)
 *
 * Contains:
 *   - constant macros
 *   - the Player structure
 *   - prototypes of every function implemented in player.c
 *
 * The inclusion guard (#ifndef / #define / #endif) prevents this header from
 * being processed twice if it is included from more than one place.
 * ---------------------------------------------------------------------------
 */
#ifndef PLAYER_H
#define PLAYER_H

/* ------------------------------ Constants ------------------------------- */
#define MAX_PLAYERS         100            /* capacity of the record array     */
#define NAME_LEN            30             /* name buffer size (incl. '\0')    */
#define INPUT_BUF_LEN       128            /* size of the raw keyboard buffer  */
#define DATA_FILE           "player_records.txt"

#define LOGIN_USER          "admin"        /* demo credentials (Security Login)*/
#define LOGIN_PASS          "cricket123"
#define MAX_LOGIN_ATTEMPTS  3

/* Player roles */
#define ROLE_BATSMAN        1
#define ROLE_BOWLER         2
#define ROLE_ALLROUNDER     3

/* Validation limits for statistics */
#define MAX_PLAYER_ID       999999
#define MAX_MATCHES         1000
#define MAX_RUNS            100000
#define MAX_WICKETS         5000

/* ------------------------------ Data types ------------------------------ */

/* One cricket player record */
typedef struct
{
    int   id;                    /* unique, positive player ID                 */
    char  name[NAME_LEN];        /* player name (no commas, non-empty)         */
    int   matches;               /* matches played                             */
    int   runs;                  /* total runs scored                          */
    int   wickets;               /* total wickets taken                        */
    float battingAverage;        /* runs / matches (calculated dynamically)    */
    int   role;                  /* ROLE_BATSMAN / ROLE_BOWLER / ROLE_ALLROUNDER */
} Player;

/* Criteria supported by the sorting module */
typedef enum
{
    SORT_BY_RUNS    = 1,
    SORT_BY_AVERAGE = 2
} SortCriterion;

/* ------------------------- Security / Login ----------------------------- */
int  login(void);

/* ------------------------ Input helper functions ------------------------ */
int  readInt(const char *prompt, int min, int max);
void readName(const char *prompt, char *dest);
int  readYesNo(const char *prompt);

/* ----------------------------- Utilities -------------------------------- */
float       calculateBattingAverage(const Player *p);
const char *roleToString(int role);

/* ------------------------ Core record operations ------------------------ */
void addPlayer(Player *players, int *count);
void displayAllPlayers(const Player *players, int count);
void searchPlayer(const Player *players, int count);
void updatePlayer(Player *players, int count);
void deletePlayer(Player *players, int *count);
void sortAndAnalytics(const Player *players, int count);

/* --------------------------- File persistence --------------------------- */
int loadPlayers(Player *players, int *count);
int savePlayers(const Player *players, int count);

#endif /* PLAYER_H */
