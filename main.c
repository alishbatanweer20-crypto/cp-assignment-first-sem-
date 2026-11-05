/*
 * main.c
 * ---------------------------------------------------------------------------
 * Cricket Player Stats Management System - program entry point.
 *
 * Build : gcc main.c player.c -o cricket_system
 * Run   : ./cricket_system        (login: admin / cricket123)
 *
 * Responsibilities of this file:
 *   1. authenticate the user,
 *   2. load saved records,
 *   3. run the menu loop (do-while + switch),
 *   4. save the records on exit.
 * ---------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include "player.h"

/* printMenu - displays the main menu */
static void printMenu(void)
{
    printf("\n=====================================\n");
    printf("   CRICKET PLAYER STATS MANAGEMENT\n");
    printf("=====================================\n");
    printf("  1. Add New Player Record\n");
    printf("  2. Display All Records\n");
    printf("  3. Search Record (ID or Name)\n");
    printf("  4. Update Record Details\n");
    printf("  5. Delete Record\n");
    printf("  6. Sort & Analytics Report\n");
    printf("  7. Save Records to File\n");
    printf("  0. Save and Exit\n");
    printf("-------------------------------------\n");
}

int main(void)
{
    Player players[MAX_PLAYERS];       /* array of structures holding all records */
    int    count  = 0;                 /* number of active records */
    int    choice;

    /* Security login: terminate if the user fails all attempts */
    if (!login())
    {
        printf("Too many failed attempts. Access denied.\n");
        return EXIT_FAILURE;
    }

    /* Load persisted data (handles a missing file gracefully) */
    loadPlayers(players, &count);

    /* Main menu loop: runs at least once, ends when the user picks 0 */
    do
    {
        printMenu();
        choice = readInt("Enter your choice (0-7): ", 0, 7);

        switch (choice)
        {
            case 1:  addPlayer(players, &count);          break;
            case 2:  displayAllPlayers(players, count);   break;
            case 3:  searchPlayer(players, count);        break;
            case 4:  updatePlayer(players, count);        break;
            case 5:  deletePlayer(players, &count);       break;
            case 6:  sortAndAnalytics(players, count);    break;
            case 7:  savePlayers(players, count);         break;
            case 0:
                printf("\nSaving data before exit...\n");
                if (!savePlayers(players, count))
                {
                    printf("WARNING: data could not be saved!\n");
                }
                printf("Goodbye!\n");
                break;
            default:
                /* Unreachable because readInt limits the range, kept for safety */
                printf("Invalid choice. Please select 0-7.\n");
                break;
        }
    } while (choice != 0);

    return EXIT_SUCCESS;
}
