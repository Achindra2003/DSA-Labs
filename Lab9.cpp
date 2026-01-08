/**
 * @file tsp_logistics.c
 * @brief Travelling Salesman Problem using Held-Karp (Dynamic Programming)
 * @domain Medical Representative Route Optimization
 * * SELF-LEARNING INITIATIVES:
 * 1. Bitmask Visualization: Prints binary representation of visited sets (e.g., {A, C} -> 0101).
 * 2. DP State Trace: Logs valid path costs as they are computed.
 * 3. Manual Verification Output: Prints the exact sequence for manual checking.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_CITIES 10
#define INF 999999

// --- Data Structures ---

typedef struct {
    int n;                          // Number of cities
    int dist[MAX_CITIES][MAX_CITIES]; // Distance Matrix
    char names[MAX_CITIES][20];     // City Names
} Graph;

// DP Memoization Table
// dimensions: [2^n][n]
// dp[mask][i] stores min cost to visit set 'mask', ending at city 'i'
int dp[1 << MAX_CITIES][MAX_CITIES];

// Parent table to reconstruct the path
// parent[mask][i] stores the city visited BEFORE 'i' in the set 'mask'
int parent[1 << MAX_CITIES][MAX_CITIES];

// --- Helper Functions ---

int min(int a, int b) { return (a < b) ? a : b; }

// Visualizes a bitmask as a binary string (SLI Component)
void printBinary(int num, int n) {
    for (int i = n - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

// --- Core Algorithm: Held-Karp (Top-Down Recursion) ---

int tsp(int mask, int pos, int n, Graph* g, int verbose) {
    // Base Case: If all cities visited (mask is all 1s)
    if (mask == (1 << n) - 1) {
        // Return cost to return to start (0) from current pos
        return g->dist[pos][0];
    }

    // Return memoized value if already computed
    if (dp[mask][pos] != -1) {
        return dp[mask][pos];
    }

    int ans = INF;
    int bestCity = -1;

    // Try to go to every other city 'nextCity'
    for (int nextCity = 0; nextCity < n; nextCity++) {
        // If 'nextCity' is NOT in the current 'mask' (not visited yet)
        if ((mask & (1 << nextCity)) == 0) {
            
            int newCost = g->dist[pos][nextCity] + tsp(mask | (1 << nextCity), nextCity, n, g, verbose);
            
            if (newCost < ans) {
                ans = newCost;
                bestCity = nextCity;
            }
        }
    }

    // SLI: Trace the decision
    if (verbose && ans < INF) {
        printf(" [DP Update] Set: ");
        printBinary(mask, n);
        printf(" | Curr: %s | MinCost to Finish: %d\n", g->names[pos], ans);
    }

    // Store state
    parent[mask][pos] = bestCity;
    return dp[mask][pos] = ans;
}

// --- Output Functions ---

void printPath(int n, Graph* g) {
    printf("\n--- Optimal Route Trace ---\n");
    int cur = 0; // Start at city 0
    int mask = 1; // Mask initially has only 0 visited (binary ...001)
    
    printf("Start at: %s\n", g->names[0]);
    
    // Reconstruct path using parent array
    for (int i = 0; i < n - 1; i++) {
        int next = parent[mask][cur];
        printf(" -> Goto: %-15s (Cumulative Cost: %d)\n", g->names[next], g->dist[cur][next]);
        cur = next;
        mask = mask | (1 << next);
    }
    printf(" -> Return: %-15s (Cumulative Cost: %d)\n", g->names[0], g->dist[cur][0]);
}

void solveTSP(Graph* g, int verbose) {
    int n = g->n;
    
    // Initialize DP tables with -1
    for (int i = 0; i < (1 << n); i++) {
        for (int j = 0; j < n; j++) {
            dp[i][j] = -1;
            parent[i][j] = -1;
        }
    }

    printf("\n[SYSTEM] Calculating optimal tour cost...\n");
    // Start TSP from city 0, with mask '1' (only 0 visited)
    int minCost = tsp(1, 0, n, g, verbose);

    printf("\n============================================\n");
    printf(" Minimum Tour Cost: %d\n", minCost);
    printf("============================================\n");
    
    printPath(n, g);
}

// --- Execution Modes ---

void runGuidedDemo() {
    printf("\n>>> Running Guided Demo (Medical Rep Loop) <<<\n");
    Graph g;
    g.n = 4;
    
    // 1. Setup Names
    sprintf(g.names[0], "HQ_Office");
    sprintf(g.names[1], "City_Clinic");
    sprintf(g.names[2], "Gov_Hospital");
    sprintf(g.names[3], "Suburb_Pharm");

    // 2. Setup Distance Matrix (Symmetric)
    // Row 0
    g.dist[0][0]=0; g.dist[0][1]=10; g.dist[0][2]=15; g.dist[0][3]=20;
    // Row 1
    g.dist[1][0]=10; g.dist[1][1]=0; g.dist[1][2]=35; g.dist[1][3]=25;
    // Row 2
    g.dist[2][0]=15; g.dist[2][1]=35; g.dist[2][2]=0; g.dist[2][3]=30;
    // Row 3
    g.dist[3][0]=20; g.dist[3][1]=25; g.dist[3][2]=30; g.dist[3][3]=0;

    printf("Distance Matrix:\n");
    printf("       HQ   Clnc  Hosp  Pharm\n");
    printf("HQ      0    10    15    20\n");
    printf("Clnc   10     0    35    25\n");
    printf("Hosp   15    35     0    30\n");
    printf("Pharm  20    25    30     0\n");

    solveTSP(&g, 1); // Run with verbose trace
}

void runDynamicInput() {
    Graph g;
    printf("\n>>> Dynamic Input Mode <<<\n");
    printf("Enter number of cities (min 4, max %d): ", MAX_CITIES);
    scanf("%d", &g.n);
    if (g.n < 4 || g.n > MAX_CITIES) { printf("Invalid number.\n"); return; }

    printf("Enter City Names:\n");
    for(int i=0; i<g.n; i++) {
        printf(" City %d: ", i);
        scanf("%s", g.names[i]);
    }

    printf("Enter Distance Matrix (%d x %d):\n", g.n, g.n);
    for(int i=0; i<g.n; i++) {
        printf(" Row %d (distances from %s): ", i, g.names[i]);
        for(int j=0; j<g.n; j++) {
            scanf("%d", &g.dist[i][j]);
        }
    }
    
    int verbose = 0;
    printf("Show Self-Learning DP Trace? (1=Yes, 0=No): ");
    scanf("%d", &verbose);
    
    solveTSP(&g, verbose);
}

// --- Main ---

int main() {
    int choice = 0;
    while(choice != 3) {
        printf("\n=== TSP Route Optimizer (Dynamic Programming) ===\n");
        printf("1. Run Guided Demo\n");
        printf("2. Custom Network Input\n");
        printf("3. Exit\n");
        printf("Choice: ");
        if(scanf("%d", &choice)!=1) {
            while(getchar()!='\n');
            choice=0;
        }
        switch(choice) {
            case 1: runGuidedDemo(); break;
            case 2: runDynamicInput(); break;
            case 3: printf("Exiting.\n"); break;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
