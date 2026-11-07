// player.c - all the work happens here
#include <stdio.h>      // printf and scanf (talking and listening)
#include <stdlib.h>     // exit (the stop button)
#include <string.h>     // strcmp (compare two words)
#include "player.h"     // our player card design and function names

// getInt: asks a question until you type a good number
int getInt(char prompt[], int min, int max)
{
    int value, result, ch;      // value = your number, result = did it work, ch = one letter

    while (1)                   // keep asking again and again
    {
        printf("%s", prompt);           // say the question
        result = scanf("%d", &value);   // listen for a number (result is 1 if it worked)
        if (result == EOF)              // if the typing door is closed
        {
            exit(1);                    // stop the program
        }
        while ((ch = getchar()) != '\n' && ch != EOF)   // pick up leftover letters until the line ends
        {
            // throw them away
        }
        if (result != 1)                // you typed a letter, not a number
        {
            printf("Please enter a number.\n");
        }
        else if (value < min || value > max)    // number too small or too big
        {
            printf("Number must be from %d to %d.\n", min, max);
        }
        else                            // number is good
        {
            return value;               // give it back and finish
        }
    }
}

// login: checks the secret name and password (3 tries)
int login(void)
{
    char user[30], pass[30];    // two word boxes
    int i;                      // counting box

    for (i = 1; i <= 3; i++)    // count 1, 2, 3
    { // we arent using & in scanf bcz user already means “address of the first letter” as it is an array
        printf("Username: ");   // ask the user name
        scanf("%29s", user);    // listen for one word
        printf("Password: ");   // ask the password 
        scanf("%29s", pass);    // listen for one word
        if (strcmp(user, "admin") == 0 && strcmp(pass, "cricket123") == 0)   // both words must be the same
        {
            return 1;           // yes, you may come in
        }
        printf("Wrong login.\n");
    }
    return 0;                   // no, 3 tries are finished
}

// findById: finds where a player sits (-1 means not found)
int findById(struct Player p[], int count, int id) //beinng used in recurssion in addPlayer, searchPlayer, updatePlayer, deletePlayer
{
    int i;                          // counting box
    for (i = 0; i < count; i++)     // look at every player
    {
        if (p[i].id == id)          // is this the id we want?, p[i].id is struct(means id of palyer at index i)
        {
            return i;               // give back his place number
        }
    }
    return -1;                      // nobody had that id
}

// swap: two players change seats (uses pointers)
void swap(struct Player *a, struct Player *b)
{
    struct Player temp = *a;    // copy a into a spare box
    *a = *b;                    // put b in a's place
    *b = temp;                  // put the spare copy in b's place
}

// printRow: prints one player as one line of the table - row with all deets of each player
void printRow(struct Player *p)
{
    printf("%-6d %-15s %-8d %-8d %-8d %-8.2f %d\n", p->id, p->name,
           p->matches, p->runs, p->wickets, p->average, p->role);
}

// printHeader: prints the heading of the table
void printHeader(void)
{
    printf("\nID     Name            Matches  Runs     Wickets  Average  Role(1=Bat 2=Bowl 3=All)\n");
}

// addPlayer: adds a new player - main ****CASE 1*****
void addPlayer(struct Player p[], int *count)
{
    struct Player n;                // a new empty player card

    if (*count == MAX_PLAYERS)      // no space left
    {
        printf("List is full.\n");
        return;
    }
    n.id = getInt("ID: ", 1, 999999);           // ask the id
    if (findById(p, *count, n.id) != -1)        // id already used
    {
        printf("This ID already exists.\n");
        return;
    }
    printf("Name (one word): ");                // ask the name
    scanf("%29s", n.name);                      // listen for one word
    n.role    = getInt("Role (1=Batsman 2=Bowler 3=All-rounder): ", 1, 3);   // only 1, 2 or 3
    n.matches = getInt("Matches: ", 1, 1000);   // at least 1 match
    n.runs    = getInt("Runs: ", 0, 100000);
    n.wickets = getInt("Wickets: ", 0, 5000);
    n.average = (float)n.runs / n.matches;      // runs divided by matches

    p[*count] = n;                  // put the card in the next empty seat
    (*count)++;                     // add 1 to the counter
    printf("Player added.\n");
}

// displayAll: shows every player - main ********CASE 2*****
void displayAll(struct Player p[], int count)
{
    int i;                          // counting box
    if (count == 0)                 // nobody to show
    {
        printf("No records.\n");
        return;
    }
    printHeader();                  // print the heading
    for (i = 0; i < count; i++)     // go through every player
    {
        printRow(&p[i]);            // print his line
    }
}

// searchPlayer: looks for a player by id or by name - main ********CASE 3*****
void searchPlayer(struct Player p[], int count)
{
    int i, found = 0, choice, id;   // found = 0 means not found yet
    char name[30];                  // a box for a name

    choice = getInt("Search by 1=ID, 2=Name: ", 1, 2);   // ask how to search
    if (choice == 1)                // search by id
    {
        id = getInt("ID: ", 1, 999999);
        i = findById(p, count, id);     // find his place
        if (i != -1)                    // he was found
        {
            printHeader();
            printRow(&p[i]);
            found = 1;                  // remember: found
        }
    }
    else                            // search by name
    {
        printf("Name: ");
        scanf("%29s", name);
        for (i = 0; i < count; i++)             // look at every player
        {
            if (strcmp(p[i].name, name) == 0)   // same word?
            {
                printHeader();
                printRow(&p[i]);
                found = 1;                      // remember: found
            }
        }
    }
    if (found == 0)                 // still not found
    {
        printf("Not found.\n");
    }
}

// updatePlayer: changes matches, runs and wickets of one player
void updatePlayer(struct Player p[], int count)
{
    int i = findById(p, count, getInt("ID to update: ", 1, 999999));   // ask id, find his place

    if (i == -1)                    // nobody has that id
    {
        printf("Not found.\n");
        return;
    }
    p[i].matches = getInt("New matches: ", 1, 1000);
    p[i].runs    = getInt("New runs: ", 0, 100000);
    p[i].wickets = getInt("New wickets: ", 0, 5000);
    p[i].average = (float)p[i].runs / p[i].matches;     // work out the average again
    printf("Updated.\n");
}

// deletePlayer: removes a player
void deletePlayer(struct Player p[], int *count)
{
    int i = findById(p, *count, getInt("ID to delete: ", 1, 999999));   // ask id, find his place

    if (i == -1)                    // nobody has that id
    {
        printf("Not found.\n");
        return;
    }
    for (; i < *count - 1; i++)     // start from his seat (learn from array element deletion)
    {
        p[i] = p[i + 1];            // next player moves one seat up
    }
    (*count)--;                     // take 1 away from the counter
    printf("Deleted.\n");
}

// sortAndReport: puts players in order and shows the best ones
void sortAndReport(struct Player p[], int count)
{
    int i, j, choice;                       // two counters and your choice
    struct Player *topRuns = &p[0];         // finger pointing at the best runs so far
    struct Player *topWickets = &p[0];      // finger pointing at the best wickets so far

    if (count == 0)                 // nobody to sort
    {
        printf("No records.\n");
        return;
    }
    choice = getInt("Sort by 1=Runs, 2=Batting average: ", 1, 2);

    // bubble sort: biggest goes on top
    for (i = 0; i < count - 1; i++)             // many rounds
    {
        for (j = 0; j < count - 1 - i; j++)     // compare neighbours
        {
            if (choice == 1 && p[j].runs < p[j + 1].runs)           // left has fewer runs
            {
                swap(&p[j], &p[j + 1]);                             // change seats
            }
            if (choice == 2 && p[j].average < p[j + 1].average)     // left has smaller average
            {
                swap(&p[j], &p[j + 1]);                             // change seats
            }
        }
    }
    displayAll(p, count);           // show the sorted table

    // find the best players
    for (i = 1; i < count; i++)
    {
        if (p[i].runs > topRuns->runs)              // more runs than the best so far  - arrow is used bcz topruns is a pointer so the arrow tells it to go to runs and fetch the value of the player.
        {
            topRuns = &p[i];                        // point my finger at him
        }
        if (p[i].wickets > topWickets->wickets)     // more wickets than the best so far
        {
            topWickets = &p[i];                     // point my finger at him
        }
    }
    printf("\nTop run-scorer  : %s (%d runs)\n", topRuns->name, topRuns->runs);
    printf("Top wicket-taker: %s (%d wickets)\n", topWickets->name, topWickets->wickets);
}

// loadFile: reads the notebook when the program starts
void loadFile(struct Player p[], int *count)
{
    FILE *fp = fopen(FILE_NAME, "r");   // open the notebook for reading
    struct Player n;                    // an empty player card to fill

    if (fp == NULL)                     // notebook could not be opened
    {
        perror("Cannot open file");     // say the reason
        return;
    }
    while (*count < MAX_PLAYERS &&      // while there is space AND
           fscanf(fp, "%d %29s %d %d %d %d", &n.id, n.name, &n.matches,
                  &n.runs, &n.wickets, &n.role) == 6)   // one full line was read
    {
        if (n.matches > 0)              // matches more than 0
        {
            n.average = (float)n.runs / n.matches;
        }
        else                            // do not divide by 0
        {
            n.average = 0;
        }
        p[*count] = n;                  // put the card in the next seat
        (*count)++;                     // add 1 to the counter
    }
    fclose(fp);                         // close the notebook
    printf("%d record(s) loaded.\n", *count);
}

// saveFile: writes all players in the notebook
void saveFile(struct Player p[], int count)
{
    FILE *fp = fopen(FILE_NAME, "w");   // open the notebook for writing
    int i;                              // counting box

    if (fp == NULL)                     // notebook could not be opened
    {
        perror("Cannot save file");
        return;
    }
    for (i = 0; i < count; i++)         // go through every player
    {
        fprintf(fp, "%d %s %d %d %d %d\n", p[i].id, p[i].name,
                p[i].matches, p[i].runs, p[i].wickets, p[i].role);   // write one line
    }
    fclose(fp);                         // close the notebook
    printf("%d record(s) saved.\n", count);
}