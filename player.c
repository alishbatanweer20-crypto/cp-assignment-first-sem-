/* player.c - all functions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

/* Ask until the user enters a whole number from min to max */
int getInt(char prompt[], int min, int max)
{
    int value, result, ch;

    while (1)
    {
        printf("%s", prompt);
        result = scanf("%d", &value);
        if (result == EOF)
        {
            exit(1);                       /* input closed */
        }
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            /* throw away the rest of the line */
        }
        if (result != 1)
        {
            printf("Please enter a number.\n");
        }
        else if (value < min || value > max)
        {
            printf("Number must be from %d to %d.\n", min, max);
        }
        else
        {
            return value;
        }
    }
}

/* 3 attempts to enter the right username and password */
int login(void)
{
    char user[30], pass[30];
    int i;

    for (i = 1; i <= 3; i++)
    {
        printf("Username: ");
        scanf("%29s", user);
        printf("Password: ");
        scanf("%29s", pass);
        if (strcmp(user, "admin") == 0 && strcmp(pass, "cricket123") == 0)
        {
            return 1;
        }
        printf("Wrong login.\n");
    }
    return 0;
}

/* Position of player with this id, or -1 if not found */
int findById(struct Player p[], int count, int id)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (p[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

/* Swap two players using pointers */
void swap(struct Player *a, struct Player *b)
{
    struct Player temp = *a;
    *a = *b;
    *b = temp;
}

/* Print one player as a table row */
void printRow(struct Player *p)
{
    printf("%-6d %-15s %-8d %-8d %-8d %-8.2f %d\n", p->id, p->name,
           p->matches, p->runs, p->wickets, p->average, p->role);
}

void printHeader(void)
{
    printf("\nID     Name            Matches  Runs     Wickets  Average  Role(1=Bat 2=Bowl 3=All)\n");
}

void addPlayer(struct Player p[], int *count)
{
    struct Player n;

    if (*count == MAX_PLAYERS)
    {
        printf("List is full.\n");
        return;
    }
    n.id = getInt("ID: ", 1, 999999);
    if (findById(p, *count, n.id) != -1)
    {
        printf("This ID already exists.\n");
        return;
    }
    printf("Name (one word): ");
    scanf("%29s", n.name);
    n.role    = getInt("Role (1=Batsman 2=Bowler 3=All-rounder): ", 1, 3);
    n.matches = getInt("Matches: ", 1, 1000);
    n.runs    = getInt("Runs: ", 0, 100000);
    n.wickets = getInt("Wickets: ", 0, 5000);
    n.average = (float)n.runs / n.matches;

    p[*count] = n;
    (*count)++;
    printf("Player added.\n");
}

void displayAll(struct Player p[], int count)
{
    int i;
    if (count == 0)
    {
        printf("No records.\n");
        return;
    }
    printHeader();
    for (i = 0; i < count; i++)
    {
        printRow(&p[i]);
    }
}

void searchPlayer(struct Player p[], int count)
{
    int i, found = 0, choice, id;
    char name[30];

    choice = getInt("Search by 1=ID, 2=Name: ", 1, 2);
    if (choice == 1)
    {
        id = getInt("ID: ", 1, 999999);
        i = findById(p, count, id);
        if (i != -1)
        {
            printHeader();
            printRow(&p[i]);
            found = 1;
        }
    }
    else
    {
        printf("Name: ");
        scanf("%29s", name);
        for (i = 0; i < count; i++)
        {
            if (strcmp(p[i].name, name) == 0)
            {
                printHeader();
                printRow(&p[i]);
                found = 1;
            }
        }
    }
    if (found == 0)
    {
        printf("Not found.\n");
    }
}

void updatePlayer(struct Player p[], int count)
{
    int i = findById(p, count, getInt("ID to update: ", 1, 999999));

    if (i == -1)
    {
        printf("Not found.\n");
        return;
    }
    p[i].matches = getInt("New matches: ", 1, 1000);
    p[i].runs    = getInt("New runs: ", 0, 100000);
    p[i].wickets = getInt("New wickets: ", 0, 5000);
    p[i].average = (float)p[i].runs / p[i].matches;
    printf("Updated.\n");
}

void deletePlayer(struct Player p[], int *count)
{
    int i = findById(p, *count, getInt("ID to delete: ", 1, 999999));

    if (i == -1)
    {
        printf("Not found.\n");
        return;
    }
    for (; i < *count - 1; i++)        /* move later records up one place */
    {
        p[i] = p[i + 1];
    }
    (*count)--;
    printf("Deleted.\n");
}

void sortAndReport(struct Player p[], int count)
{
    int i, j, choice;
    struct Player *topRuns = &p[0];
    struct Player *topWickets = &p[0];

    if (count == 0)
    {
        printf("No records.\n");
        return;
    }
    choice = getInt("Sort by 1=Runs, 2=Batting average: ", 1, 2);

    /* bubble sort, biggest first */
    for (i = 0; i < count - 1; i++)
    {
        for (j = 0; j < count - 1 - i; j++)
        {
            if (choice == 1 && p[j].runs < p[j + 1].runs)
            {
                swap(&p[j], &p[j + 1]);
            }
            if (choice == 2 && p[j].average < p[j + 1].average)
            {
                swap(&p[j], &p[j + 1]);
            }
        }
    }
    displayAll(p, count);

    /* find the best players */
    for (i = 1; i < count; i++)
    {
        if (p[i].runs > topRuns->runs)
        {
            topRuns = &p[i];
        }
        if (p[i].wickets > topWickets->wickets)
        {
            topWickets = &p[i];
        }
    }
    printf("\nTop run-scorer  : %s (%d runs)\n", topRuns->name, topRuns->runs);
    printf("Top wicket-taker: %s (%d wickets)\n", topWickets->name, topWickets->wickets);
}

void loadFile(struct Player p[], int *count)
{
    FILE *fp = fopen(FILE_NAME, "r");
    struct Player n;

    if (fp == NULL)
    {
        perror("Cannot open file");
        return;
    }
    while (*count < MAX_PLAYERS &&
           fscanf(fp, "%d %29s %d %d %d %d", &n.id, n.name, &n.matches,
                  &n.runs, &n.wickets, &n.role) == 6)
    {
        if (n.matches > 0)
        {
            n.average = (float)n.runs / n.matches;
        }
        else
        {
            n.average = 0;
        }
        p[*count] = n;
        (*count)++;
    }
    fclose(fp);
    printf("%d record(s) loaded.\n", *count);
}

void saveFile(struct Player p[], int count)
{
    FILE *fp = fopen(FILE_NAME, "w");
    int i;

    if (fp == NULL)
    {
        perror("Cannot save file");
        return;
    }
    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %d %d %d %d\n", p[i].id, p[i].name,
                p[i].matches, p[i].runs, p[i].wickets, p[i].role);
    }
    fclose(fp);
    printf("%d record(s) saved.\n", count);
}