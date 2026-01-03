#include <stdio.h>
#include <stdlib.h>

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode *front, *rear;
} Queue;

Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue* q, int value) {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    temp->data = value;
    temp->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

int dequeue(Queue* q) {
    if (q->front == NULL) return -1;
    QueueNode* temp = q->front;
    int data = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return data;
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

typedef struct Graph {
    int V;
    AdjListNode** adj;
    int* in_degree;
} Graph;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getInt() {
    int value, result;
    do {
        printf("> ");
        result = scanf("%d", &value);
        if (result != 1) {
            printf("!! Invalid input. Please enter an integer.\n");
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    } while (1);
}

Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;

    graph->adj = (AdjListNode**)malloc(V * sizeof(AdjListNode*));

    graph->in_degree = (int*)calloc(V, sizeof(int));

    for (int i = 0; i < V; ++i)
        graph->adj[i] = NULL;

    return graph;
}

void addEdge(Graph* graph, int src, int dest) {
    if (src >= graph->V || dest >= graph->V || src < 0 || dest < 0) {
        printf("!! Invalid vertex number.\n");
        return;
    }
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = graph->adj[src];
    graph->adj[src] = newNode;

    graph->in_degree[dest]++;

    printf("-> Edge from %d to %d added.\n", src, dest);
}

void freeGraph(Graph* graph) {
    if (graph == NULL) return;
    for (int i = 0; i < graph->V; i++) {
        AdjListNode* current = graph->adj[i];
        while (current != NULL) {
            AdjListNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adj);
    free(graph->in_degree);
    free(graph);
}

void printGraph(Graph* graph) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return;
    }
    printf("\n--- Graph Adjacency List ---\n");
    for (int v = 0; v < graph->V; ++v) {
        AdjListNode* temp = graph->adj[v];

        printf("Vertex %d (in-degree: %d): ", v, graph->in_degree[v]);
        while (temp) {
            printf("-> %d ", temp->dest);
            temp = temp->next;
        }
        printf("\n");
    }
}

void DFTUtil(Graph* graph, int v, int visited[]) {
    visited[v] = 1;
    printf("%d ", v);
    AdjListNode* temp = graph->adj[v];
    while (temp != NULL) {
        if (!visited[temp->dest])
            DFTUtil(graph, temp->dest, visited);
        temp = temp->next;
    }
}

void DFT(Graph* graph) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return;
    }
    int* visited = (int*)calloc(graph->V, sizeof(int));
    printf("Depth First Traversal: ");
    for (int i = 0; i < graph->V; i++)
        if (!visited[i])
            DFTUtil(graph, i, visited);
    printf("\n");
    free(visited);
}

void BFT(Graph* graph) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return;
    }
    int* visited = (int*)calloc(graph->V, sizeof(int));
    Queue* q = createQueue();
    printf("Breadth First Traversal: ");
    for (int i = 0; i < graph->V; i++) {
        if (!visited[i]) {
            visited[i] = 1;
            enqueue(q, i);
            while (!isQueueEmpty(q)) {
                int v = dequeue(q);
                printf("%d ", v);
                AdjListNode* temp = graph->adj[v];
                while (temp != NULL) {
                    if (!visited[temp->dest]) {
                        visited[temp->dest] = 1;
                        enqueue(q, temp->dest);
                    }
                    temp = temp->next;
                }
            }
        }
    }
    printf("\n");
    free(visited);
    free(q);
}

int isDAG_DFT_Util(Graph* graph, int v, int visited[], int recStack[]) {
    if (visited[v] == 0) {
        visited[v] = 1;
        recStack[v] = 1;
        AdjListNode* temp = graph->adj[v];
        while (temp != NULL) {
            if (!visited[temp->dest] && isDAG_DFT_Util(graph, temp->dest, visited, recStack))
                return 1;
            else if (recStack[temp->dest])
                return 1;
            temp = temp->next;
        }
    }
    recStack[v] = 0;
    return 0;
}

int isDAG_DFT(Graph* graph) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return 0;
    }
    int* visited = (int*)calloc(graph->V, sizeof(int));
    int* recStack = (int*)calloc(graph->V, sizeof(int));
    for (int i = 0; i < graph->V; i++)
        if (isDAG_DFT_Util(graph, i, visited, recStack)) {
            free(visited);
            free(recStack);
            return 0;
        }
    free(visited);
    free(recStack);
    return 1;
}

void topoSort_DFT_Util(Graph* graph, int v, int visited[], int* stack, int* stackIndex) {
    visited[v] = 1;
    AdjListNode* temp = graph->adj[v];
    while (temp != NULL) {
        if (!visited[temp->dest])
            topoSort_DFT_Util(graph, temp->dest, visited, stack, stackIndex);
        temp = temp->next;
    }
    stack[(*stackIndex)++] = v;
}

void topologicalSort_DFT(Graph* graph) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return;
    }
    if (!isDAG_DFT(graph)) {
        printf("!! Error: Graph contains a cycle. Topological sort not possible.\n");
        return;
    }
    int* stack = (int*)malloc(graph->V * sizeof(int));
    int stackIndex = 0;
    int* visited = (int*)calloc(graph->V, sizeof(int));

    for (int i = 0; i < graph->V; i++)
        if (!visited[i])
            topoSort_DFT_Util(graph, i, visited, stack, &stackIndex);

    printf("Topological Sort (DFT-Based): ");
    while (stackIndex > 0)
        printf("%d ", stack[--stackIndex]);
    printf("\n");
    free(stack);
    free(visited);
}

int topologicalSort_Kahn(Graph* graph, int printSort) {
    if (graph == NULL) {
        printf("!! Graph not created yet.\n");
        return 0;
    }

    int* in_degree_copy = (int*)malloc(graph->V * sizeof(int));
    for (int i = 0; i < graph->V; i++)
        in_degree_copy[i] = graph->in_degree[i];

    Queue* q = createQueue();
    for (int i = 0; i < graph->V; i++)
        if (in_degree_copy[i] == 0)
            enqueue(q, i);

    int count = 0;
    int* sortedOrder = (int*)malloc(graph->V * sizeof(int));

    while (!isQueueEmpty(q)) {
        int u = dequeue(q);
        sortedOrder[count] = u;
        count++;

        AdjListNode* temp = graph->adj[u];
        while (temp != NULL) {
            in_degree_copy[temp->dest]--;
            if (in_degree_copy[temp->dest] == 0)
                enqueue(q, temp->dest);
            temp = temp->next;
        }
    }

    free(in_degree_copy);
    free(q);

    if (count != graph->V) {
        if(printSort) printf("!! Error: Graph contains a cycle. Topological sort not possible.\n");
        free(sortedOrder);
        return 0;
    }

    if (printSort) {
        printf("Topological Sort (Kahn's): ");
        for (int i = 0; i < graph->V; i++)
            printf("%d ", sortedOrder[i]);
        printf("\n");
    }

    free(sortedOrder);
    return 1;
}

Graph* loadDemoGraph(Graph* oldGraph) {
    if (oldGraph != NULL) {
        freeGraph(oldGraph);
    }
    printf("\n--- Loading Demo Graph (Course Prerequisites) ---\n");
    Graph* graph = createGraph(6);

    addEdge(graph, 5, 0);
    addEdge(graph, 5, 2);
    addEdge(graph, 4, 0);
    addEdge(graph, 4, 1);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 1);
    printf("-> Demo Graph (DAG with 6 vertices) loaded.\n");
    printGraph(graph);
    return graph;
}

int main() {
    Graph* graph = NULL;
    int choice;

    while (1) {
        printf("\n===== Directed Graph Menu (with Initiatives) =====\n");
        printf("1. Load Demo Graph (DAG for testing)\n");
        printf("2. Create Custom Graph\n");
        printf("3. Traversal (BFT & DFT)\n");
        printf("4. Topological Sort (Demo of 2 methods)\n");
        printf("5. Check if graph is a DAG (Demo of 2 methods)\n");
        printf("6. Exit\n");

        choice = getInt();

        switch (choice) {
            case 1:
                graph = loadDemoGraph(graph);
                break;
            case 2: {
                if (graph != NULL) {
                    freeGraph(graph);
                    printf("-> Existing graph cleared.\n");
                }
                printf("Enter the number of vertices: ");
                int V = getInt();
                if (V <= 0) {
                    printf("!! Number of vertices must be positive.\n");
                    break;
                }
                graph = createGraph(V);
                printf("Graph with %d vertices created. Now add edges.\n", V);
                printf("Enter edges (e.g., '1 2' for edge 1->2). Type '-1 -1' to stop.\n");
                while(1) {
                    int src, dest;
                    printf("Edge (src dest): ");
                    scanf("%d %d", &src, &dest);
                    clearInputBuffer();
                    if (src == -1 || dest == -1) break;
                    addEdge(graph, src, dest);
                }
                printGraph(graph);
                break;
            }
            case 3: {
                if (graph == NULL) {
                    printf("!! Graph not created yet.\n");
                    break;
                }
                printf("\n-- Traversal Menu --\n");
                printf("1. Breadth First Traversal (BFT)\n");
                printf("2. Depth First Traversal (DFT)\n");
                int travChoice = getInt();
                if (travChoice == 1) BFT(graph);
                else if (travChoice == 2) DFT(graph);
                else printf("!! Invalid choice.\n");
                break;
            }
            case 4: {
                if (graph == NULL) {
                    printf("!! Graph not created yet.\n");
                    break;
                }
                printf("\n-- Topological Sort Menu --\n");
                printf("1. DFT-Based (Stack method)\n");
                printf("2. Kahn's Algorithm (In-Degree/Queue method)\n");
                int sortChoice = getInt();
                if (sortChoice == 1) topologicalSort_DFT(graph);
                else if (sortChoice == 2) topologicalSort_Kahn(graph, 1);
                else printf("!! Invalid choice.\n");
                break;
            }
            case 5: {
                if (graph == NULL) {
                    printf("!! Graph not created yet.\n");
                    break;
                }
                printf("\n-- Cycle Check (DAG) Menu --\n");
                printf("1. DFT-Based (Recursive Stack method)\n");
                printf("2. Kahn's-Based (In-Degree method)\n");
                int dagChoice = getInt();
                if (dagChoice == 1) {
                    if (isDAG_DFT(graph)) printf("Result (DFT): The graph is a DAG.\n");
                    else printf("Result (DFT): The graph contains a cycle.\n");
                } else if (dagChoice == 2) {
                    if (topologicalSort_Kahn(graph, 0)) printf("Result (Kahn's): The graph is a DAG.\n");
                    else printf("Result (Kahn's): The graph contains a cycle.\n");
                } else {
                    printf("!! Invalid choice.\n");
                }
                break;
            }
            case 6: {
                printf("Exiting. Freeing graph memory...\n");
                freeGraph(graph);
                return 0;
            }
            default:
                printf("!! Invalid selection. Please try again.\n");
        }
    }
    return 0;
}
