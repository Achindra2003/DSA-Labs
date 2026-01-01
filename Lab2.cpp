#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 5 

typedef struct {
    int prescriptionID;
    int priority; 
} Prescription;

void pressEnterToContinue() {
    printf("\n... Press Enter to continue ...\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    getchar(); 
}

typedef struct {
    Prescription items[MAX_SIZE];
    int front;
    int rear;
} NormalQueue;

NormalQueue* createNormalQueue() {
    NormalQueue* q = (NormalQueue*)malloc(sizeof(NormalQueue));
    q->front = -1;
    q->rear = -1;
    return q;
}

int nq_isFull(NormalQueue* q) { return q->rear == MAX_SIZE - 1; }
int nq_isEmpty(NormalQueue* q) { return q->front == -1 || q->front > q->rear; }

void nq_enqueue(NormalQueue* q, Prescription p) {
    if (nq_isFull(q)) {
        printf("!! Overflow: Standard line is full. Cannot add Prescription #%d.\n", p.prescriptionID);
        return;
    }
    if (nq_isEmpty(q)) q->front = 0;
    q->items[++q->rear] = p;
    printf("-> Added Prescription #%d to line. (Status: Front=%d, Rear=%d)\n", p.prescriptionID, q->front, q->rear);
}

Prescription nq_dequeue(NormalQueue* q) {
    if (nq_isEmpty(q)) {
        printf("!! Underflow: Standard line is empty.\n");
        return (Prescription){-1, -1};
    }
    Prescription p = q->items[q->front++];
    printf("<- Filled Prescription #%d. (Status: Front=%d, Rear=%d)\n", p.prescriptionID, q->front, q->rear);
    return p;
}

void nq_display(NormalQueue* q) {
    if (nq_isEmpty(q)) {
        printf("Display: The standard prescription line is empty.\n");
        return;
    }
    printf("Display: Standard Line [FRONT to REAR]: ");
    for (int i = q->front; i <= q->rear; i++) {
        printf("#%d ", q->items[i].prescriptionID);
    }
    printf("\n");
}

typedef struct {
    Prescription items[MAX_SIZE];
    int front;
    int rear;
} CircularQueue;

CircularQueue* createCircularQueue() {
    CircularQueue* q = (CircularQueue*)malloc(sizeof(CircularQueue));
    q->front = -1;
    q->rear = -1;
    return q;
}

int cq_isEmpty(CircularQueue* q) { return q->front == -1; }
int cq_isFull(CircularQueue* q) { return (q->rear + 1) % MAX_SIZE == q->front; }

void cq_enqueue(CircularQueue* q, Prescription p) {
    if (cq_isFull(q)) {
        printf("!! Overflow: Efficient line is full. Cannot add Prescription #%d.\n", p.prescriptionID);
        return;
    }
    if (cq_isEmpty(q)) q->front = 0;
    q->rear = (q->rear + 1) % MAX_SIZE;
    q->items[q->rear] = p;
    printf("-> Added Prescription #%d to line. (Status: Front=%d, Rear=%d)\n", p.prescriptionID, q->front, q->rear);
}

Prescription cq_dequeue(CircularQueue* q) {
    if (cq_isEmpty(q)) {
        printf("!! Underflow: Efficient line is empty.\n");
        return (Prescription){-1, -1};
    }
    Prescription p = q->items[q->front];
    if (q->front == q->rear) { 
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % MAX_SIZE;
    }
    printf("<- Filled Prescription #%d. (Status: Front=%d, Rear=%d)\n", p.prescriptionID, q->front, q->rear);
    return p;
}

void cq_display(CircularQueue* q) {
    if (cq_isEmpty(q)) {
        printf("Display: The efficient prescription line is empty.\n");
        return;
    }
    printf("Display: Efficient Line [FRONT to REAR]: ");
    int i;
    for (i = q->front; i != q->rear; i = (i + 1) % MAX_SIZE) {
        printf("#%d ", q->items[i].prescriptionID);
    }
    printf("#%d \n", q->items[i].prescriptionID);
}

typedef struct {
    Prescription heap[MAX_SIZE];
    int size;
} PriorityQueue;

PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    return pq;
}

int pq_isEmpty(PriorityQueue* pq) { return pq->size == 0; }
int pq_isFull(PriorityQueue* pq) { return pq->size == MAX_SIZE; }

void swap(Prescription* a, Prescription* b) {
    Prescription temp = *a; *a = *b; *b = temp;
}

void heapifyUp(PriorityQueue* pq, int index) {
    while (index > 0 && pq->heap[index].priority < pq->heap[(index - 1) / 2].priority) {
        swap(&pq->heap[index], &pq->heap[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

void heapifyDown(PriorityQueue* pq, int index) {
    int minIndex = index;
    while (1) {
        int left = 2 * index + 1, right = 2 * index + 2;
        if (left < pq->size && pq->heap[left].priority < pq->heap[minIndex].priority) minIndex = left;
        if (right < pq->size && pq->heap[right].priority < pq->heap[minIndex].priority) minIndex = right;
        if (index == minIndex) break;
        swap(&pq->heap[index], &pq->heap[minIndex]);
        index = minIndex;
    }
}

void pq_enqueue(PriorityQueue* pq, Prescription p) {
    if (pq_isFull(pq)) { printf("!! Overflow: Priority line is full.\n"); return; }
    pq->heap[pq->size] = p;
    heapifyUp(pq, pq->size);
    pq->size++;
    printf("-> Added Prescription #%d (Priority: %d). (Status: Size=%d)\n", p.prescriptionID, p.priority, pq->size);
}

Prescription pq_dequeue(PriorityQueue* pq) {
    if (pq_isEmpty(pq)) { printf("!! Underflow: Priority line is empty.\n"); return (Prescription){-1, -1}; }
    Prescription root = pq->heap[0];
    pq->heap[0] = pq->heap[--pq->size];
    heapifyDown(pq, 0);
    printf("<- Filled HI-PRIORITY Prescription #%d (Priority: %d). (Status: Size=%d)\n", root.prescriptionID, root.priority, pq->size);
    return root;
}

void pq_display(PriorityQueue* pq) {
    if (pq_isEmpty(pq)) { printf("Display: The priority line is empty.\n"); return; }
    printf("Display: Priority Line [Heap order, not sorted]: ");
    for (int i = 0; i < pq->size; i++) {
        printf("#%d(P:%d) ", pq->heap[i].prescriptionID, pq->heap[i].priority);
    }
    printf("\n");
}


typedef struct DequeNode {
    Prescription data;
    struct DequeNode* next;
    struct DequeNode* prev;
} DequeNode;

typedef struct {
    DequeNode* front;
    DequeNode* rear;
} Deque;

Deque* createDeque() {
    Deque* d = (Deque*)malloc(sizeof(Deque));
    d->front = NULL;
    d->rear = NULL;
    return d;
}

int dq_isEmpty(Deque* d) { return d->front == NULL; }

void dq_insertFront(Deque* d, Prescription p) {
    DequeNode* newNode = (DequeNode*)malloc(sizeof(DequeNode));
    newNode->data = p;
    newNode->prev = NULL;
    newNode->next = d->front;
    if (dq_isEmpty(d)) {
        d->front = d->rear = newNode;
    } else {
        d->front->prev = newNode;
        d->front = newNode;
    }
    printf("-> Inserted Front Rx #%d.\n", p.prescriptionID);
}

void dq_insertRear(Deque* d, Prescription p) {
    DequeNode* newNode = (DequeNode*)malloc(sizeof(DequeNode));
    newNode->data = p;
    newNode->next = NULL;
    newNode->prev = d->rear;
    if (dq_isEmpty(d)) {
        d->front = d->rear = newNode;
    } else {
        d->rear->next = newNode;
        d->rear = newNode;
    }
    printf("-> Inserted Rear Rx #%d.\n", p.prescriptionID);
}

Prescription dq_deleteFront(Deque* d) {
    if (dq_isEmpty(d)) { printf(" Underflow: Deque is empty.\n"); return (Prescription){-1,-1}; }
    DequeNode* temp = d->front;
    Prescription p = temp->data;
    d->front = d->front->next;
    if (d->front == NULL) d->rear = NULL;
    else d->front->prev = NULL;
    free(temp);
    printf("<- Deleted Front Rx #%d.\n", p.prescriptionID);
    return p;
}

Prescription dq_deleteRear(Deque* d) {
    if (dq_isEmpty(d)) { printf(" Underflow: Deque is empty.\n"); return (Prescription){-1,-1}; }
    DequeNode* temp = d->rear;
    Prescription p = temp->data;
    d->rear = d->rear->prev;
    if (d->rear == NULL) d->front = NULL;
    else d->rear->next = NULL;
    free(temp);
    printf("<- Deleted Rear Rx #%d.\n", p.prescriptionID);
    return p;
}

void dq_display(Deque* d) {
    if (dq_isEmpty(d)) { printf("Display: Deque is empty.\n"); return; }
    printf("Display: Deque [FRONT to REAR]: ");
    DequeNode* current = d->front;
    while (current != NULL) {
        printf("#%d ", current->data.prescriptionID);
        current = current->next;
    }
    printf("\n");
}

void demoNormalQueue(NormalQueue* q) {
    printf("\n--- [Running Guided Demo for Normal Queue] ---\n");
    printf("\n[SCENARIO 1: Standard FIFO Processing]\n");
    printf("Action: Three prescriptions arrive in order (#101, #102, #103).\n");
    nq_enqueue(q, (Prescription){101, 0}); nq_enqueue(q, (Prescription){102, 0}); nq_enqueue(q, (Prescription){103, 0});
    nq_display(q); 
    pressEnterToContinue();
    printf("\nAction: The first prescription in line (#101) is filled.\n");
    nq_dequeue(q);
    nq_display(q); 
    pressEnterToContinue();
    printf("\n[SCENARIO 2: Demonstrating Wasted Space ('Queue Drift')]\n");
    printf("Action: The remaining prescriptions are filled.\n");
    nq_dequeue(q); nq_dequeue(q);
    nq_display(q); 
    printf("STATUS: Notice the Front pointer is now %d. The space at the start of the array is now wasted.\n", q->front);
    printf("--- [Demo Complete. Returning to menu.] ---\n");
}

void demoCircularQueue(CircularQueue* q) {
    printf("\n--- [Running Guided Demo for Circular Queue] ---\n");
    printf("\n[SCENARIO 1: A Busy Day - Filling & Making Space]\n");
    printf("Action: The line is filled to capacity.\n");
    cq_enqueue(q, (Prescription){201, 0}); cq_enqueue(q, (Prescription){202, 0});
    cq_enqueue(q, (Prescription){203, 0}); cq_enqueue(q, (Prescription){204, 0});
    cq_enqueue(q, (Prescription){205, 0});
    cq_display(q); 
    pressEnterToContinue();
    printf("\nAction: Two prescriptions are filled, making space at the front.\n");
    cq_dequeue(q); cq_dequeue(q);
    cq_display(q); 
    pressEnterToContinue();
    printf("\n[SCENARIO 2: New Prescriptions Reusing Space]\n");
    printf("Action: Two new prescriptions arrive and reuse the empty slots via 'wrap-around'.\n");
    cq_enqueue(q, (Prescription){206, 0}); cq_enqueue(q, (Prescription){207, 0}); 
    cq_display(q); 
    printf("STATUS: The Rear pointer has wrapped around to index 1, demonstrating efficient space usage.\n");
    printf("--- [Demo Complete. Returning to menu.] ---\n");
}

void demoPriorityQueue(PriorityQueue* q) {
    printf("\n--- [Running Guided Demo for Priority Queue] ---\n");
    printf("Assumption: Lower number = higher priority (1=STAT).\n");
    printf("\n[SCENARIO 1: A STAT Order Arrives]\n");
    printf("Action: A standard prescription (#301, P:3) and an urgent one (#302, P:2) arrive.\n");
    pq_enqueue(q, (Prescription){301, 3}); pq_enqueue(q, (Prescription){302, 2});
    pq_display(q);
    pressEnterToContinue();
    printf("\nAction: A critical STAT prescription (#999, P:1) arrives for an emergency.\n");
    pq_enqueue(q, (Prescription){999, 1});
    pq_display(q); 
    pressEnterToContinue();
    printf("\nAction: The next prescription to be filled is the STAT order, jumping the line.\n");
    pq_dequeue(q);
    pq_display(q); 
    pressEnterToContinue();
    printf("\n[SCENARIO 2: Triage of Multiple Priorities]\n");
    printf("Action: The remaining prescriptions are filled strictly in order of priority.\n");
    while(!pq_isEmpty(q)) { pq_dequeue(q); }
    pq_display(q);
    printf("--- [Demo Complete. Returning to menu.] ---\n");
}

void demoDeque(Deque* d) {
    printf("\n--- [Running Guided Demo for Deque] ---\n");
    printf("\n[SCENARIO 1: Handling an Escalated Prescription]\n");
    printf("Action: A standard prescription (#401) is added to the back of the line.\n");
    dq_insertRear(d, (Prescription){401, 0});
    dq_display(d);
    pressEnterToContinue();
    printf("\nAction: An urgent, escalated prescription (#911) is added to the FRONT of the line.\n");
    dq_insertFront(d, (Prescription){911, 0});
    dq_display(d);
    pressEnterToContinue();
    printf("\nAction: The next prescription to be filled is the escalated one from the front.\n");
    dq_deleteFront(d);
    dq_display(d);
    pressEnterToContinue();
    printf("\n[SCENARIO 2: Last-In, First-Out for a special medication]\n");
    printf("Action: A technician prepares two parts of a compound (#501, #502), adding them to the rear.\n");
    dq_insertRear(d, (Prescription){501, 0}); dq_insertRear(d, (Prescription){502, 0});
    dq_display(d);
    pressEnterToContinue();
    printf("\nAction: The pharmacist needs the most recently added part first, so they take from the rear.\n");
    dq_deleteRear(d);
    dq_display(d);
    printf("--- [Demo Complete. Returning to menu.] ---\n");
}


void handleNormalQueue() {
    NormalQueue* q = createNormalQueue();
    int choice, id;
    while (1) {
        printf("\n--- Normal Queue Menu ---\n");
        printf("1. Add Prescription\n2. Fill Prescription\n3. Display Line\n4. Run Guided Demo\n5. Back to Main Menu\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: printf("  Enter Prescription ID: "); scanf("%d", &id); nq_enqueue(q, (Prescription){id, 0}); nq_display(q); break;
            case 2: nq_dequeue(q); nq_display(q); break;
            case 3: nq_display(q); break;
            case 4: demoNormalQueue(q); break;
            case 5: free(q); return;
            default: printf(" Invalid choice.\n");
        }
    }
}

void handleCircularQueue() {
    CircularQueue* q = createCircularQueue();
    int choice, id;
    while (1) {
        printf("\n--- Circular Queue Menu ---\n");
        printf("1. Add Prescription\n2. Fill Prescription\n3. Display Line\n4. Run Guided Demo\n5. Back to Main Menu\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: printf("  Enter Prescription ID: "); scanf("%d", &id); cq_enqueue(q, (Prescription){id, 0}); cq_display(q); break;
            case 2: cq_dequeue(q); cq_display(q); break;
            case 3: cq_display(q); break;
            case 4: demoCircularQueue(q); break;
            case 5: free(q); return;
            default: printf(" Invalid choice.\n");
        }
    }
}

void handlePriorityQueue() {
    PriorityQueue* q = createPriorityQueue();
    int choice, id, priority;
    while (1) {
        printf("\n--- Priority Queue Menu ---\n");
        printf("1. Add Prescription\n2. Fill Highest-Priority Rx\n3. Display Line\n4. Run Guided Demo\n5. Back to Main Menu\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("  Enter Prescription ID: "); scanf("%d", &id);
                printf("  Enter Priority (1=High, 5=Low): "); scanf("%d", &priority);
                if (priority < 1 || priority > 5) { printf(" Invalid priority.\n"); continue; }
                pq_enqueue(q, (Prescription){id, priority}); pq_display(q);
                break;
            case 2: pq_dequeue(q); pq_display(q); break;
            case 3: pq_display(q); break;
            case 4: demoPriorityQueue(q); break;
            case 5: free(q); return;
            default: printf(" Invalid choice.\n");
        }
    }
}

void handleDeque() {
    Deque* d = createDeque();
    int choice, id;
    while (1) {
        printf("\n--- Deque Menu (Special Handling Line) ---\n");
        printf("1. Add Rx to Front  2. Add Rx to Rear\n");
        printf("3. Fill Rx from Front 4. Fill Rx from Rear\n");
        printf("5. Display Line     6. Run Guided Demo\n");
        printf("7. Back to Main Menu\n> ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: printf("  Enter Rx ID: "); scanf("%d", &id); dq_insertFront(d, (Prescription){id, 0}); dq_display(d); break;
            case 2: printf("  Enter Rx ID: "); scanf("%d", &id); dq_insertRear(d, (Prescription){id, 0}); dq_display(d); break;
            case 3: dq_deleteFront(d); dq_display(d); break;
            case 4: dq_deleteRear(d); dq_display(d); break;
            case 5: dq_display(d); break;
            case 6: demoDeque(d); break;
            case 7: while(!dq_isEmpty(d)) { dq_deleteFront(d); } free(d); return;
            default: printf(" Invalid choice.\n");
        }
    }
}


int main() {
    int choice;
    while (1) {
        printf("\n===== Pharmacy Prescription Queue System =====\n");
        printf("Select the type of queue to manage:\n");
        printf("  1. Normal (Linear) Queue\n");
        printf("  2. Circular Queue\n");
        printf("  3. Priority Queue (STAT/Urgent Orders)\n");
        printf("  4. Double-Ended Queue (Deque)\n");
        printf("  5. Exit\n");
        printf("Your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); 
            printf(" Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1: handleNormalQueue(); break;
            case 2: handleCircularQueue(); break;
            case 3: handlePriorityQueue(); break;
            case 4: handleDeque(); break;
            case 5: printf("Exiting. \n"); return 0;
            default: printf(" Invalid selection. Please try again.\n");
        }
    }
    return 0;
}

