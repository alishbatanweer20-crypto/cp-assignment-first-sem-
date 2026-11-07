#include <stdio.h>               // brings printf
#include "player.h"              // brings our player card design and function names

int main(void)                   // the program always starts here
{
    struct Player players[MAX_PLAYERS];   // a row of 100 empty player cards
    int count = 0;                        // how many players we have now: 0
    int choice;                           // a box for your menu pick

    if (login() == 0)                     // call login; if it gives back 0 (failed)
    {
        printf("Access denied.\n");       // say "no entry"
        return 1;                         // stop the program (1 means something went wrong)
    }
    loadFile(players, &count);            // read the old players from the notebook

    do                                    // do this at least one time
    {
        printf("\n1. Add  2. Display  3. Search  4. Update\n");           // show first half of the menu
        printf("5. Delete  6. Sort & Report  7. Save  0. Exit\n");        // show second half of the menu
        choice = getInt("Choice: ", 0, 7);        // ask for a number from 0 to 7

        switch (choice)                   // look at your choice and jump to the matching door
        {
            case 1: addPlayer(players, &count);      break;   // 1: add a player, then break = leave the switch
            case 2: displayAll(players, count);      break;   // 2: show all players
            case 3: searchPlayer(players, count);    break;   // 3: search
            case 4: updatePlayer(players, count);    break;   // 4: update
            case 5: deletePlayer(players, &count);   break;   // 5: delete
            case 6: sortAndReport(players, count);   break;   // 6: sort and best players
            case 7: saveFile(players, count);        break;   // 7: save to the notebook
            case 0: saveFile(players, count);        break;   // 0: save before leaving
        }
    } while (choice != 0);                // go back and show the menu again, unless you picked 0

    return 0;                             // 0 means "finished happily"
}