/*
 * player.c
 * ---------------------------------------------------------------------------
 * Implementation of every operation of the Cricket Player Stats Management
 * System: input validation, CRUD operations, sorting/analytics, login and
 * file persistence.
 *
 * Design notes
 *   - All keyboard input is read with fgets() and converted with strtol().
 *     This avoids the classic scanf() problems (letters in numeric fields,
 *     leftover newlines, buffer overflows) so the program never crashes or
 *     loops forever on bad input.
 *   - Batting average is calculated as runs / matches (simplified model,
 *     because dismissals are not stored).  It is recomputed whenever the
 *     underlying stats change, never typed in by the user.
 * ---------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "player.h"

/* ========================================================================= */
/*                       PRIVATE (static) HELPER FUNCTIONS                    */
/* ========================================================================= */

/*
 * readLine - reads one line from stdin into buf (size bytes) and strips '\n'.
 * Returns 1 on success, 0 if the line was longer than the buffer
 * (the excess is discarded so it cannot pollute the next prompt).
 * If stdin is closed (EOF) the program terminates cleanly, which prevents an
 * infinite re-prompt loop.
 */
static int readLine(char *buf, size_t size)
{
    if (fgets(buf, (int)size, stdin) == NULL)
    {
        printf("\nInput stream closed. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
    {
        buf[len - 1] = '\0';          /* remove trailing newline */
        return 1;
    }

    /* No newline found: the line was too long -> flush the rest of it */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        ;                              /* discard characters */
    }
    return 0;
}

/* trimWhitespace - removes leading and trailing spaces/tabs in place */
static void trimWhitespace(char *s)
{
    char *start = s;
    while (*start != '\0' && isspace((unsigned char)*start))
    {
        start++;                       /* skip leading whitespace */
    }

    /* Shift the text to the front of the buffer */
    if (start != s)
    {
        memmove(s, start, strlen(start) + 1);
    }

    /* Walk backwards from the end, overwriting trailing whitespace */
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
    {
        s[--len] = '\0';
    }
}

/* findIndexById - linear search; returns array index or -1 if not found */
static int findIndexById(const Player *players, int count, int id)
{
    for (int i = 0; i < count; i++)
    {
        if (players[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

/*
 * containsIgnoreCase - returns 1 if 'needle' occurs anywhere inside 'hay',
 * ignoring upper/lower case.  Used for partial, case-insensitive name search.
 */
static int containsIgnoreCase(const char *hay, const char *needle)
{
    size_t nlen = strlen(needle);
    if (nlen == 0)
    {
        return 0;
    }

    for (; *hay != '\0'; hay++)
    {
        size_t k = 0;
        /* Compare character by character from the current position */
        while (k < nlen && hay[k] != '\0' &&
               tolower((unsigned char)hay[k]) == tolower((unsigned char)needle[k]))
        {
            k++;
        }
        if (k == nlen)
        {
            return 1;                  /* whole needle matched */
        }
    }
    return 0;
}

/*
 * validateStats - cross-field consistency check.
 * A player cannot score runs or take wickets without playing a match.
 * Returns 1 if valid, 0 otherwise (and prints the reason).
 */
static int validateStats(int matches, int runs, int wickets)
{
    if (matches == 0 && (runs > 0 || wickets > 0))
    {
        printf("  Invalid: runs/wickets cannot be greater than 0 when matches played is 0.\n");
        return 0;
    }
    return 1;
}

/* printTableHeader - prints the column titles and a separator line */
static void printTableHeader(void)
{
    printf("\n%-6s %-*s %-12s %8s %8s %8s %10s\n",
           "ID", NAME_LEN - 1, "Name", "Role", "Matches", "Runs", "Wickets", "Bat Avg");
    for (int i = 0; i < 6 + 1 + (NAME_LEN - 1) + 1 + 12 + 1 + 8 + 1 + 8 + 1 + 8 + 1 + 10; i++)
    {
        putchar('-');
    }
    putchar('\n');
}

/* printPlayerRow - prints one record using the same widths as the header */
static void printPlayerRow(const Player *p)
{
    printf("%-6d %-*s %-12s %8d %8d %8d %10.2f\n",
           p->id, NAME_LEN - 1, p->name, roleToString(p->role),
           p->matches, p->runs, p->wickets, p->battingAverage);
}

/*
 * isOutOfOrder - returns 1 when 'a' must be placed AFTER 'b' in a descending
 * ordering by the chosen criterion (i.e. a is strictly smaller than b).
 * Using a strict comparison keeps the sort stable for equal values.
 */
static int isOutOfOrder(const Player *a, const Player *b, SortCriterion criterion)
{
    if (criterion == SORT_BY_RUNS)
    {
        return a->runs < b->runs;
    }
    return a->battingAverage < b->battingAverage;
}

/* ========================================================================= */
/*                              PUBLIC FUNCTIONS                              */
/* ========================================================================= */

/* ------------------------------ Input helpers ----------------------------- */

/*
 * readInt - repeatedly prompts until the user enters a whole number between
 * min and max (inclusive).  Rejects letters, decimals, empty input,
 * overflow and out-of-range values.
 */
int readInt(const char *prompt, int min, int max)
{
    char buf[INPUT_BUF_LEN];
    char *endptr;

    while (1)
    {
        printf("%s", prompt);

        if (!readLine(buf, sizeof buf))
        {
            printf("  Input too long. Please try again.\n");
            continue;
        }

        errno = 0;
        long value = strtol(buf, &endptr, 10);

        if (endptr == buf)                         /* no digits at all */
        {
            printf("  Invalid input. Please enter a whole number.\n");
            continue;
        }

        while (isspace((unsigned char)*endptr))    /* allow trailing spaces */
        {
            endptr++;
        }

        if (*endptr != '\0')                       /* junk after the number */
        {
            printf("  Invalid input. Please enter digits only.\n");
            continue;
        }

        if (errno == ERANGE || value < min || value > max)
        {
            printf("  Value out of range. Enter a number from %d to %d.\n", min, max);
            continue;
        }

        return (int)value;
    }
}

/*
 * readName - prompts until a valid name is entered: non-empty after trimming,
 * shorter than NAME_LEN and free of commas (the comma is the file delimiter).
 */
void readName(const char *prompt, char *dest)
{
    char buf[INPUT_BUF_LEN];

    while (1)
    {
        printf("%s", prompt);

        if (!readLine(buf, sizeof buf))
        {
            printf("  Input too long. Please try again.\n");
            continue;
        }

        trimWhitespace(buf);

        if (buf[0] == '\0')
        {
            printf("  Name cannot be empty.\n");
        }
        else if (strlen(buf) >= NAME_LEN)
        {
            printf("  Name too long (maximum %d characters).\n", NAME_LEN - 1);
        }
        else if (strchr(buf, ',') != NULL)
        {
            printf("  Name must not contain commas.\n");
        }
        else
        {
            strcpy(dest, buf);                      /* safe: length checked above */
            return;
        }
    }
}

/* readYesNo - returns 1 for 'y'/'Y', 0 for 'n'/'N'; re-prompts otherwise */
int readYesNo(const char *prompt)
{
    char buf[INPUT_BUF_LEN];

    while (1)
    {
        printf("%s", prompt);
        if (readLine(buf, sizeof buf))
        {
            trimWhitespace(buf);
            if (buf[1] == '\0' && (buf[0] == 'y' || buf[0] == 'Y'))
            {
                return 1;
            }
            if (buf[1] == '\0' && (buf[0] == 'n' || buf[0] == 'N'))
            {
                return 0;
            }
        }
        printf("  Please answer with y or n.\n");
    }
}

/* ------------------------------- Utilities -------------------------------- */

/* calculateBattingAverage - runs per match; 0 when no matches were played */
float calculateBattingAverage(const Player *p)
{
    if (p->matches <= 0)
    {
        return 0.0f;                   /* avoids division by zero */
    }
    return (float)p->runs / (float)p->matches;
}

/* roleToString - converts the numeric role code into readable text */
const char *roleToString(int role)
{
    switch (role)
    {
        case ROLE_BATSMAN:     return "Batsman";
        case ROLE_BOWLER:      return "Bowler";
        case ROLE_ALLROUNDER:  return "All-rounder";
        default:               return "Unknown";
    }
}

/* ------------------------------- Security --------------------------------- */

/*
 * login - asks for username and password and compares them with strcmp().
 * The user has MAX_LOGIN_ATTEMPTS tries.  Returns 1 on success, 0 on failure.
 */
int login(void)
{
    char user[INPUT_BUF_LEN];
    char pass[INPUT_BUF_LEN];

    printf("=====================================\n");
    printf("   CRICKET PLAYER STATS SYSTEM LOGIN\n");
    printf("=====================================\n");

    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++)
    {
        printf("Username: ");
        if (!readLine(user, sizeof user))
        {
            user[0] = '\0';            /* over-long input counts as wrong */
        }
        printf("Password: ");
        if (!readLine(pass, sizeof pass))
        {
            pass[0] = '\0';
        }

        /* strcmp returns 0 when both strings are identical */
        if (strcmp(user, LOGIN_USER) == 0 && strcmp(pass, LOGIN_PASS) == 0)
        {
            printf("\nLogin successful. Welcome, %s!\n", user);
            return 1;
        }

        printf("Incorrect credentials. Attempts remaining: %d\n\n",
               MAX_LOGIN_ATTEMPTS - attempt);
    }
    return 0;
}

/* ---------------------------- 1. Add a player ----------------------------- */

void addPlayer(Player *players, int *count)
{
    if (*count >= MAX_PLAYERS)
    {
        printf("\nDatabase is full (%d players). Delete a record first.\n", MAX_PLAYERS);
        return;
    }

    Player p;                          /* build the record in a temporary */
    memset(&p, 0, sizeof p);

    printf("\n--- Add New Player ---\n");

    /* Player ID must be positive and unique */
    while (1)
    {
        p.id = readInt("Player ID (positive number): ", 1, MAX_PLAYER_ID);
        if (findIndexById(players, *count, p.id) == -1)
        {
            break;
        }
        printf("  ID %d already exists. Choose a different ID.\n", p.id);
    }

    readName("Player name: ", p.name);
    p.role = readInt("Role (1=Batsman, 2=Bowler, 3=All-rounder): ", 1, 3);

    /* Statistics are re-requested until they are mutually consistent */
    while (1)
    {
        p.matches = readInt("Matches played: ", 0, MAX_MATCHES);
        p.runs    = readInt("Runs scored: ",    0, MAX_RUNS);
        p.wickets = readInt("Wickets taken: ",  0, MAX_WICKETS);
        if (validateStats(p.matches, p.runs, p.wickets))
        {
            break;
        }
    }

    p.battingAverage = calculateBattingAverage(&p);

    players[*count] = p;               /* copy the finished record into the array */
    (*count)++;                        /* update the active record count */

    printf("\nPlayer '%s' added successfully.\n", p.name);
}

/* ------------------------- 2. Display all records ------------------------- */

void displayAllPlayers(const Player *players, int count)
{
    if (count == 0)
    {
        printf("\nNo player records available.\n");
        return;
    }

    printf("\n==================== ALL PLAYER RECORDS ====================");
    printTableHeader();

    /* Pointer walk: 'p' visits every element of the array */
    for (const Player *p = players; p < players + count; p++)
    {
        printPlayerRow(p);
    }
    printf("\nTotal players: %d\n", count);
}

/* --------------------------- 3. Search a record --------------------------- */

void searchPlayer(const Player *players, int count)
{
    if (count == 0)
    {
        printf("\nNo records to search.\n");
        return;
    }

    printf("\n--- Search Player ---\n");
    printf("1. Search by Player ID\n");
    printf("2. Search by Player Name\n");
    int mode = readInt("Choose search type (1-2): ", 1, 2);

    if (mode == 1)
    {
        int id  = readInt("Enter Player ID: ", 1, MAX_PLAYER_ID);
        int idx = findIndexById(players, count, id);

        if (idx == -1)
        {
            printf("\nNo player found with ID %d.\n", id);
            return;
        }
        printTableHeader();
        printPlayerRow(&players[idx]);
    }
    else
    {
        char query[NAME_LEN];
        int  found = 0;

        readName("Enter name (or part of it): ", query);

        for (int i = 0; i < count; i++)
        {
            if (containsIgnoreCase(players[i].name, query))
            {
                if (!found)
                {
                    printTableHeader();   /* print header only before first hit */
                }
                printPlayerRow(&players[i]);
                found++;
            }
        }

        if (found == 0)
        {
            printf("\nNo player found matching '%s'.\n", query);
        }
        else
        {
            printf("\n%d match(es) found.\n", found);
        }
    }
}

/* -------------------------- 4. Update a record ---------------------------- */

void updatePlayer(Player *players, int count)
{
    if (count == 0)
    {
        printf("\nNo records to update.\n");
        return;
    }

    printf("\n--- Update Player Stats ---\n");
    int id  = readInt("Enter Player ID to update: ", 1, MAX_PLAYER_ID);
    int idx = findIndexById(players, count, id);

    if (idx == -1)
    {
        printf("\nNo player found with ID %d.\n", id);
        return;
    }

    Player *p = &players[idx];         /* pointer to the record being edited */
    printf("\nCurrent record:");
    printTableHeader();
    printPlayerRow(p);

    printf("\n1. Matches played\n2. Runs scored\n3. Wickets taken\n");
    printf("4. All three stats\n0. Cancel\n");
    int field = readInt("What do you want to update? (0-4): ", 0, 4);

    if (field == 0)
    {
        printf("Update cancelled.\n");
        return;
    }

    /* Work on copies so an invalid combination never corrupts the record */
    int newMatches = p->matches;
    int newRuns    = p->runs;
    int newWickets = p->wickets;

    if (field == 1 || field == 4)
    {
        newMatches = readInt("New matches played: ", 0, MAX_MATCHES);
    }
    if (field == 2 || field == 4)
    {
        newRuns = readInt("New runs scored: ", 0, MAX_RUNS);
    }
    if (field == 3 || field == 4)
    {
        newWickets = readInt("New wickets taken: ", 0, MAX_WICKETS);
    }

    if (!validateStats(newMatches, newRuns, newWickets))
    {
        printf("Update rejected. Record left unchanged.\n");
        return;
    }

    /* Commit the changes and recompute the dynamic batting average */
    p->matches = newMatches;
    p->runs    = newRuns;
    p->wickets = newWickets;
    p->battingAverage = calculateBattingAverage(p);

    printf("\nRecord updated successfully:");
    printTableHeader();
    printPlayerRow(p);
}

/* -------------------------- 5. Delete a record ---------------------------- */

void deletePlayer(Player *players, int *count)
{
    if (*count == 0)
    {
        printf("\nNo records to delete.\n");
        return;
    }

    printf("\n--- Delete Player ---\n");
    int id  = readInt("Enter Player ID to delete: ", 1, MAX_PLAYER_ID);
    int idx = findIndexById(players, *count, id);

    if (idx == -1)
    {
        printf("\nNo player found with ID %d.\n", id);
        return;
    }

    printf("About to delete: %s (ID %d)\n", players[idx].name, players[idx].id);
    if (!readYesNo("Are you sure? (y/n): "))
    {
        printf("Deletion cancelled.\n");
        return;
    }

    /* Close the gap: shift every later record one position to the left */
    for (int i = idx; i < *count - 1; i++)
    {
        players[i] = players[i + 1];
    }
    (*count)--;                        /* one fewer active record */

    printf("Player deleted successfully. %d record(s) remain.\n", *count);
}

/* ----------------------- 6. Sorting and analytics ------------------------- */

void sortAndAnalytics(const Player *players, int count)
{
    if (count == 0)
    {
        printf("\nNo records available for analysis.\n");
        return;
    }

    printf("\n--- Sorting & Analytics ---\n");
    printf("1. Sort by Runs Scored (descending)\n");
    printf("2. Sort by Batting Average (descending)\n");
    SortCriterion criterion = (SortCriterion)readInt("Choose sort criterion (1-2): ", 1, 2);

    /*
     * Instead of moving whole Player structs (large), we sort an array of
     * POINTERS to the records.  The original array order stays untouched and
     * each swap only exchanges two addresses.
     */
    const Player *sorted[MAX_PLAYERS];
    for (int i = 0; i < count; i++)
    {
        sorted[i] = &players[i];       /* store address of each record */
    }

    /* Bubble sort on the pointer array; early exit when no swap happens */
    for (int pass = 0; pass < count - 1; pass++)
    {
        int swapped = 0;
        for (int j = 0; j < count - 1 - pass; j++)
        {
            if (isOutOfOrder(sorted[j], sorted[j + 1], criterion))
            {
                const Player *tmp = sorted[j];     /* swap the two addresses */
                sorted[j]     = sorted[j + 1];
                sorted[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped)
        {
            break;                     /* already sorted */
        }
    }

    printf("\n======== PLAYERS SORTED BY %s (DESCENDING) ========",
           criterion == SORT_BY_RUNS ? "RUNS SCORED" : "BATTING AVERAGE");
    printTableHeader();
    for (int i = 0; i < count; i++)
    {
        printPlayerRow(sorted[i]);
    }

    /* ---- Summary report: find top run-scorer and top wicket-taker ---- */
    const Player *topRuns    = &players[0];
    const Player *topWickets = &players[0];
    long totalRuns = 0;
    long totalWickets = 0;

    for (const Player *p = players; p < players + count; p++)
    {
        if (p->runs > topRuns->runs)
        {
            topRuns = p;               /* first player wins ties */
        }
        if (p->wickets > topWickets->wickets)
        {
            topWickets = p;
        }
        totalRuns    += p->runs;
        totalWickets += p->wickets;
    }

    printf("\n=============== SUMMARY REPORT ===============\n");
    printf("Total players        : %d\n", count);
    printf("Total runs (all)     : %ld\n", totalRuns);
    printf("Total wickets (all)  : %ld\n", totalWickets);
    printf("Top Run-Scorer       : %s (ID %d) - %d runs, average %.2f\n",
           topRuns->name, topRuns->id, topRuns->runs, topRuns->battingAverage);
    if (topWickets->wickets > 0)
    {
        printf("Top Wicket-Taker     : %s (ID %d) - %d wickets\n",
               topWickets->name, topWickets->id, topWickets->wickets);
    }
    else
    {
        printf("Top Wicket-Taker     : none (no wickets recorded yet)\n");
    }
    printf("==============================================\n");
}

/* ------------------------- File handling (persistence) -------------------- */

/*
 * loadPlayers - reads DATA_FILE into the array.
 * File format (one record per line):  id,name,matches,runs,wickets,role
 * Returns 1 if the file was read, 0 if it could not be opened
 * (e.g. first run), in which case the system starts with an empty database.
 * Corrupt or duplicate lines are skipped with a warning.
 */
int loadPlayers(Player *players, int *count)
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL)                    /* NULL check required for graceful handling */
    {
        perror("Note: could not open " DATA_FILE);
        printf("Starting with an empty database.\n");
        return 0;
    }

    Player p;
    int fields;
    int skipped = 0;

    *count = 0;

    /*
     * " %d" - leading space skips the newline from the previous line.
     * "%29[^,]" - reads the name up to the next comma (max NAME_LEN-1 chars).
     */
    while (*count < MAX_PLAYERS &&
           (fields = fscanf(fp, " %d,%29[^,],%d,%d,%d,%d",
                            &p.id, p.name, &p.matches, &p.runs,
                            &p.wickets, &p.role)) != EOF)
    {
        if (fields != 6)
        {
            /* Malformed line: discard the rest of it and carry on */
            int ch;
            while ((ch = fgetc(fp)) != '\n' && ch != EOF)
            {
                ;
            }
            skipped++;
            continue;
        }

        /* Reject records that break the validation rules or repeat an ID */
        if (p.id < 1 || p.id > MAX_PLAYER_ID ||
            p.matches < 0 || p.matches > MAX_MATCHES ||
            p.runs < 0 || p.runs > MAX_RUNS ||
            p.wickets < 0 || p.wickets > MAX_WICKETS ||
            p.role < 1 || p.role > 3 ||
            (p.matches == 0 && (p.runs > 0 || p.wickets > 0)) ||
            findIndexById(players, *count, p.id) != -1)
        {
            skipped++;
            continue;
        }

        p.battingAverage = calculateBattingAverage(&p);   /* recompute, not stored */
        players[*count] = p;
        (*count)++;
    }

    fclose(fp);

    printf("Loaded %d record(s) from %s.\n", *count, DATA_FILE);
    if (skipped > 0)
    {
        printf("Warning: %d invalid line(s) were skipped.\n", skipped);
    }
    return 1;
}

/*
 * savePlayers - writes every record to DATA_FILE with fprintf().
 * Returns 1 on success, 0 on any failure.
 */
int savePlayers(const Player *players, int count)
{
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL)
    {
        perror("Error: could not open " DATA_FILE " for writing");
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        /* The batting average is derived data, so it is not written */
        if (fprintf(fp, "%d,%s,%d,%d,%d,%d\n",
                    players[i].id, players[i].name, players[i].matches,
                    players[i].runs, players[i].wickets, players[i].role) < 0)
        {
            perror("Error: write failure");
            fclose(fp);
            return 0;
        }
    }

    if (fclose(fp) != 0)               /* fclose can fail when flushing */
    {
        perror("Error: could not close " DATA_FILE);
        return 0;
    }

    printf("Saved %d record(s) to %s.\n", count, DATA_FILE);
    return 1;
}
