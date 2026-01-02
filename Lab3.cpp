#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Medicine {
    char name[100];
    int quantity;
    float price;
} Medicine;

typedef struct Node {
    int medicineID;
    Medicine data;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getInt() {
    int value;
    int result;
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
    } while (result != 1);
    return 0; 
}

void pressEnterToContinue() {
    printf("\n... Press Enter to continue ...\n");
    getchar();
}

void printNode(Node* node) {
    if (node == NULL) return;
    printf("[ID: %-5d | Name: %-20s | Qty: %-5d | Price: $%.2f]\n",
           node->medicineID, node->data.name, node->data.quantity, node->data.price);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(Node *n) {
    if (n == NULL)
        return 0;
    return n->height;
}

void updateHeight(Node *n) {
    if (n != NULL)
        n->height = 1 + max(height(n->left), height(n->right));
}

Node* createNode(int id, Medicine data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("!! Fatal Error: Memory allocation failed.\n");
        exit(1);
    }
    newNode->medicineID = id;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

void freeTree(Node* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

int getBalance(Node *n) {
    if (n == NULL)
        return 0;
    return height(n->left) - height(n->right);
}

Node* bst_insert(Node* root, int id, Medicine data) {
    if (root == NULL)
        return createNode(id, data);
    if (id < root->medicineID)
        root->left = bst_insert(root->left, id, data);
    else if (id > root->medicineID)
        root->right = bst_insert(root->right, id, data);
    return root;
}

Node* findMin(Node* root) {
    if (root == NULL) return NULL;
    while (root->left != NULL)
        root = root->left;
    return root;
}

Node* bst_delete(Node* root, int id) {
    if (root == NULL)
        return root;
    if (id < root->medicineID)
        root->left = bst_delete(root->left, id);
    else if (id > root->medicineID)
        root->right = bst_delete(root->right, id);
    else {
        if (root->left == NULL) {
            Node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Node *temp = root->left;
            free(root);
            return temp;
        }
        Node* temp = findMin(root->right);
        root->medicineID = temp->medicineID;
        root->data = temp->data;
        root->right = bst_delete(root->right, temp->medicineID);
    }
    return root;
}

Node* avl_insert(Node* root, int id, Medicine data) {
    if (root == NULL)
        return createNode(id, data);
    if (id < root->medicineID)
        root->left = avl_insert(root->left, id, data);
    else if (id > root->medicineID)
        root->right = avl_insert(root->right, id, data);
    else 
        return root;

    updateHeight(root);
    
    int balance = getBalance(root);

    if (balance > 1 && id < root->left->medicineID)
        return rightRotate(root);
    if (balance < -1 && id > root->right->medicineID)
        return leftRotate(root);
    if (balance > 1 && id > root->left->medicineID) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && id < root->right->medicineID) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    
    return root;
}

Node* avl_delete(Node* root, int id) {
    if (root == NULL)
        return root;
    if (id < root->medicineID)
        root->left = avl_delete(root->left, id);
    else if (id > root->medicineID)
        root->right = avl_delete(root->right, id);
    else {
        if (root->left == NULL || root->right == NULL) {
            Node *temp = root->left ? root->left : root->right;
            if (temp == NULL) { 
                temp = root;
                root = NULL;
            } else 
                *root = *temp; 
            free(temp);
        } else {
            Node* temp = findMin(root->right);
            root->medicineID = temp->medicineID;
            root->data = temp->data;
            root->right = avl_delete(root->right, temp->medicineID);
        }
    }
    
    if (root == NULL)
      return root;

    updateHeight(root);
    
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    
    return root;
}

Node* search(Node* root, int id) {
    if (root == NULL || root->medicineID == id)
        return root;
    if (id < root->medicineID)
        return search(root->left, id);
    else
        return search(root->right, id);
}

void morrisInorder(Node* root) {
    Node *current = root, *pre;
    if (root == NULL) {
        printf("!! Inventory is empty.\n");
        return;
    }

    while (current != NULL) {
        if (current->left == NULL) {
            printNode(current);
            current = current->right;
        } else {
            pre = current->left;
            while (pre->right != NULL && pre->right != current)
                pre = pre->right;

            if (pre->right == NULL) {
                pre->right = current;
                current = current->left;
            } else {
                pre->right = NULL;
                printNode(current);
                current = current->right;
            }
        }
    }
}

void morrisPreorder(Node* root) {
    Node *current = root, *pre;
    if (root == NULL) {
        printf("!! Inventory is empty.\n");
        return;
    }

    while (current != NULL) {
        if (current->left == NULL) {
            printNode(current);
            current = current->right;
        } else {
            pre = current->left;
            while (pre->right != NULL && pre->right != current)
                pre = pre->right;

            if (pre->right == NULL) {
                printNode(current); 
                pre->right = current;
                current = current->left;
            } else {
                pre->right = NULL;
                current = current->right;
            }
        }
    }
}

void reverseList(Node* node) {
    Node *prev = NULL, *curr = node, *next = NULL;
    while (curr != NULL) {
        next = curr->right;
        curr->right = prev;
        prev = curr;
        curr = next;
    }
}

void printReverse(Node* node) {
    Node* temp = node;
    while (temp != NULL) {
        printNode(temp);
        temp = temp->right;
    }
}

Node* findPredecessor(Node* current) {
    Node* pre = current->left;
    while (pre->right != NULL && pre->right != current) {
        pre = pre->right;
    }
    return pre;
}

void morrisPostorder(Node* root) {
    if (root == NULL) {
        printf("!! Inventory is empty.\n");
        return;
    }
    
    Node* dummy = (Node*)malloc(sizeof(Node));
    if (dummy == NULL) return;
    dummy->left = root;
    dummy->right = NULL;
    Node *current = dummy, *pre;

    while (current != NULL) {
        if (current->left == NULL) {
            current = current->right;
        } else {
            pre = findPredecessor(current);
            if (pre->right == NULL) {
                pre->right = current;
                current = current->left;
            } else {
                Node* start = current->left;
                reverseList(start);
                printReverse(pre);
                reverseList(pre); 
                
                pre->right = NULL;
                current = current->right;
            }
        }
    }
    free(dummy);
}


void displayMenu(Node* root) {
    int choice;
    while(1) {
        printf("\n  -- Display Menu (Morris Traversal) --\n");
        printf("  1. Inorder (Sorted by ID)\n");
        printf("  2. Preorder\n");
        printf("  3. Postorder\n");
        printf("  4. Back\n");
        choice = getInt();
        
        switch(choice) {
            case 1:
                printf("\n--- Morris Inorder Traversal ---\n");
                morrisInorder(root);
                break;
            case 2:
                printf("\n--- Morris Preorder Traversal ---\n");
                morrisPreorder(root);
                break;
            case 3:
                printf("\n--- Morris Postorder Traversal ---\n");
                morrisPostorder(root);
                break;
            case 4:
                return;
            default:
                printf("!! Invalid choice.\n");
        }
    }
}

Node* runGuidedDemo(Node* root, int isAVL) {
    printf("\n--- [Running Guided Demo] ---\n");
    printf("Action: Clearing any existing data...\n");
    freeTree(root);
    root = NULL;
    pressEnterToContinue();

    printf("[SCENARIO 1: Initial Stocking]\n");
    printf("Action: Inserting 5 medicines: (50, Paracetamol), (25, Ibuprofen), (75, Aspirin), (15, Amoxicillin), (30, Metformin)\n");
    Medicine m1 = {"Paracetamol", 100, 5.50};
    Medicine m2 = {"Ibuprofen", 50, 8.00};
    Medicine m3 = {"Aspirin", 75, 3.25};
    Medicine m4 = {"Amoxicillin", 120, 15.00};
    Medicine m5 = {"Metformin", 60, 12.75};

    if(isAVL) {
        root = avl_insert(root, 50, m1);
        root = avl_insert(root, 25, m2);
        root = avl_insert(root, 75, m3);
        root = avl_insert(root, 15, m4);
        root = avl_insert(root, 30, m5); 
    } else {
        root = bst_insert(root, 50, m1);
        root = bst_insert(root, 25, m2);
        root = bst_insert(root, 75, m3);
        root = bst_insert(root, 15, m4);
        root = bst_insert(root, 30, m5);
    }
    printf("Current Inventory (Inorder):\n");
    morrisInorder(root);
    pressEnterToContinue();

    printf("[SCENARIO 2: Searching Inventory]\n");
    printf("Action: Searching for Medicine ID 30 (Metformin)...\n");
    Node* found = search(root, 30);
    if(found) { printf("Result: Found! "); printNode(found); }
    else { printf("Result: Not Found.\n"); }

    printf("\nAction: Searching for Medicine ID 99 (Non-existent)...\n");
    found = search(root, 99);
    if(found) { printf("Result: Found! "); printNode(found); }
    else { printf("Result: Not Found.\n"); }
    pressEnterToContinue();

    printf("[SCENARIO 3: Deleting Items]\n");
    printf("Action: Deleting Medicine ID 15 (Amoxicillin) - a leaf node.\n");
    if(isAVL) { root = avl_delete(root, 15); }
    else { root = bst_delete(root, 15); }
    printf("Current Inventory (Inorder):\n");
    morrisInorder(root);
    pressEnterToContinue();

    printf("Action: Deleting Medicine ID 75 (Aspirin) - a leaf node.\n");
    if(isAVL) { root = avl_delete(root, 75); }
    else { root = bst_delete(root, 75); }
    printf("Current Inventory (Inorder):\n");
    morrisInorder(root);
    pressEnterToContinue();

    printf("Action: Deleting Medicine ID 50 (Paracetamol) - the root node with two children.\n");
    if(isAVL) { root = avl_delete(root, 50); } 
    else { root = bst_delete(root, 50); }
    printf("Current Inventory (Inorder):\n");
    morrisInorder(root);
    
    printf("\n--- [Demo Complete] ---\n");
    return root;
}

Node* loadSkewedData(Node* root, int isAVL) {
    printf("\n--- [Loading Skewed Data Demo] ---\n");
    printf("Action: Clearing any existing data...\n");
    freeTree(root);
    root = NULL;

    printf("Action: Inserting IDs 10, 20, 30, 40, 50 in ascending order.\n");
    int ids[] = {10, 20, 30, 40, 50};
    char names[][100] = {"Med-A", "Med-B", "Med-C", "Med-D", "Med-E"};
    
    for(int i=0; i<5; i++) {
        Medicine m = {"", 0, 0.0};
        strcpy(m.name, names[i]);
        m.quantity = 10 * (i+1);
        m.price = 5.0 * (i+1);
        
        if(isAVL) {
            root = avl_insert(root, ids[i], m);
        } else {
            root = bst_insert(root, ids[i], m);
        }
    }
    
    printf("-> Skewed data loaded.\n");
    if(isAVL) {
        printf("Notice: The AVL tree rebalanced itself during insertions.\n");
    } else {
        printf("Notice: The BST is now a degenerate tree (a linked list).\n");
    }
    printf("Use 'Display Inventory' (Preorder) to see the resulting structure.\n");
    return root;
}

void handleTree(Node** root, int isAVL) {
    int choice, id;
    Node* found;
    Medicine data;
    char treeType[4];
    strcpy(treeType, isAVL ? "AVL" : "BST");

    while (1) {
        printf("\n--- Pharmacy Management (%s Tree) ---\n", treeType);
        printf("1. Insert Medicine\n");
        printf("2. Delete Medicine\n");
        printf("3. Search Medicine\n");
        printf("4. Display Inventory\n");
        printf("5. Run Guided Demo\n");
        printf("6. Load Skewed Data (for Demo)\n");
        printf("7. Back to Main Menu\n");

        choice = getInt();

        switch (choice) {
            case 1:
                printf("  Enter Medicine ID (Key): ");
                id = getInt();
                found = search(*root, id);
                if (found != NULL) {
                    printf("!! Error: Medicine ID %d already exists.\n", id);
                    continue;
                }
                printf("  Enter Medicine Name: ");
                fgets(data.name, 100, stdin);
                data.name[strcspn(data.name, "\n")] = 0;
                printf("  Enter Stock Quantity: ");
                data.quantity = getInt();
                printf("  Enter Price: ");
                if (scanf("%f", &data.price) != 1) {
                    printf("!! Invalid input. Price set to 0.0\n");
                    data.price = 0.0;
                }
                clearInputBuffer();
                if (isAVL)
                    *root = avl_insert(*root, id, data);
                else
                    *root = bst_insert(*root, id, data);
                printf("-> Medicine ID %d inserted.\n", id);
                break;

            case 2:
                printf("  Enter Medicine ID to delete: ");
                id = getInt();
                found = search(*root, id);
                if (found == NULL) {
                    printf("!! Error: Medicine ID %d not found.\n", id);
                } else {
                    if (isAVL)
                        *root = avl_delete(*root, id);
                    else
                        *root = bst_delete(*root, id);
                    printf("-> Medicine ID %d deleted.\n", id);
                }
                break;

            case 3:
                printf("  Enter Medicine ID to search: ");
                id = getInt();
                found = search(*root, id);
                if (found == NULL) {
                    printf("!! Result: Medicine ID %d not found.\n", id);
                } else {
                    printf("== Result: Medicine Found ==\n");
                    printNode(found);
                }
                break;

            case 4:
                displayMenu(*root);
                break;
                
            case 5:
                *root = runGuidedDemo(*root, isAVL);
                break;

            case 6:
                *root = loadSkewedData(*root, isAVL);
                break;

            case 7:
                return;

            default:
                printf("!! Invalid choice. Please try again.\n");
        }
        
        if (choice != 4 && choice != 7) {
             pressEnterToContinue();
        }
    }
}

void compareComplexities() {
    printf("\n--- Time Complexity Analysis: BST vs. AVL ---\n\n");
    printf("N = Number of nodes in the tree. H = Height of the tree.\n\n");
    printf("=======================================================================\n");
    printf("| Operation |   Data Structure   |   Average Case   |    Worst Case    |\n");
    printf("=======================================================================\n");
    printf("| Search    | Binary Search Tree | O(log N)         | O(N)             |\n");
    printf("|           | AVL Tree           | O(log N)         | O(log N)         |\n");
    printf("-----------------------------------------------------------------------\n");
    printf("| Insertion | Binary Search Tree | O(log N)         | O(N)             |\n");
    printf("|           | AVL Tree           | O(log N)         | O(log N)         |\n");
    printf("-----------------------------------------------------------------------\n");
    printf("| Deletion  | Binary Search Tree | O(log N)         | O(N)             |\n");
    printf("|           | AVL Tree           | O(log N)         | O(log N)         |\n");
    printf("=======================================================================\n\n");
    
    printf("Key Takeaway:\n");
    printf(" * **BST (Worst Case):** When data is inserted in sorted order (like in the\n");
    printf("   'Load Skewed Data' demo), the BST degenerates into a linked list.\n");
    printf("   The height (H) becomes N, so operations become O(N), which is slow.\n\n");
    
    printf(" * **AVL (Worst Case):** The AVL tree performs rotations (like `leftRotate`,\n");
    printf("   `rightRotate`) during insertion and deletion to *guarantee* the tree\n");
    printf("   remains balanced. The height (H) is always kept at O(log N).\n");
    printf("   This ensures that all operations are *always* O(log N).\n");
    
    pressEnterToContinue();
}

int main() {
    Node* bstRoot = NULL;
    Node* avlRoot = NULL;
    int choice;

    while (1) {
        printf("\n===== BST & AVL Tree Pharmacy System =====\n");
        printf("1. Work with Binary Search Tree (BST)\n");
        printf("2. Work with AVL Tree\n");
        printf("3. Compare Time Complexities (BST vs AVL)\n"); 
        printf("4. Exit\n");                                 

        choice = getInt();

        switch (choice) {
            case 1:
                handleTree(&bstRoot, 0);
                break;
            case 2:
                handleTree(&avlRoot, 1);
                break;
            case 3: 
                compareComplexities();
                break;
            case 4: 
                printf("Exiting. Freeing all tree memory...\n");
                freeTree(bstRoot);
                freeTree(avlRoot);
                return 0;
            default:
                printf("!! Invalid selection. Please try again.\n");
        }
    }
    return 0;
}
