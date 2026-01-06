/**
 * @file pharmacy_logistics.c
 * @brief Intelligent Stock Transfer System using Floyd-Warshall Algorithm
 * @domain Pharmacy Management System
 * * SELF-LEARNING INITIATIVES:
 * 1. Named Locations: Maps IDs to Strings for clear business context.
 * 2. Decision Support Module: Automated recommendation engine for stockouts.
 * 3. Formatted Reporting: Professional table output.
 * 4. Guided Demo: Pre-configured scenario for instant visualization.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 99999
#define MAX_NODES 10
#define NAME_LEN 20

// --- Data Structures ---

typedef struct {
    int id;
    char name[NAME_LEN];
} Location;

typedef struct {
    int V; // Number of active locations
    int adj[MAX_NODES][MAX_NODES]; // Adjacency Matrix (Time in mins)
    Location locations[MAX_NODES]; // Metadata for locations
} PharmacyNetwork;

// --- Helper Functions ---

void initNetwork(PharmacyNetwork* net, int V) {
    net->V = V;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) net->adj[i][j] = 0;
            else net->adj[i][j] = INF;
        }
        // Default naming if not provided
        sprintf(net->locations[i].name, "Branch_%d", i+1);
        net->locations[i].id = i;
    }
}

// Pretty prints the Logistics Matrix
void printLogisticsTable(PharmacyNetwork net, int dist[MAX_NODES][MAX_NODES]) {
    printf("\n+-------------------------------------------------------------+\n");
    printf("| LOGISTICS TIME MATRIX (Minutes)                             |\n");
    printf("+------------+");
    for(int i=0; i<net.V; i++) printf("------------+");
    printf("\n| %-10s |", "FROM \\ TO");
    
    // Print Column Headers
    for(int i=0; i<net.V; i++) {
        // Truncate name to 10 chars for table alignment
        char temp[11];
        strncpy(temp, net.locations[i].name, 10);
        temp[10] = '\0';
        printf(" %-10s |", temp);
    }
    printf("\n+------------+");
    for(int i=0; i<net.V; i++) printf("------------+");
    printf("\n");

    // Print Rows
    for (int i = 0; i < net.V; i++) {
        char temp[11];
        strncpy(temp, net.locations[i].name, 10);
        temp[10] = '\0';
        printf("| %-10s |", temp); // Row Header
        
        for (int j = 0; j < net.V; j++) {
            if (dist[i][j] == INF)
                printf(" %-10s |", "Unreach");
            else
                printf(" %-10d |", dist[i][j]);
        }
        printf("\n");
    }
    printf("+------------+");
    for(int i=0; i<net.V; i++) printf("------------+");
    printf("\n");
}

// --- Domain Logic: Decision Support ---

/**
 * @brief Simulates a Stockout scenario and finds the best supplier.
 * This connects the algorithm to the specific domain problem.
 */
void recommendStockSource(PharmacyNetwork net, int dist[MAX_NODES][MAX_NODES], int targetLocId) {
    printf("\n[ ALERT ] CRITICAL STOCKOUT REPORTED AT: %s\n", net.locations[targetLocId].name);
    printf("[ ACTION ] Searching for nearest surplus stock...\n");

    int bestSource = -1;
    int minTime = INF;

    // Check all possible sources (i -> targetLocId)
    // Note: In logistics, we need the path FROM source TO target.
    for (int i = 0; i < net.V; i++) {
        if (i == targetLocId) continue; // Cannot supply self

        // If reachable and faster than current best
        if (dist[i][targetLocId] != INF && dist[i][targetLocId] < minTime) {
            minTime = dist[i][targetLocId];
            bestSource = i;
        }
    }

    if (bestSource != -1) {
        printf(">> RECOMMENDATION: Request stock from '%s'.\n", net.locations[bestSource].name);
        printf(">> ESTIMATED ARRIVAL: %d minutes.\n", minTime);
    } else {
        printf(">> FAILURE: No connected supply sources found! Emergency procurement needed.\n");
    }
}

// --- Core Algorithm ---

void runSupplyOptimization(PharmacyNetwork* net) {
    int dist[MAX_NODES][MAX_NODES];
    int i, j, k;

    // Initialize distance matrix
    for (i = 0; i < net->V; i++) {
        for (j = 0; j < net->V; j++) {
            dist[i][j] = net->adj[i][j];
        }
    }

    // Floyd-Warshall Logic
    for (k = 0; k < net->V; k++) {
        for (i = 0; i < net->V; i++) {
            for (j = 0; j < net->V; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF && 
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    printLogisticsTable(*net, dist);

    // SLI: Run a simulation for the last added node (usually the furthest edge case)
    recommendStockSource(*net, dist, net->V - 1);
}

// --- Driver Modes ---

void runGuidedDemo() {
    PharmacyNetwork net;
    int V = 4;
    initNetwork(&net, V);

    printf("\n--- 1. Running Guided Demo ---\n");
    printf("Scenario: 4 Locations (Warehouse, City, Suburb, Emergency)\n");

    // 1. Setup Names
    strcpy(net.locations[0].name, "Warehouse");
    strcpy(net.locations[1].name, "CityPharm");
    strcpy(net.locations[2].name, "SuburbClin");
    strcpy(net.locations[3].name, "EmergCenter");

    // 2. Setup Routes (Hardcoded Adjacency Matrix)
    // Route: Warehouse -> CityPharm (5 mins)
    net.adj[0][1] = 5;
    
    // Route: CityPharm -> SuburbClin (4 mins)
    net.adj[1][2] = 4;
    
    // Route: Warehouse -> SuburbClin (Direct but traffic: 15 mins)
    // Note: The algo should realize Warehouse->City->Suburb (5+4=9) is faster than direct (15)
    net.adj[0][2] = 15;

    // Route: SuburbClin -> EmergCenter (6 mins)
    net.adj[2][3] = 6;

    // Route: CityPharm -> EmergCenter (Direct: 20 mins)
    net.adj[1][3] = 20;

    printf("Topology Defined:\n");
    printf("  - Warehouse -> CityPharm (5m)\n");
    printf("  - CityPharm -> SuburbClin (4m)\n");
    printf("  - Warehouse -> SuburbClin (15m - High Traffic)\n");
    printf("  - SuburbClin -> EmergCenter (6m)\n");
    printf("  - CityPharm -> EmergCenter (20m)\n");

    printf("\n[SYSTEM] Optimizing Supply Chain Routes...\n");
    runSupplyOptimization(&net);
}

void runDynamicInput() {
    int V, E, src, dest, weight;
    PharmacyNetwork net;

    printf("\n--- 2. Running Dynamic Input Mode ---\n");
    printf("Enter number of Pharmacy Branches: ");
    if (scanf("%d", &V) != 1 || V < 1 || V > MAX_NODES) {
        printf("Invalid input.\n");
        return;
    }

    initNetwork(&net, V);

    printf("\n--- Network Configuration ---\n");
    for(int i=0; i<V; i++) {
        printf("Enter name for Location ID %d (e.g., Warehouse): ", i);
        scanf("%s", net.locations[i].name);
    }

    printf("\nEnter number of transport routes: ");
    scanf("%d", &E);

    printf("Enter Routes (SourceID DestID Time_Minutes):\n");
    for (int i = 0; i < E; i++) {
        printf("Route %d: ", i+1);
        scanf("%d %d %d", &src, &dest, &weight);
        if(src >= 0 && src < V && dest >= 0 && dest < V) {
            net.adj[src][dest] = weight;
        } else {
            printf("  Invalid IDs ignored.\n");
        }
    }

    printf("\n[SYSTEM] Optimizing Supply Chain Routes...\n");
    runSupplyOptimization(&net);
}

// --- Main Menu ---

int main() {
    int choice = 0;
    while (choice != 3) {
        printf("\n\n=== Intelligent Pharmacy Logistics System (Floyd-Warshall) ===\n");
        printf("1. Run Guided Demo (4-Node Network)\n");
        printf("2. Enter Dynamic Graph\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear buffer
            choice = 0;
        }

        switch (choice) {
            case 1:
                runGuidedDemo();
                break;
            case 2:
                runDynamicInput();
                break;
            case 3:
                printf("Exiting System.\n");
                break;
            default:
                printf("Invalid choice. Please enter 1, 2, or 3.\n");
        }
    }
    return 0;
}
