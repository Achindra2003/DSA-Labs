/**
 * @file lcs_genetics.c
 * @brief Genetic Sequence Alignment using LCS (Dynamic Programming)
 * @domain Pharmaceutical Research (Gene Therapy Targeting)
 * * SELF-LEARNING INITIATIVES:
 * 1. Similarity Index Calculation: Computes percentage match (Sorensen-Dice coefficient).
 * 2. Visual Trace Matrix: Displays the DP table to visualize the decision path.
 * 3. Alignment Visualization: Shows matches vs gaps in a readable format.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Max buffer for input (dynamic allocation used for actual processing)
#define INPUT_BUFFER 1024

// --- Result Structure ---
typedef struct {
    char* lcs_str;      // The resulting subsequence
    int length;         // Length of LCS
    double similarity;  // Calculated similarity percentage
    int** table;        // The DP Table (for visualization)
    int m;              // Length of seq1
    int n;              // Length of seq2
} LCSResult;

// --- Helper Functions ---

int max(int a, int b) {
    return (a > b) ? a : b;
}

// Validates input strings (rejects empty or null)
int validateInput(char* str) {
    if (str == NULL || strlen(str) == 0) return 0;
    // Optional: Check for valid DNA chars (A, C, G, T) could go here
    return 1;
}

// --- Core Algorithm (Dynamic Programming) ---

LCSResult* computeLCS(char* X, char* Y) {
    int m = strlen(X);
    int n = strlen(Y);

    LCSResult* result = (LCSResult*)malloc(sizeof(LCSResult));
    result->m = m;
    result->n = n;

    // 1. Dynamic Memory Allocation for DP Table
    // We need (m+1) x (n+1) matrix
    result->table = (int**)malloc((m + 1) * sizeof(int*));
    for (int i = 0; i <= m; i++) {
        result->table[i] = (int*)malloc((n + 1) * sizeof(int));
    }

    // 2. Build the DP Table (Bottom-Up)
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                result->table[i][j] = 0; // Base case: zero length strings
            else if (X[i - 1] == Y[j - 1])
                result->table[i][j] = result->table[i - 1][j - 1] + 1; // Match found
            else
                result->table[i][j] = max(result->table[i - 1][j], result->table[i][j - 1]); // Take max of adjacent
        }
    }

    result->length = result->table[m][n];

    // 3. Backtrack to find the actual LCS string
    int index = result->length;
    result->lcs_str = (char*)malloc((index + 1) * sizeof(char));
    result->lcs_str[index] = '\0'; // Null terminator

    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            // If characters match, they are part of LCS
            result->lcs_str[index - 1] = X[i - 1];
            i--; 
            j--; 
            index--;
        }
        else if (result->table[i - 1][j] > result->table[i][j - 1])
            i--; // Move up
        else
            j--; // Move left
    }

    // 4. SLI: Calculate Similarity Index (Sorensen-Dice)
    // Formula: (2 * LCS_Len) / (Len_X + Len_Y)
    result->similarity = (2.0 * result->length) / (m + n) * 100.0;

    return result;
}

// --- Output & Visualization Functions ---

void printDPTable(LCSResult* res, char* X, char* Y) {
    printf("\n--- Visual Trace: Dynamic Programming Matrix ---\n");
    printf("      "); // Spacing for header
    printf("0 "); // Null char column
    for (int k = 0; k < res->n; k++) printf("%c ", Y[k]);
    printf("\n");

    for (int i = 0; i <= res->m; i++) {
        if (i == 0) printf("0 ");
        else printf("%c ", X[i-1]);

        for (int j = 0; j <= res->n; j++) {
            printf("%d ", res->table[i][j]);
        }
        printf("\n");
    }
    printf("------------------------------------------------\n");
}

void printAnalysis(LCSResult* res, char* X, char* Y) {
    printf("\n=== Genetic Sequence Analysis Report ===\n");
    printf("Sequence 1 (Target):  %s\n", X);
    printf("Sequence 2 (Variant): %s\n", Y);
    printf("----------------------------------------\n");
    printf("LCS Length:           %d\n", res->length);
    printf("Common Subsequence:   %s\n", res->lcs_str);
    
    // SLI: Similarity Interpretation
    printf("Similarity Score:     %.2f%%\n", res->similarity);
    printf("Interpretation:       ");
    if (res->similarity > 80.0) printf("HIGH MATCH. Variant likely susceptible to target drug.\n");
    else if (res->similarity > 50.0) printf("MODERATE MATCH. Further testing required.\n");
    else printf("LOW MATCH. Drug likely ineffective on this variant.\n");
}

void freeLCSResult(LCSResult* res) {
    if (res) {
        if (res->table) {
            for (int i = 0; i <= res->m; i++) free(res->table[i]);
            free(res->table);
        }
        if (res->lcs_str) free(res->lcs_str);
        free(res);
    }
}

// --- Execution Modes ---

void runGuidedDemo() {
    printf("\n>>> Running Guided Demo (Gene Variant Comparison) <<<\n");
    // Example: Comparing a Vaccine Target Gene vs a Virus Variant Gene
    char* target = "GATTACA";
    char* variant = "GCATGCU";

    // Dynamic allocation as per requirement
    char* X = strdup(target);
    char* Y = strdup(variant);

    LCSResult* res = computeLCS(X, Y);
    printDPTable(res, X, Y);
    printAnalysis(res, X, Y);

    freeLCSResult(res);
    free(X);
    free(Y);
}

void runDynamicInput() {
    char buffer[INPUT_BUFFER];
    
    printf("\n>>> Running Dynamic Input Mode <<<\n");
    
    // Input 1
    printf("Enter Sequence 1 (e.g., DNA/Chemical formula): ");
    scanf("%s", buffer);
    if (!validateInput(buffer)) { printf("Invalid input.\n"); return; }
    char* X = strdup(buffer); // Allocate exact memory

    // Input 2
    printf("Enter Sequence 2: ");
    scanf("%s", buffer);
    if (!validateInput(buffer)) { printf("Invalid input.\n"); free(X); return; }
    char* Y = strdup(buffer);

    LCSResult* res = computeLCS(X, Y);
    
    // Ask user if they want the verbose trace
    int verbose = 0;
    printf("Display DP Trace Matrix? (1=Yes, 0=No): ");
    scanf("%d", &verbose);

    if (verbose) printDPTable(res, X, Y);
    printAnalysis(res, X, Y);

    freeLCSResult(res);
    free(X);
    free(Y);
}

// --- Main ---

int main() {
    int choice = 0;
    while(choice != 3) {
        printf("\n============================================\n");
        printf("   PHARMA-GENETIC SEQUENCE ALIGNMENT (LCS)  \n");
        printf("============================================\n");
        printf("1. Run Guided Demo\n");
        printf("2. Compare Custom Sequences\n");
        printf("3. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            choice = 0;
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
