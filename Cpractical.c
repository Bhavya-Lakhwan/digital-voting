/*
 * ============================================================
 *  DIGITAL VOTING SYSTEM
 *  Level   : Practical
 *  Modules : Arrays, Structures, Conditions
 *
 ==========
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Flush leftover characters (especially '\n') from stdin */
void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ── Constants ─────────────────────────────────────────────── */
#define MAX_CANDIDATES 10
#define MAX_VOTERS     100
#define ADMIN_PIN      1234     /* change this to something secure */
#define VOTES_FILE     "votes.dat"
#define VOTERS_FILE    "voted.dat"

/* ── Structures ─────────────────────────────────────────────── */
typedef struct {
    int  id;
    char name[50];
    char party[50];
    int  votes;
} Candidate;

typedef struct {
    int voter_id;         /* unique voter ID to prevent double voting */
    char voter_name[50];
    int  voted_for;       /* candidate id */
} VoterRecord;

/* ── Globals ────────────────────────────────────────────────── */
Candidate   candidates[MAX_CANDIDATES];
VoterRecord voter_log[MAX_VOTERS];
int ccount = 0;
int vcount = 0;
int voting_open = 0;    /* 0 = closed, 1 = open */

/* ── File I/O ───────────────────────────────────────────────── */
void save_data() {
    FILE *fp;
    fp = fopen(VOTES_FILE, "wb");
    fwrite(&ccount,     sizeof(int),       1,      fp);
    fwrite(&voting_open,sizeof(int),       1,      fp);
    fwrite(candidates,  sizeof(Candidate), ccount, fp);
    fclose(fp);

    fp = fopen(VOTERS_FILE, "wb");
    fwrite(&vcount,     sizeof(int),         1,      fp);
    fwrite(voter_log,   sizeof(VoterRecord), vcount, fp);
    fclose(fp);
    printf("Data saved.\n");
}

void load_data() {
    FILE *fp;
    fp = fopen(VOTES_FILE, "rb");
    if (fp) {
        fread(&ccount,      sizeof(int),       1,      fp);
        fread(&voting_open, sizeof(int),       1,      fp);
        fread(candidates,   sizeof(Candidate), ccount, fp);
        fclose(fp);
    }
    fp = fopen(VOTERS_FILE, "rb");
    if (fp) {
        fread(&vcount,    sizeof(int),         1,      fp);
        fread(voter_log,  sizeof(VoterRecord), vcount, fp);
        fclose(fp);
    }
    printf("Loaded %d candidate(s), %d voter record(s).\n", ccount, vcount);
    printf("Voting is currently: %s\n", voting_open ? "OPEN" : "CLOSED");
}

/* ── Helpers ────────────────────────────────────────────────── */
/* Returns 1 if voter_id already voted, 0 otherwise */
int has_voted(int voter_id) {
    for (int i = 0; i < vcount; i++)
        if (voter_log[i].voter_id == voter_id) return 1;
    return 0;
}

/* Find candidate index by id, -1 if not found */
int find_candidate(int id) {
    for (int i = 0; i < ccount; i++)
        if (candidates[i].id == id) return i;
    return -1;
}

/* ════════════════════════════════════════════════════════════
 *  ADMIN FUNCTIONS
 * ════════════════════════════════════════════════════════════ */
void add_candidate() {
    if (ccount >= MAX_CANDIDATES) { printf("Candidate list full!\n"); return; }
    Candidate c;
    printf("\n--- Add Candidate ---\n");
    printf("Candidate ID   : "); scanf("%d", &c.id); flush_input();
    printf("Name           : "); fgets(c.name,  sizeof(c.name),  stdin); c.name[strcspn(c.name,"\n")]  = 0;
    printf("Party          : "); fgets(c.party, sizeof(c.party), stdin); c.party[strcspn(c.party,"\n")]= 0;
    c.votes = 0;
    candidates[ccount++] = c;
    printf("Candidate added: %s (%s)\n", c.name, c.party);
}

void display_candidates() {
    if (ccount == 0) { printf("No candidates registered.\n"); return; }
    printf("\n%-5s %-25s %-25s\n", "ID", "Name", "Party");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < ccount; i++)
        printf("%-5d %-25s %-25s\n",
               candidates[i].id, candidates[i].name, candidates[i].party);
}

void open_voting()  { voting_open = 1; printf("Voting is now OPEN.\n"); }
void close_voting() { voting_open = 0; printf("Voting is now CLOSED.\n"); }

void declare_results() {
    if (ccount == 0) { printf("No candidates found.\n"); return; }
    printf("\n============= ELECTION RESULTS =============\n");
    printf("%-5s %-25s %-25s %-6s\n", "ID","Name","Party","Votes");
    printf("------------------------------------------------------------\n");

    int total_votes = 0, winner_idx = 0;
    for (int i = 0; i < ccount; i++) {
        printf("%-5d %-25s %-25s %-6d\n",
               candidates[i].id, candidates[i].name,
               candidates[i].party, candidates[i].votes);
        total_votes += candidates[i].votes;
        if (candidates[i].votes > candidates[winner_idx].votes)
            winner_idx = i;
    }
    printf("------------------------------------------------------------\n");
    printf("Total Votes Cast : %d\n", total_votes);

    if (total_votes == 0) {
        printf("No votes have been cast yet.\n");
        return;
    }
    printf("\n🏆 WINNER: %s (%s) with %d votes (%.1f%%)\n",
           candidates[winner_idx].name,
           candidates[winner_idx].party,
           candidates[winner_idx].votes,
           (candidates[winner_idx].votes * 100.0f) / total_votes);
}

void reset_election() {
    char confirm[5];
    printf("Are you sure you want to RESET all votes? (yes/no): ");
    flush_input();
    fgets(confirm, sizeof(confirm), stdin);
    confirm[strcspn(confirm, "\n")] = 0;
    if (strcmp(confirm, "yes") == 0) {
        for (int i = 0; i < ccount; i++) candidates[i].votes = 0;
        vcount = 0;
        voting_open = 0;
        printf("Election reset. All votes cleared.\n");
    } else {
        printf("Reset cancelled.\n");
    }
}

void admin_menu() {
    int pin;
    printf("Enter Admin PIN: "); scanf("%d", &pin);
    if (pin != ADMIN_PIN) { printf("Wrong PIN! Access denied.\n"); return; }

    int ch;
    do {
        printf("\n--- Admin Panel ---\n");
        printf("1. Add Candidate\n");
        printf("2. Display Candidates\n");
        printf("3. Open Voting\n");
        printf("4. Close Voting\n");
        printf("5. Declare Results\n");
        printf("6. Reset Election\n");
        printf("7. Back\n");
        printf("Choice: ");
        if (scanf("%d", &ch) != 1) { ch = 0; }
        flush_input();
        switch (ch) {
            case 1: add_candidate();     break;
            case 2: display_candidates();break;
            case 3: open_voting();       break;
            case 4: close_voting();      break;
            case 5: declare_results();   break;
            case 6: reset_election();    break;
            case 7: break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 7);
}

/* ════════════════════════════════════════════════════════════
 *  VOTER FUNCTIONS
 * ════════════════════════════════════════════════════════════ */
void cast_vote() {
    if (!voting_open) { printf("Voting is currently CLOSED.\n"); return; }
    if (ccount == 0)  { printf("No candidates registered yet.\n"); return; }

    VoterRecord vr;
    printf("\n--- Cast Your Vote ---\n");
    printf("Enter your Voter ID : "); scanf("%d", &vr.voter_id); flush_input();

    if (has_voted(vr.voter_id)) {
        printf("You have already voted! Each voter can vote only once.\n");
        return;
    }

    printf("Enter your Name     : "); fgets(vr.voter_name, sizeof(vr.voter_name), stdin); vr.voter_name[strcspn(vr.voter_name,"\n")] = 0;
    display_candidates();
    printf("\nEnter Candidate ID to vote for: ");
    scanf("%d", &vr.voted_for);

    int idx = find_candidate(vr.voted_for);
    if (idx == -1) {
        printf("Invalid Candidate ID.\n");
        return;
    }
    candidates[idx].votes++;
    voter_log[vcount++] = vr;
    printf("✅ Vote cast successfully for %s (%s)!\n",
           candidates[idx].name, candidates[idx].party);
}

/* ── Main ───────────────────────────────────────────────────── */
int main() {
    load_data();
    int choice;
    do {
        printf("\n========= Digital Voting System =========\n");
        printf("1. Cast Vote\n");
        printf("2. View Candidates\n");
        printf("3. Admin Panel\n");
        printf("4. Save & Exit\n");
        printf("=========================================\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { choice = 0; }
        flush_input();
        switch (choice) {
            case 1: cast_vote();          break;
            case 2: display_candidates(); break;
            case 3: admin_menu();         break;
            case 4: save_data(); printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 4);
    return 0;
}