/*
    AUTHOR: Denis Stocki
    PROJECT: 15 puzzle solving programme, using two different heuristics
*/

/*
    DEFAULT LIBRARIES IMPORT
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

/*
    CONSTANTS DEFINITION
*/
#define SIZE 3
#define true 1
#define false 0

/*
    GLOBAL VARIABLES
*/
int printStep;

/*
    PUZZLE DATA TYPE CREATION - HOLDS PARAMETERS OF CURRENT STATE
*/
typedef struct State{
    unsigned long long hashCode;
    int totalCost;
    int stepCost;
    struct State *parent;
} State;

/*
    SINGLE NODE DATA TYPE CREATION
*/
typedef struct Node{
    State *state;
    struct Node *left;
    struct Node *right;
} Node;

/*
    HEAP DATA TYPE CREATION
*/
typedef struct Heap{
    Node *head;
    int size;
} Heap;

/*
    PROGRAMME'S FUNCTIONS
*/
void prepareDefault(State *state); 
void printPuzzle(State *state); 
int isSolvableHash(unsigned long long hashCode); 
State *solvePuzzle(State *startingState);
Heap *initializeHeap(); 
void swapStates(Node *node1, Node *node2);
Node *initializeNode(State *state);
State *prepareState(unsigned long long hashCode, int totalCost, int stepCost); 
void addToHeap(Heap *heap, State *state); 
// void enlargeArrayList(ArrayList *list); // MOZNA PRZYSPIESZYC
// State *removeFromList(ArrayList *list, State *state); // MOZNA PRZYSPIESZYC
// State *reduceAndHandleRemoval(ArrayList *list, State *state); // MOZNA PRZYSPIESZYC
// State *handleRemoval(ArrayList *list, State *state); // MOZNA PRZYSPIESZYC
int isLegalMove(State *state, int *move); 
// int listContains(ArrayList *list, State *state); // MOZNA PRZYSPIESZYC
// State *findMinimumInList(ArrayList *list); // MOZNA PRZYSPIESZYC
unsigned long long swapInsideHash(unsigned long long hashCode, int pos1, int pos2); 
unsigned long long modifyHashOf(unsigned long long hashCode, int *move); 
void addToHeapRec(Node **nodePtr, State *state, int heapSize);
unsigned long long createGoalHashCode(); 
int getFromHash(unsigned long long hashCode, int pos); 
int getZeroPos(unsigned long long hashCode); 
// void printHeap(ArrayList *list, int k);
int manhattanHeuristicOf(unsigned long long hash); 
// State* findInList(ArrayList *list, unsigned long long hashCode); // MOZNA PRZYSPIESZYC
void printLine(); 
void printState(State *state);
// void printList(ArrayList *list);
void reconstructPath(State *finishState);

void printBinary(unsigned long long n) {
    for (int i = 63; i >= 0; i--) {
        if ((n >> i) & 1) {
            printf("1");
        } else {
            printf("0");
        }
        if(i % 4 == 0) printf(" ");
    }
    printf("\n");
}

void printHeap(Node *heap, int level){
    
    if (heap == NULL) {
        return;
    }
    printHeap(heap->right, level + 1);
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d\n", heap->state->totalCost);
    printHeap(heap->left, level + 1);
    
}

/*
    MAIN STARTING FUNCTION
*/
int main(){
    Heap *heap = initializeHeap();
    State *state = prepareState(0x123456780, 5, 5);
    State *state1 = prepareState(0x123456780, 6, 6);
    State *state2 = prepareState(0x123456780, 3, 3);
    State *state3 = prepareState(0x123456780, 4, 4);
    addToHeap(heap, state);
    addToHeap(heap, state1);
    addToHeap(heap, state2);
    addToHeap(heap, state3);
    addToHeap(heap, state1);
    addToHeap(heap, state1);
    addToHeap(heap, state2);
    addToHeap(heap, state3);
    addToHeap(heap, state3);
    addToHeap(heap, state3);
    printHeap(heap -> head, 0);
    // State *state = malloc(sizeof(State));
    // prepareState(state, 0x123456780, 0, 0, NULL);
    // addToHeap(heap, state);
    // printHeap(heap -> head);

    // prepareDefault(startingState);
    // reconstructPath(solvePuzzle(startingState));

    free(heap);
    
    return 0;
}

/*
    FUNCTION RECONSTRUCTPATH
    ------------------------
    THIS FUNCTION RECONSTRUCTS PATH FROM A GIVEN STATE.
*/
void reconstructPath(State *finishState){
    if ((finishState -> parent) == NULL){
        printPuzzle(finishState);
    } else {
        reconstructPath(finishState -> parent);
        printPuzzle(finishState);
    }
    // State *currentState = finishState;
    // while (currentState != NULL) {
    //     printPuzzle(currentState);
    //     currentState = currentState->parent;
    // }

}

/*
    FUNCTION MANHATTANHEURISTICOF
    -----------------------------
    THIS FUNCTION COUNTS HEURISTIC SCORE OF A GIVEN HASH. IT COUNTS
    EXACTLY MANHATTAN DISTANCE FOR EACH NUMBER TO BE PLACED IN THE CORRECT
    ORDER.
*/
int manhattanHeuristicOf(unsigned long long hashCode){
    int sum = 0;

    for (int i = 0; i < SIZE * SIZE - 1; i++){
        int number = ((hashCode >> (4 * (SIZE * SIZE - i - 1)) & 0xF));
        sum += abs((number - 1) / SIZE - i / SIZE) + abs((number - 1) % SIZE - i % SIZE);    
    }
    
    return sum;
}

/*
    FUNCTION PREPAREDEFAULT
    ----------------------
    THIS FUNCTION BEGINS FROM GENERATING SOLVED POSITION IN THE PUZZLE AND 
    THEN RANDOMIZES IT USING FISHER-YATES-SHUFFLE ALGORITHM
*/
void prepareDefault(State *state){
    unsigned long long hashCode = createGoalHashCode();

    do{
        printf("Generating puzzle...\n\n");
        for (int i = SIZE * SIZE - 2; i > 0; i--){
            int j = rand() % (i + 1);
            hashCode = swapInsideHash(hashCode, j, i);
        }
    } while (isSolvableHash(hashCode) == false);

    state = prepareState(hashCode, manhattanHeuristicOf(hashCode), 0);
}

/*
    FUNCTION SWAPINSIDEHASH
    -----------------------
    THIS FUNCTION SWAPS VALUES ON THE GIVEN INDEXES IN THE GIVEN HASH.
*/
unsigned long long swapInsideHash(unsigned long long hashCode, int pos1, int pos2){
    unsigned long long temp = (hashCode >> (4 * (SIZE * SIZE - 1 - pos2))) & 0xF;

    hashCode &= ~((unsigned long long) 0xF << (4 * (SIZE * SIZE - 1 - pos2)));
    hashCode |= ((hashCode >> (4 * (SIZE * SIZE - 1 - pos1))) & 0xF) << (4 * (SIZE * SIZE - 1 - pos2));
    hashCode &= ~((unsigned long long) 0xF << (4 * (SIZE * SIZE - 1 - pos1)));
    hashCode |= temp << (4* (SIZE * SIZE - 1 - pos1));

    return hashCode;
}

/*
    FUNCTION PREPAREPUZZLE
    ----------------------
    THIS FUNCTION SETS DEFAULT VALUES TO A GIVEN PUZZLE.
*/
State *prepareState(unsigned long long hashCode, int totalCost, int stepCost){
    State *state = (State*)malloc(sizeof(State));

    state -> hashCode = hashCode;
    state -> totalCost = totalCost;
    state -> stepCost = stepCost;
    state -> parent = NULL;

    return state;
}

/*
    FUNCTION CREATEGOALHASH
    -----------------------
    THIS FUNCTION CREATES A HASH THAT IS EXACTLY A UNIQUE CODE OF A 
    GOAL BOARD. THE FUNCTION CAN OBTAIN A GOALHASH FOR BOARD SIZES LIKE
    1x1, 2x2, 3x3, 4x4
*/
unsigned long long createGoalHashCode(){

    if (SIZE > 4){
        fprintf(stderr, "This programme works only for variable SIZE lower or equal to 4...\n");
        return -1;
    }
    
    unsigned long long goalHashCode = 0;

    for (int i = 1; i <= SIZE * SIZE; i++){
        goalHashCode <<= 4;
        goalHashCode |= ((i % (SIZE * SIZE)) & 0xF);
    }

    return goalHashCode;
}

/*
    FUNCTION GETFROMHASH
    --------------------
    THIS FUNCTION RETURNS A VALUE FROM GIVEN HASH ON A GIVEN POSITION.
*/
int getFromHash(unsigned long long hashCode, int pos){
    hashCode >>= 4 * (SIZE * SIZE - pos - 1);
    return hashCode & 0xF;
}

/*
    FUNCTION ISSOLVABLEPUZZLE
    -------------------------
    THIS FUNCTION CHECKS WHETHER THE PUZZLE IS SOLVABLE OR NOT, THIS MEANS
    IT RETURNS TRUE IF THE NUMBER OF INVERSIONS IN THE PUZZLE IS EVEN
*/
int isSolvableHash(unsigned long long hashCode){
    int counter = 0;
    
    for (int i = 0; i < SIZE * SIZE - 2; i++){
        for (int j = i + 1; j < SIZE * SIZE - 1; j++){
            if (getFromHash(hashCode, i) > getFromHash(hashCode, j)){
                counter++;
            }
        }
    }

    if (counter % 2 == 0){
        printf("The puzzle is solvable, let us begin solving!\n\n");
        return true;
    } else {
        printf("The puzzle is not solvable, another try...\n\n");
        return false;
    }
}

/*
    FUNCTION PRINTPUZZLE
    --------------------
    THIS FUNCTION PRINTS GIVEN PUZZLE IN A SIZE X SIZE SQUARE
*/
void printPuzzle(State *state){
    printf("STEP: %d\n\n", printStep++);
    printLine();
    
    for (int i = 0; i < SIZE; i++){
        printf("|");
        for (int j = 0; j < SIZE; j++){
            printf("%*d |", 3, getFromHash(state -> hashCode, i * SIZE + j));   
        }
        printf("\n");
        printLine();
    }
    printf("\n");

    sleep(1);
}

/*
    FUNCTION PRINTLINE
    ------------------
    THIS FUNCTION PRINTS A LINE OF 4 * SIZE * (SIZE - 1) LENGTH.
*/
void printLine(){
    for (int i = 0; i < 5 * SIZE + 1; i++){
        printf("-");
    }
    printf("\n");
}

/*
    FUNCTION SOLVEPUZZLE
    --------------------
    THIS FUNCTION SOLVES THE PUZZLE AND PRINTS FOLLOWING STATES OF THE PUZZLE.
    SOLVING METHOD IS A* ALGORITHM WHICH IS USING ONE OF THE GIVEN HEURISTICS
    IN THE PROGRAMME.
*/
// State *solvePuzzle(State *startingState){
//     ArrayList *explored = malloc(sizeof(ArrayList));
//     ArrayList *heap = malloc(sizeof(ArrayList));

//     initializeArrayList(explored, 0, 10);
//     initializeArrayList(heap, 0, 10);

//     int moves[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};

//     State *goalState = malloc(sizeof(State));

//     prepareState(goalState, createGoalHashCode(), 0, 0, NULL);
//     addToList(heap, startingState);

//     while (heap -> count != 0){
//         // printf("\nHEAP\n");
//         // printList(heap);
//         // printf("\nEXPLORED\n");
//         // printList(explored);
//         State *currentState = findMinimumInList(heap);
//         // printf("\nMINIMUM STATE\n");
//         // printState(currentState);

//         if (currentState -> hashCode == goalState -> hashCode){
//             printf("PUZZLE HAS BEEN SOLVED !\n\n");
//             return currentState;
//         }

//         // printPuzzle(currentState);
//         removeFromList(heap, currentState);

//         for (int i = 0; i < 4; i++){
//             if (isLegalMove(currentState, moves[i]) == true){
//                 State *neighState = malloc(sizeof(State));
//                 unsigned long long neighHashCode = modifyHashOf(currentState -> hashCode, moves[i]);
//                 int newStepCost = currentState -> stepCost + 1;

//                 prepareState(neighState, neighHashCode, manhattanHeuristicOf(neighHashCode), newStepCost, currentState);
//                 // printf("NEIGHBOUR OF MOVE: %d, %d\n", moves[i][0], moves[i][1]);
//                 // printState(neighState);

//                 if (listContains(heap, neighState) == true){
//                     if (findInList(heap, neighState -> hashCode) -> stepCost <= newStepCost){
//                         continue;
//                     } else {
//                         addToList(heap, neighState);
//                     }
//                 } else if (listContains(explored, neighState) == true){
//                     if (findInList(explored, neighState -> hashCode) -> stepCost <= newStepCost){
//                         continue;
//                     } else {
//                         removeFromList(explored, neighState);
//                         addToList(heap, neighState);
//                     }
//                 } else {
//                     addToList(heap, neighState);
//                 }
//             }
//         }

//         addToList(explored, currentState);
//     }
//     free(heap);
//     free(explored);
//     free(goalState);
//     printf("SIEMA\n");
//     return NULL;
// }

// void printList(ArrayList *list){
//     for (int i = 0; i < list -> count; i++){
//         printState(list -> array[i]);
//     }
//     printf("LIST SIZE: %d\n", list -> count);
//     printf("LIST MAX SIZE: %d\n", list -> maxCount);
// }

// void printState(State *state){
//     printf("STATE PRINTING\n");
//     printf("STATE HASH: ");
//     printBinary(state -> hashCode);
//     printf("STEP COST: %d\n", state -> stepCost);
//     printf("HEURISTIC COST: %d\n", state -> heuristicCost);
// }

/*
    FUNCTION FINDINLIST
    -------------------
    THIS FUNCTION RETURNS AN STATE STRUCTURE IF FOUND IN A GIVEN ARRAYLIST
    OR NULL.
*/
// State *findInList(ArrayList *list, unsigned long long hashCode){

//     for (int i = 0; i < list -> count; i++){
//         if(list -> array[i] -> hashCode == hashCode){
//             return list -> array[i];
//         }
//     }
    
//     return NULL;
// }

// void printHeap(ArrayList *list, int k){
//     if(k == 0) printf("HEAP\n");
//     if(k == 1) printf("EXPLORED\n");
//     for (int i = 0; i < list -> count; i++){
//         printf("ELEMENT: ");
//         printBinary(list -> array[i] -> hash);
//         printf("COST: %d, STEPCOST: %d\n", list->array[i]->cost, list->array[i]->stepCost);
//     }
// }

/*
    FUNCTION MODIFYHASHOF
    ---------------------
    THIS FUNCTION CREATES A NEW HASH THAT IS A CONTINUATION OF A CURRENT
    PUZZLE AFTER THE NEXT MOVE.
*/
unsigned long long modifyHashOf(unsigned long long hashCode, int *move){
    int pos = getZeroPos(hashCode);

    if (move[0] != 0){
        hashCode = swapInsideHash(hashCode, pos, pos + move[0]);
    } else {
        hashCode = swapInsideHash(hashCode, pos, pos + SIZE * move[1]);
    }

    return hashCode;
}

/*
    FUNCTION GETZEROPOS
    -------------------
    THIS FUNCTION RETURNS POSITION OF ZERO IN A GIVEN HASH.
*/
int getZeroPos(unsigned long long hashCode){
    int i = 0;

    while ((hashCode & 0xF) != 0 && i < SIZE * SIZE) {
        hashCode >>= 4;
        i++;
    }

    if (i == SIZE * SIZE){
        fprintf(stderr, "Given hashCode does not include 0 value...\n");
    }

    return SIZE * SIZE - 1 - i;
}

/*
    FUNCTION FINDMINIMUMIN
    ----------------------
    THIS FUNCTION FINDS A MINIMUM COST VALUE IN WHOLE GIVEN ARRAYLIST.
*/
// State* findMinimumInList(ArrayList *list){
//     State *minimumState = list -> array[0];
//     int minimumCost = minimumState -> stepCost + minimumState -> heuristicCost;

//     for (int i = 1; i < list -> count; i++){
//         if (minimumCost > list -> array[i] -> stepCost + list -> array[i] -> heuristicCost){
//             minimumState = list -> array[i];
//             minimumCost = list -> array[i] -> stepCost + list -> array[i] -> heuristicCost;
//         }
//     }

//     return minimumState;
// }

/*
    FUNCTION LISTCONTAINS
    ---------------------
    THIS FUNCTION CHECKS WHETHER THERE IS A PUZZLE IN A GIVEN LIST
    WITH A GIVEN STATE.
*/
// int listContains(ArrayList *list, State *state){
//     for (int i = 0; i < list -> count; i++){
//         if (list -> array[i] -> hashCode == state -> hashCode){
//             return true;
//         }
//     }

//     return false;
// }

/*
    FUNCTION ADDTOHEAP
    ------------------
    THIS FUNCTION ADDS A PUZZLE TO A GIVEN HEAP.
*/
void addToHeap(Heap *heap, State *state){
    int heapSize = (heap -> size)++;

    // if (heapSize == 0) {
    //     heap->head = initializeNode(state);
    // } else {
    //     addToHeapRec(&heap->head, state, heapSize - 1);
    // }
    addToHeapRec(&heap->head, state, heapSize);
}

/*
    FUNCTION ADDTOHEAPREC
    ---------------------
    THIS FUNCTION ADDS NODE TO ITS CHILDREN RECURSIVELY.
*/
void addToHeapRec(Node **nodePtr, State *state, int heapSize){
    Node *node = *nodePtr;

    if (heapSize == 0){
        *nodePtr = initializeNode(state);
    } else if ((heapSize - 1) % 2 == 0){
        addToHeapRec(&node -> left, state, (heapSize - 1) / 2);
        if (node -> state -> totalCost > node -> left -> state -> totalCost){
            swapStates(node, node -> left);
        }
    } else {
        addToHeapRec(&node -> right, state, (heapSize - 1) / 2);
        if (node -> state -> totalCost > node -> right -> state -> totalCost){
            swapStates(node, node -> right);
        }
    }
}

/*
    FUNCTION SWAPSTATES
    -------------------
    THIS FUNCTION SWAPS STATES OF CURRENT NODE AND ITS CHILD ON A GIVEN POSITION.
*/
void swapStates(Node *node1, Node *node2){
    State *temp = node1 -> state;
    node1 -> state = node2 -> state;
    node2 -> state = temp;
}

/*
    FUNCTION REMOVEFROMLIST
    -----------------------
    THIS FUNCTION REMOVES A GIVEN ELEMENT FROM LIST. IF NECESARRY, THE FUNCTION REDUCES 
    SIZE OF A GIVEN ARRAYLIST BY 33 PERCENT.
*/
// State *removeFromList(ArrayList *list, State *state){
//     State *score;

//     if (list -> count < list -> maxCount / 2 && list -> maxCount > 10){
//         score = reduceAndHandleRemoval(list, state);
//     } else {
//         score = handleRemoval(list, state);   
//     }

//     list -> count--;

//     return score;
// }

/*
    FUNCTION HANDLEREMOVAL
    ----------------------
    THIS FUNCTION DELETES A GIVEN PUZZLE FROM A GIVEN ARRAYLIST.
*/
// State *handleRemoval(ArrayList *list, State *state){
//     int gap = 0;
//     State *score;

//     for (int i = 0; i < list -> count - 1; i++){
//         if (list -> array[i] -> hashCode == state -> hashCode){
//             score = list -> array[i];
//             gap++;        
//         }
//         list -> array[i] = list -> array[i + gap];
//     }

//     return score;
// }   

/*
    FUNCTION REDUCEANDHANDLEREMOVAL
    -------------------------------
    THIS FUNCTION REDUCES AN ARRAYLIST BY 33 PERCENT AND DELETES A GIVEN PUZZLE
    AT ONCE.
*/
// State *reduceAndHandleRemoval(ArrayList *list, State *state){
//     int newMaxCount = (int)(list -> maxCount * 2 / 3);
//     ArrayList *newArrayList = malloc(sizeof(ArrayList));
//     State *score;

//     initializeArrayList(newArrayList, list -> count, newMaxCount);

//     int gap = 0;

//     for (int i = 0; i < list -> count - 1; i++){
//         if (list -> array[i] -> hashCode == state -> hashCode){
//             score = list -> array[i];
//             gap++;        
//         }
//         newArrayList -> array[i] = list -> array[i + gap];
//     }

//     list = newArrayList;

//     return score;
// }

/*
    FUNCTION ENLARGEARRAYLIST
    -------------------------
    THIS FUNCTION ENLARGES A GIVEN ARRAYLIST BY 50 PERCENT AND REWRITES
    OLD DATA INTO A NEW LIST.
*/
// void enlargeArrayList(ArrayList *list){
//     int newMaxCount = (int)(list -> maxCount * 1.5);
//     ArrayList *newArrayList = malloc(sizeof(ArrayList));

//     initializeArrayList(newArrayList, list -> count, newMaxCount);

//     for (int i = 0; i < list -> count; i++){
//         newArrayList -> array[i] = list -> array[i]; 
//     }

//     list = newArrayList;
// }

/*
    FUNCTION ISLEGALMOVE
    --------------------
    THIS FUNCTION CHECKS WHETHER THE MOVE IS LEGAL OR NOT. THIS MEANS THAT
    IS CHECKS IF THE MOVE DOES NOT GO OVER THE BOARD.
*/
int isLegalMove(State *state, int *move){
    int pos = getZeroPos(state -> hashCode);

    if (pos % SIZE + move[0] >= 0 && pos % SIZE + move[0] < SIZE){
        if (pos / SIZE + move[1] >= 0 && pos / SIZE + move[1] < SIZE){
            return true;
        }   
    }

    return false;
}

/*
    FUNCTION INITIALIZEHEAP
    ----------------------------
    THIS FUNCTION IS USED TO INITIALIZE HEAP WITH STARTING VALUES.
*/
Heap *initializeHeap(){
    Heap *heap = (Heap*)malloc(sizeof(Heap));

    heap -> head = NULL;
    heap -> size = 0;

    return heap;
}

/*
    FUNCTION INITIALIZENODE
    -----------------------
    THIS FUNCTIONS INITIALIZES NODE WITH GIVEN VALUES OF STATE, LEFTCHILD
    AND RIGHTCHILD AND RETURNS IT.
*/
Node *initializeNode(State *state){
    Node *node = (Node*)malloc(sizeof(Node));

    node -> state = state;
    node -> left = NULL;
    node -> right = NULL;
    
    return node;
}