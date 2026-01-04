#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // For INT_MAX

// --- 1. Graph Data Structures (Adjacency List) ---

// A node in the adjacency list
typedef struct AdjListNode {
    int dest;      // Destination vertex index
    int time;    // Time (weight) of the route
    struct AdjListNode* next;
} AdjListNode;

// The adjacency list for a single vertex
typedef struct AdjList {
    struct AdjListNode* head; // Pointer to the head of the list
} AdjList;

// The graph itself
typedef struct Graph {
    int V;               // Number of vertices (locations)
    struct AdjList* array; // Array of adjacency lists
} Graph;

// Create a new node for the adjacency list
AdjListNode* newAdjListNode(int dest, int time) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (newNode == NULL) {
        perror("malloc failed for AdjListNode");
        exit(EXIT_FAILURE);
    }
    newNode->dest = dest;
    newNode->time = time;
    newNode->next = NULL;
    return newNode;
}

// Create a graph with V vertices
Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        perror("malloc failed for Graph");
        exit(EXIT_FAILURE);
    }
    graph->V = V;

    // Create an array of adjacency lists
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));
    if (graph->array == NULL) {
        perror("malloc failed for AdjList array");
        exit(EXIT_FAILURE);
    }

    // Initialize each list as empty
    for (int i = 0; i < V; ++i) {
        graph->array[i].head = NULL;
    }
    return graph;
}

// Add a directed route (edge) to the graph
void addRoute(Graph* graph, int src, int dest, int time) {
    // --- Validation 1: No negative weights ---
    if (time < 0) {
        printf("Validation Error: Route time cannot be negative. Skipping.\n");
        return;
    }
    
    // Add an edge from src to dest
    AdjListNode* newNode = newAdjListNode(dest, time);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

// --- 2. Min-Heap Data Structures (Priority Queue) ---

// A node in the min-heap
typedef struct MinHeapNode {
    int v;    // Vertex index
    int dist; // Distance (time) from source
} MinHeapNode;

// The min-heap
typedef struct MinHeap {
    int size;     // Current number of nodes in heap
    int capacity; // Maximum capacity
    int* pos;     // To help with decreaseKey
    MinHeapNode** array; // Array of heap nodes
} MinHeap;

// Create a new min-heap node
MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* node = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    node->v = v;
    node->dist = dist;
    return node;
}

// Create a min-heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// Swap two min-heap nodes
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Heapify at a given index
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx) {
        // Swap nodes and their positions
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Check if heap is empty
int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

// Extract the minimum node from the heap
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;

    // Get the root
    MinHeapNode* root = minHeap->array[0];

    // Replace root with the last node
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update position
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    // Reduce size and heapify
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Decrease distance value of a vertex v
void decreaseKey(MinHeap* minHeap, int v, int dist) {
    // Get the index of v in the heap array
    int i = minHeap->pos[v];
    
    // Update the distance
    minHeap->array[i]->dist = dist;

    // "Bubble up" while parent is greater
    while (i > 0 && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        // Swap with parent
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        
        // move to parent's index
        i = (i - 1) / 2;
    }
}

// Check if a vertex is in the min-heap
int isInMinHeap(MinHeap* minHeap, int v) {
    if (minHeap->pos[v] < minHeap->size)
        return 1;
    return 0;
}

// --- 3. Dijkstra's Algorithm Implementation ---

void dijkstra(Graph* graph, int src) {
    int V = graph->V;
    int* dist = (int*)malloc(V * sizeof(int)); // Holds shortest times
    
    // --- Validation 2: Check for valid graph and source ---
    if (graph == NULL || src < 0 || src >= V) {
        printf("Validation Error: Invalid graph or source vertex.\n");
        if (dist) free(dist);
        return;
    }

    // Create and initialize the min-heap
    MinHeap* minHeap = createMinHeap(V);
    
    // Initialize distances and heap
    for (int v = 0; v < V; ++v) {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // Set source distance to 0 and update heap
    dist[src] = 0;
    decreaseKey(minHeap, src, 0);

    minHeap->size = V;

    // Main loop
    while (!isEmpty(minHeap)) {
        // Greedily extract the unvisited node with the smallest time
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Vertex index

        // Iterate over all neighbors
        AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL) {
            int v = pCrawl->dest;

            // If a shorter path is found
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
                pCrawl->time + dist[u] < dist[v]) 
            {
                dist[v] = dist[u] + pCrawl->time;
                // Update the distance in the priority queue
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
        free(minHeapNode); // Free the extracted node
    }

    // Print the results
    printf(" Shortest Delivery Times from: Warehouse (Node 0)\n");
    printf("--------------------------------------------------\n");
    char* locations[] = {"Warehouse", "Pharmacy A", "Hospital B", "Clinic C", "Patient Home D"};
    
    for (int i = 0; i < V; ++i) {
        if (dist[i] == INT_MAX) {
            printf("Location: %-18s (Node %d) | Time: UNREACHABLE\n", locations[i], i);
        } else {
            printf("Location: %-18s (Node %d) | Time: %d minutes\n", locations[i], i, dist[i]);
        }
    }

    // --- 4. Free all allocated memory ---
    free(dist);
    // Free heap (nodes were freed in extractMin)
    free(minHeap->pos);
    free(minHeap->array);
    free(minHeap);
}

// Helper function to free the entire graph
void freeGraph(Graph* graph) {
    if (graph == NULL) return;
    for (int i = 0; i < graph->V; i++) {
        AdjListNode* crawler = graph->array[i].head;
        while (crawler != NULL) {
            AdjListNode* temp = crawler;
            crawler = crawler->next;
            free(temp);
        }
    }
    free(graph->array);
    free(graph);
}


// --- 5. Main (Driver) Function ---

int main() {
    int V = 5; // 5 locations
    Graph* graph = createGraph(V);

    // Map:
    // 0 = Warehouse
    // 1 = Pharmacy A
    // 2 = Hospital B
    // 3 = Clinic C
    // 4 = Patient Home D

    // Add routes from the manual workout
    addRoute(graph, 0, 1, 10); // W -> A (10)
    addRoute(graph, 0, 2, 3);  // W -> B (3)
    addRoute(graph, 1, 2, 4);  // A -> B (4)
    addRoute(graph, 1, 3, 2);  // A -> C (2)
    addRoute(graph, 2, 1, 1);  // B -> A (1)
    addRoute(graph, 2, 3, 8);  // B -> C (8)
    addRoute(graph, 2, 4, 2);  // B -> D (2)
    addRoute(graph, 3, 4, 7);  // C -> D (7)
    addRoute(graph, 4, 3, 5);  // D -> C (5)

    // Run Dijkstra's from the Warehouse (Node 0)
    dijkstra(graph, 0);
    
    // Clean up memory
    freeGraph(graph);

    return 0;
}
