/* main.c - menu
   Compile: gcc main.c player.c -o cricket_system
   Login  : admin / cricket123 */
#include <stdio.h>
#include "player.h"

int main(void)
{
    struct Player players[MAX_PLAYERS];
    int count = 0;
    int choice;

    if (login() == 0)
    {
        printf("Access denied.\n");
        return 1;
    }
    loadFile(players, &count);

    do
    {
        printf("\n1. Add  2. Display  3. Search  4. Update\n");
        printf("5. Delete  6. Sort & Report  7. Save  0. Exit\n");
        choice = getInt("Choice: ", 0, 7);

        switch (choice)
        {
            case 1: addPlayer(players, &count);      break;
            case 2: displayAll(players, count);      break;
            case 3: searchPlayer(players, count);    break;
            case 4: updatePlayer(players, count);    break;
            case 5: deletePlayer(players, &count);   break;
            case 6: sortAndReport(players, count);   break;
            case 7: saveFile(players, count);        break;
            case 0: saveFile(players, count);        break;
        }
    } while (choice != 0);

    return 0;
}