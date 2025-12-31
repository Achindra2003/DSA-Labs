#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int batchID;
    int expiryDate;
} MedicineBatch;

typedef struct Node {
    MedicineBatch data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    MedicineBatch min;
} MinStack;

MinStack* createMinStack() {
    MinStack* ms = (MinStack*)malloc(sizeof(MinStack));
    ms->head = NULL;
    ms->min = (MedicineBatch){-1, INT_MAX};
    return ms;
}

int isEmpty(MinStack* ms) {
    return ms->head == NULL;
}

void push(MinStack* ms, MedicineBatch batch) {
    if (batch.expiryDate <= ms->min.expiryDate) {
        Node* oldMinNode = (Node*)malloc(sizeof(Node));
        oldMinNode->data = ms->min;
        oldMinNode->next = ms->head;
        ms->head = oldMinNode;
        ms->min = batch;
    }
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = batch;
    newNode->next = ms->head;
    ms->head = newNode;
    printf("-> Pushed Batch ID %d (Expires: %d)\n", batch.batchID, batch.expiryDate);
}

void pop(MinStack* ms) {
    if (isEmpty(ms)) {
        printf(" ERROR: Inventory is empty. Cannot pop.\n");
        return;
    }
    Node* temp = ms->head;
    MedicineBatch poppedBatch = temp->data;
    ms->head = ms->head->next;
    free(temp);
    printf("<- Popped Batch ID %d (Expires: %d)\n", poppedBatch.batchID, poppedBatch.expiryDate);

    if (poppedBatch.batchID == ms->min.batchID && poppedBatch.expiryDate == ms->min.expiryDate) {
        if (!isEmpty(ms)) {
            Node* oldMinNode = ms->head;
            ms->min = oldMinNode->data;
            ms->head = ms->head->next;
            free(oldMinNode);
        } else {
            ms->min = (MedicineBatch){-1, INT_MAX};
        }
    }
}

MedicineBatch top(MinStack* ms) {
    if (isEmpty(ms)) {
        return (MedicineBatch){-1, -1};
    }
    return ms->head->data;
}

MedicineBatch getMin(MinStack* ms) {
    if (isEmpty(ms) || ms->min.batchID == -1) {
        return (MedicineBatch){-1, -1};
    }
    return ms->min;
}

void printStatus(MinStack* ms) {
    if (isEmpty(ms)) {
        printf("\n   (Inventory is now empty.)\n");
        return;
    }
    printf("\n   --- Current Inventory Status ---\n");
    MedicineBatch topBatch = top(ms);
    MedicineBatch minBatch = getMin(ms);

    if (topBatch.batchID != -1) {
        printf("   Current Top Batch -> ID: %d, Expires: %d\n", topBatch.batchID, topBatch.expiryDate);
    }
    if (minBatch.batchID != -1) {
        printf("   Earliest Expiry   -> ID: %d, Expires: %d\n", minBatch.batchID, minBatch.expiryDate);
    }
    printf("   ------------------------------\n");
}

int main() {
    MinStack* inventory = createMinStack();
    int choice;
    MedicineBatch batch;

    printf("===== Pharmacy Inventory Min-Stack System =====\n");
    printf("Domain: Pharmacy Management\n");
    printf("Stack Element: Medicine Batch (ID, Expiry Date)\n");
    printf("--------------------------------------------------\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Push a new medicine batch\n");
        printf("2. Pop the last batch\n");
        printf("3. View the top batch\n");
        printf("4. View batch with earliest expiry (getMin)\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\n Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter Batch ID: ");
                scanf("%d", &batch.batchID);
                printf("Enter Expiry Date (YYYYMMDD): ");
                scanf("%d", &batch.expiryDate);
                push(inventory, batch);
                printStatus(inventory);
                break;

            case 2:
                pop(inventory);
                printStatus(inventory);
                break;

            case 3:
                if (!isEmpty(inventory)) {
                    batch = top(inventory);
                    printf("\nTop Batch -> ID: %d, Expires: %d\n", batch.batchID, batch.expiryDate);
                } else {
                    printf("\n Inventory is empty.\n");
                }
                break;

            case 4:
                if (!isEmpty(inventory)) {
                    batch = getMin(inventory);
                    printf("\nEarliest Expiry -> ID: %d, Expires: %d\n", batch.batchID, batch.expiryDate);
                } else {
                     printf("\n Inventory is empty.\n");
                }
                break;

            case 5:
                printf("\nExiting program.\n");
                while(!isEmpty(inventory)) {
                    pop(inventory);
                }
                free(inventory);
                return 0;

            default:
                printf("\n Invalid choice. Please enter a number between 1 and 5.\n");
        }
    }
}

