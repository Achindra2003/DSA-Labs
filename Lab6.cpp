#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NAME_LEN 64
#define MAX_EDGES 1000

typedef struct Edge {
    int src;
    int dest;
    int cost;
} Edge;

typedef struct AdjListNode {
    int dest;
    int cost;
    struct AdjListNode* next;
} AdjListNode;

typedef struct AdjList {
    struct AdjListNode* head;
} AdjList;

typedef struct Graph {
    int V;
    int E;
    struct AdjList* array;
    char** locationNames;
    Edge* edges; 
    int edgeCount;
} Graph;

typedef struct MinHeapNode {
    int v;
    int key;
    int parent;
} MinHeapNode;

typedef struct MinHeap {
    int size;
    int capacity;
    int* pos;
    MinHeapNode** array;
} MinHeap;

typedef struct UnionFind {
    int* parent;
    int* rank;
    int size;
} UnionFind;

typedef struct MSTResult {
    Edge* edges;
    int edgeCount;
    int totalCost;
} MSTResult;

AdjListNode* newAdjListNode(int dest, int cost) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

Graph* createGraph(int V, char* locationNamesInput[]) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->E = 0;
    graph->edgeCount = 0;
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));
    graph->locationNames = (char**)malloc(V * sizeof(char*));
    graph->edges = (Edge*)malloc(MAX_EDGES * sizeof(Edge));

    for (int i = 0; i < V; ++i) {
        graph->array[i].head = NULL;
        graph->locationNames[i] = strdup(locationNamesInput[i]);
    }
    return graph;
}

void addConnection(Graph* graph, int src, int dest, int cost) {
    if (cost < 0) {
        printf("Validation Error: Connection cost cannot be negative. Skipping.\n");
        return;
    }

    AdjListNode* newNode = newAdjListNode(dest, cost);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    newNode = newAdjListNode(src, cost);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;

    if (graph->edgeCount < MAX_EDGES) {
        graph->edges[graph->edgeCount].src = src;
        graph->edges[graph->edgeCount].dest = dest;
        graph->edges[graph->edgeCount].cost = cost;
        graph->edgeCount++;
    }
}

void freeGraph(Graph* graph) {
    if (graph == NULL) return;
    for (int i = 0; i < graph->V; i++) {
        AdjListNode* crawler = graph->array[i].head;
        while (crawler != NULL) {
            AdjListNode* temp = crawler;
            crawler = crawler->next;
            free(temp);
        }
        free(graph->locationNames[i]);
    }
    free(graph->locationNames);
    free(graph->array);
    free(graph->edges);
    free(graph);
}

int findLocationIndex(Graph* graph, char* name) {
    for (int i = 0; i < graph->V; i++) {
        if (strcmp(name, graph->locationNames[i]) == 0) {
            return i;
        }
    }
    return -1;
}

MinHeapNode* newMinHeapNode(int v, int key, int parent) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    node->v = v;
    node->key = key;
    node->parent = parent;
    return node;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->key < minHeap->array[smallest]->key)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->key < minHeap->array[smallest]->key)
        smallest = right;

    if (smallest != idx) {
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[idx];
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap)) return NULL;
    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}

void decreaseKey(MinHeap* minHeap, int v, int key, int parent) {
    int i = minHeap->pos[v];
    minHeap->array[i]->key = key;
    minHeap->array[i]->parent = parent;

    while (i > 0 && minHeap->array[i]->key < minHeap->array[(i - 1) / 2]->key) {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int isInMinHeap(MinHeap* minHeap, int v) {
    return minHeap->pos[v] < minHeap->size;
}

UnionFind* createUnionFind(int n) {
    UnionFind* uf = (UnionFind*)malloc(sizeof(UnionFind));
    uf->parent = (int*)malloc(n * sizeof(int));
    uf->rank = (int*)malloc(n * sizeof(int));
    uf->size = n;

    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
    return uf;
}

int find(UnionFind* uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = find(uf, uf->parent[x]); 
    }
    return uf->parent[x];
}

int unionSets(UnionFind* uf, int x, int y) {
    int xroot = find(uf, x);
    int yroot = find(uf, y);

    if (xroot == yroot) {
        return 0; 
    }

    if (uf->rank[xroot] < uf->rank[yroot]) {
        uf->parent[xroot] = yroot;
    } else if (uf->rank[xroot] > uf->rank[yroot]) {
        uf->parent[yroot] = xroot;
    } else {
        uf->parent[yroot] = xroot;
        uf->rank[xroot]++;
    }
    return 1;
}

void freeUnionFind(UnionFind* uf) {
    free(uf->parent);
    free(uf->rank);
    free(uf);
}

MSTResult* primMST(Graph* graph, int verbose) {
    int V = graph->V;
    int* parent = (int*)malloc(V * sizeof(int));
    int* key = (int*)malloc(V * sizeof(int));

    MinHeap* minHeap = createMinHeap(V);

    for (int v = 0; v < V; v++) {
        parent[v] = -1;
        key[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, key[v], -1);
        minHeap->pos[v] = v;
    }

    key[0] = 0;
    decreaseKey(minHeap, 0, key[0], -1);
    minHeap->size = V;

    if (verbose) {
        printf("\n--- Prim's Algorithm Self-Learning Trace ---\n");
        printf("[Start] Building MST from '%s'\n", graph->locationNames[0]);
        printf("Initial Keys: { ");
        for(int i=0; i<V; i++) printf("%s:%s ", graph->locationNames[i], (key[i] == INT_MAX) ? "Inf" : "0");
        printf("}\n");
    }

    MSTResult* result = (MSTResult*)malloc(sizeof(MSTResult));
    result->edges = (Edge*)malloc((V-1) * sizeof(Edge));
    result->edgeCount = 0;
    result->totalCost = 0;

    while (!isEmpty(minHeap)) {
        MinHeapNode* minNode = extractMin(minHeap);
        int u = minNode->v;

        if (verbose && minNode->parent != -1) {
            printf("\n(Step) --------------------------------------\n");
            printf(" -> GREEDY CHOICE: Adding edge '%s' <-> '%s' (Cost: %d)\n", 
                   graph->locationNames[minNode->parent], 
                   graph->locationNames[u], 
                   key[u]);
        }

        if (minNode->parent != -1) {
            result->edges[result->edgeCount].src = minNode->parent;
            result->edges[result->edgeCount].dest = u;
            result->edges[result->edgeCount].cost = key[u];
            result->edgeCount++;
            result->totalCost += key[u];
        }

        AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL) {
            int v = pCrawl->dest;

            if (isInMinHeap(minHeap, v) && pCrawl->cost < key[v]) {
                if (verbose) {
                    printf("     >> (UPDATE) Key of '%s': Old %s, New %d (via '%s')\n", 
                           graph->locationNames[v], 
                           (key[v]==INT_MAX)?"Inf":"Val", 
                           pCrawl->cost,
                           graph->locationNames[u]);
                }
                key[v] = pCrawl->cost;
                parent[v] = u;
                decreaseKey(minHeap, v, key[v], parent[v]);
            }
            pCrawl = pCrawl->next;
        }
        free(minNode);
    }

    if (verbose) printf("\n--- Prim's Trace Complete ---\n");

    free(parent);
    free(key);
    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);

    return result;
}

int compareEdges(const void* a, const void* b) {
    Edge* edge1 = (Edge*)a;
    Edge* edge2 = (Edge*)b;
    return edge1->cost - edge2->cost;
}

MSTResult* kruskalMST(Graph* graph, int verbose) {
    int V = graph->V;
    int E = graph->edgeCount;

    Edge* sortedEdges = (Edge*)malloc(E * sizeof(Edge));
    for (int i = 0; i < E; i++) {
        sortedEdges[i] = graph->edges[i];
    }
    qsort(sortedEdges, E, sizeof(Edge), compareEdges);

    if (verbose) {
        printf("\n--- Kruskal's Algorithm Self-Learning Trace ---\n");
        printf("[Start] Sorted edges by cost (Greedy approach):\n");
        for (int i = 0; i < E; i++) {
            printf("   Edge %d: %s <-> %s (Cost: %d)\n", i+1,
                   graph->locationNames[sortedEdges[i].src],
                   graph->locationNames[sortedEdges[i].dest],
                   sortedEdges[i].cost);
        }
        printf("\n");
    }

    MSTResult* result = (MSTResult*)malloc(sizeof(MSTResult));
    result->edges = (Edge*)malloc((V-1) * sizeof(Edge));
    result->edgeCount = 0;
    result->totalCost = 0;

    UnionFind* uf = createUnionFind(V);

    int edgesAdded = 0;
    for (int i = 0; i < E && edgesAdded < V - 1; i++) {
        Edge currentEdge = sortedEdges[i];
        int srcRoot = find(uf, currentEdge.src);
        int destRoot = find(uf, currentEdge.dest);

        if (verbose) {
            printf("(Step) Considering: %s <-> %s (Cost: %d)\n",
                   graph->locationNames[currentEdge.src],
                   graph->locationNames[currentEdge.dest],
                   currentEdge.cost);
        }

        if (srcRoot != destRoot) {

            result->edges[result->edgeCount] = currentEdge;
            result->edgeCount++;
            result->totalCost += currentEdge.cost;
            edgesAdded++;

            unionSets(uf, srcRoot, destRoot);

            if (verbose) {
                printf("   >> ACCEPTED: No cycle detected. Edge added to MST.\n");
                printf("   >> Union: Merged sets containing '%s' and '%s'\n\n",
                       graph->locationNames[currentEdge.src],
                       graph->locationNames[currentEdge.dest]);
            }
        } else {
            if (verbose) {
                printf("   >> REJECTED: Would create cycle (both in same set).\n\n");
            }
        }
    }

    if (verbose) printf("--- Kruskal's Trace Complete ---\n");

    free(sortedEdges);
    freeUnionFind(uf);

    return result;
}

void printMSTResult(Graph* graph, MSTResult* result, char* algorithmName) {
    printf("\n\n--- %s: Minimum Spanning Tree Result ---\n", algorithmName);
    printf("Total Cable/Network Cost: %d units\n", result->totalCost);
    printf("--------------------------------------------------------------------------\n");
    printf("| Edge #  | Connection                    | Cost (units)              |\n");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < result->edgeCount; i++) {
        printf("| %-7d | %-15s <-> %-10s | %-25d |\n", 
               i+1,
               graph->locationNames[result->edges[i].src],
               graph->locationNames[result->edges[i].dest],
               result->edges[i].cost);
    }
    printf("--------------------------------------------------------------------------\n");
}

void freeMSTResult(MSTResult* result) {
    if (result) {
        free(result->edges);
        free(result);
    }
}

void runComparativeAnalysis(Graph* graph) {
    printf("\n\n--- SLI: Automated Comparative Analysis (Prim vs Kruskal) ---\n");
    printf("Objective: Compare algorithm performance and validate MST correctness\n");
    printf("=========================================================================\n");

    MSTResult* primResult = primMST(graph, 0);
    MSTResult* kruskalResult = kruskalMST(graph, 0);

    printf("\n+++ CORRECTNESS VERIFICATION +++\n");
    printf("  Prim's Total Cost:    %d units\n", primResult->totalCost);
    printf("  Kruskal's Total Cost: %d units\n", kruskalResult->totalCost);

    if (primResult->totalCost == kruskalResult->totalCost) {
        printf("  >> STATUS: PASS - Both algorithms found the same minimum cost.\n");
        printf("  >> INTERPRETATION: The MST is correctly computed.\n");
    } else {
        printf("  >> STATUS: WARNING - Cost mismatch detected.\n");
        printf("  >> INTERPRETATION: Possible implementation issue or disconnected graph.\n");
    }

    printf("\n+++ EDGE COUNT VERIFICATION +++\n");
    printf("  Expected Edges (V-1): %d\n", graph->V - 1);
    printf("  Prim's Edges:         %d\n", primResult->edgeCount);
    printf("  Kruskal's Edges:      %d\n", kruskalResult->edgeCount);

    if (primResult->edgeCount == graph->V - 1 && kruskalResult->edgeCount == graph->V - 1) {
        printf("  >> STATUS: PASS - Correct number of edges for a spanning tree.\n");
    } else {
        printf("  >> STATUS: FAIL - Graph may be disconnected.\n");
    }

    printf("\n+++ NETWORK SAVINGS ANALYSIS +++\n");
    int totalPossibleCost = 0;
    for (int i = 0; i < graph->edgeCount; i++) {
        totalPossibleCost += graph->edges[i].cost;
    }
    printf("  Total cost if ALL connections used: %d units\n", totalPossibleCost);
    printf("  MST Cost:                            %d units\n", primResult->totalCost);
    printf("  Savings:                             %d units (%.2f%%)\n", 
           totalPossibleCost - primResult->totalCost,
           100.0 * (totalPossibleCost - primResult->totalCost) / totalPossibleCost);

    printf("\n+++ REDUNDANCY ANALYSIS +++\n");
    printf("  Original Edges:       %d\n", graph->edgeCount);
    printf("  MST Edges:            %d\n", primResult->edgeCount);
    printf("  Redundant Edges:      %d\n", graph->edgeCount - primResult->edgeCount);
    printf("  >> INTERPRETATION: %d connections can be eliminated without\n", 
           graph->edgeCount - primResult->edgeCount);
    printf("     affecting network connectivity.\n");

    printf("\n+++ ALGORITHM COMPLEXITY COMPARISON +++\n");
    printf("  Prim's Time Complexity:    O(E log V) with binary heap\n");
    printf("  Kruskal's Time Complexity: O(E log E) = O(E log V)\n");
    printf("  For this graph (V=%d, E=%d):\n", graph->V, graph->edgeCount);
    printf("    - Prim's is more efficient for DENSE graphs\n");
    printf("    - Kruskal's is better for SPARSE graphs\n");

    if (graph->edgeCount > (graph->V * (graph->V - 1)) / 4) {
        printf("  >> RECOMMENDATION: This is a DENSE graph. Prim's is optimal.\n");
    } else {
        printf("  >> RECOMMENDATION: This is a SPARSE graph. Kruskal's is optimal.\n");
    }

    printf("=========================================================================\n");

    freeMSTResult(primResult);
    freeMSTResult(kruskalResult);
}

void runGuidedDemo() {
    printf("\n=== GUIDED DEMO: Pharmacy Network Cable Installation ===\n");
    printf("Domain: Minimize cable cost to connect pharmacy locations\n\n");

    int V = 6;
    char* locationNames[] = { 
        "MainHub", 
        "PharmacyA", 
        "PharmacyB", 
        "Hospital", 
        "Warehouse", 
        "ClinicC" 
    };

    Graph* graph = createGraph(V, locationNames);

    printf("Network Topology:\n");
    addConnection(graph, 0, 1, 4);  printf("  MainHub <-> PharmacyA (Cost: 4)\n");
    addConnection(graph, 0, 2, 4);  printf("  MainHub <-> PharmacyB (Cost: 4)\n");
    addConnection(graph, 1, 2, 2);  printf("  PharmacyA <-> PharmacyB (Cost: 2)\n");
    addConnection(graph, 1, 3, 5);  printf("  PharmacyA <-> Hospital (Cost: 5)\n");
    addConnection(graph, 2, 3, 8);  printf("  PharmacyB <-> Hospital (Cost: 8)\n");
    addConnection(graph, 2, 4, 10); printf("  PharmacyB <-> Warehouse (Cost: 10)\n");
    addConnection(graph, 3, 4, 11); printf("  Hospital <-> Warehouse (Cost: 11)\n");
    addConnection(graph, 3, 5, 9);  printf("  Hospital <-> ClinicC (Cost: 9)\n");
    addConnection(graph, 4, 5, 18); printf("  Warehouse <-> ClinicC (Cost: 18)\n");

    int verbose = 0;
    printf("\nEnable Self-Learning Trace (Verbose Mode)? (1=Yes, 0=No): ");
    scanf("%d", &verbose);

    printf("\n\n========== RUNNING PRIM'S ALGORITHM ==========");
    MSTResult* primResult = primMST(graph, verbose);
    printMSTResult(graph, primResult, "Prim's Algorithm");

    printf("\n\n========== RUNNING KRUSKAL'S ALGORITHM ==========");
    MSTResult* kruskalResult = kruskalMST(graph, verbose);
    printMSTResult(graph, kruskalResult, "Kruskal's Algorithm");

    runComparativeAnalysis(graph);

    freeMSTResult(primResult);
    freeMSTResult(kruskalResult);
    freeGraph(graph);
}

void runDynamicInput() {
    int V, E;
    char buffer[MAX_NAME_LEN];

    printf("\n=== DYNAMIC INPUT MODE ===\n");
    printf("Enter number of locations: ");
    scanf("%d", &V);
    if (V <= 0) {
        printf("Invalid number of locations.\n");
        return;
    }

    char** locationNames = (char**)malloc(V * sizeof(char*));
    printf("Enter names of %d locations:\n", V);
    for (int i = 0; i < V; i++) {
        printf(" Location %d: ", i);
        scanf("%s", buffer);
        locationNames[i] = strdup(buffer);
    }

    Graph* graph = createGraph(V, locationNames);
    for(int i=0; i<V; i++) free(locationNames[i]);
    free(locationNames);

    printf("Enter number of connections: ");
    scanf("%d", &E);
    printf("Enter connections (Location1 Location2 Cost):\n");
    for (int i = 0; i < E; i++) {
        char loc1[64], loc2[64];
        int cost;
        printf(" Connection %d: ", i+1);
        scanf("%s %s %d", loc1, loc2, &cost);

        int u = findLocationIndex(graph, loc1);
        int v = findLocationIndex(graph, loc2);

        if (u != -1 && v != -1) {
            addConnection(graph, u, v, cost);
        } else {
            printf("Invalid location names. Skipping connection.\n");
        }
    }

    int verbose = 0;
    printf("\nEnable Verbose Trace? (1=Yes, 0=No): ");
    scanf("%d", &verbose);

    printf("\n\n========== RUNNING PRIM'S ALGORITHM ==========");
    MSTResult* primResult = primMST(graph, verbose);
    printMSTResult(graph, primResult, "Prim's Algorithm");

    printf("\n\n========== RUNNING KRUSKAL'S ALGORITHM ==========");
    MSTResult* kruskalResult = kruskalMST(graph, verbose);
    printMSTResult(graph, kruskalResult, "Kruskal's Algorithm");

    runComparativeAnalysis(graph);

    freeMSTResult(primResult);
    freeMSTResult(kruskalResult);
    freeGraph(graph);
}

int main() {
    int choice = 0;

    while (choice != 3) {
        printf("\n\n");
        printf("============================================================\n");
        printf("   MINIMUM SPANNING TREE - PHARMACY NETWORK OPTIMIZER\n");
        printf("============================================================\n");
        printf(" Domain: Cable Installation Cost Minimization\n");
        printf(" Algorithms: Prim's & Kruskal's\n");
        printf("============================================================\n");
        printf(" 1. Run Guided Demo (Predefined Network)\n");
        printf(" 2. Run Dynamic Input (Custom Network)\n");
        printf(" 3. Exit\n");
        printf("============================================================\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            while(getchar()!='\n');
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
                printf("\n>>> Exiting program. Thank you!\n");
                break;
            default: 
                printf("\nInvalid choice. Please try again.\n");
        }
    }

    return 0;
}


