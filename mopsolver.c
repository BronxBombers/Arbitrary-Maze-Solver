/**
 * Title: Mopsolver Program
 * Description: Reads in a maze and finds the shortest path from beginning
 * to end of that maze.
 * @author Zach Morgan
 * RIT ID: zdm9127
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>


//----------------MAZE REPRESENTATION STRUCTS---------------------------

/**
 * Struct Representing the coordinates of a Vertex
 */
    struct Coords {
        int y;
        int x;
        int listIndex;
    };

    typedef struct Coords * Coords;

/**
 * Struct Representing a Vertex in Maze
 */
    struct vertex{
        int nborCount;
        int dist;
        Coords coords;
        Coords * nCoords;
        struct vertex * previous;
    };

    typedef struct vertex * Vertex;

/**
 * Struct Used for creating the Queue used in the Breath First Search
 */
    struct VertexQNode{
        Vertex vert;
        struct VertexQNode * next;
    }VertexQNode;

    typedef struct VertexQNode * VNode;

/**
 * Compare Function used in ordering the queue for the search algorithm
 */
    typedef int (*cmp)(Vertex a, Vertex b);

/**
 * Struct Representing the Queue used in the searching algorithm
 */
    struct VertexQ{
        int size;
        VNode VNodeList;
        cmp funct;
    }VertexQ;

    typedef struct VertexQ * Que;

//----------------END MAZE REPRESENTATION STRUCTS---------------------------


//----------------FILE READ STRUCTS-----------------------------------------

/**
 * Struct Representing a Node of data read in from the file
 */
    struct Node{
        int x;
        int y;
        struct Node * next;
    }Node;

    typedef struct Node * ListNode;

/**
 * Struct Representing the linked list of the data read in from the file
 */
    typedef struct FileDataLinkedList{
        ListNode data;
        int xSize;
        int ySize;
        int totalVertices;
    }FileDataLinkedList;

//----------------END FILE READING STRUCTS----------------------------------


//----------------QUEUE-----------------------------------------------------

/**
 * Determines where in the priority queue a vertex should be placed
 * @param two vertex being inserted in the BFS queue
 * @return +number if a > b, 0 is a==b or -number if a < b
 */
int compareFunct(Vertex a, Vertex b){
    return a->dist - b->dist;
}

/**
 * Places the Vertex at the correct spot in the BFS queue
 * @params queue: BFS Queue, vert: Vertex to be inserted
 */
bool insertList(Que queue, Vertex vert){
    VNode newNode = malloc(sizeof(struct VertexQNode));
    bool beforeFirstNode = true;
    VNode currNode = queue->VNodeList;
    VNode previousNode;
    if (newNode == NULL){
        return false;
    }
    while (currNode != NULL && queue->funct(vert, currNode->vert) > 0){
        previousNode = currNode;
        currNode = currNode->next;
        beforeFirstNode = false;
    }
    if (beforeFirstNode) {
        newNode->next = currNode;
        queue->VNodeList = newNode;
    }
    else{
        newNode->next = previousNode->next;
        previousNode->next = newNode;
    }
    newNode->vert = vert;
    return true;
}


/** Create a QueueADT which uses the supplied function as a comparison
 * routine.
 *	Result	Relationship
 *	======	===========
 *	 < 0	a < b
 *	 = 0	a == b
 *	 > 0	a > b
 *
 *
 * @return a QueueADT instance, or NULL if the allocation fails
 */
Que que_create(){
    Que instance = malloc(sizeof(struct VertexQ));
    if (instance == NULL){
        return NULL;
    }
    instance->funct = &compareFunct;
    instance->size = 0;
    return instance;
}

void freeVertex(Vertex v){
    free(v->nCoords);
    free(v->coords);
    free(v);
}


/**
 * Helper function that iterates through the linked list containing
 * the Queue's contents and frees the memory allocated for each
 * @param queue : the queue to free the contents from
 */
void freeContentsOfList(Vertex * list, int size){
    for (int i = 0; i < size; i++){
        freeVertex(list[i]);
    }
}

/** Insert the specified data into the Queue in the appropriate place
 *
 * Uses the queue's comparison function to determine the appropriate
 * place for the insertion.
 *
 * @param queue the QueueADT into which the value is to be inserted
 * @param data the data to be inserted
 * @exception If the value cannot be inserted into the queue for
 *     whatever reason (usually implementation-dependent), the program
 *      should terminate with an error message.  This can be done by
 *      printing an appropriate message to the standard error output and
 *      then exiting with EXIT_FAILURE, or by having an assert() fail.
 */
void que_insert( Que queue, Vertex vert ){
    if (queue->size == 0){
        VNode headOfList = malloc(sizeof(struct VertexQNode));
        if (headOfList == NULL) fprintf(stderr, "Unable to Insert Data.\n");
        headOfList->vert = vert;
        headOfList->next = NULL;
        queue->VNodeList = headOfList;
    }
    else {
        bool result = insertList(queue, vert);
        if (!result) fprintf(stderr, "Unable to Insert Data.\n");
    }
    queue->size++;
}

/** Indicate whether or not the supplied Queue is empty
 * 
 *  @param the QueueADT to be tested
 *  @return true if the queue is empty, otherwise false
 */
bool que_empty( Que queue ){
    if (queue->size > 0) {return false;}
    else {return true;}
}

/** Remove and return the first element from the QueueADT.
 * 
 *  @param queue the QueueADT to be manipulated
 *  @return the value that was removed from the queue
 *  @exception If the queue is empty, the program should terminate
 *      with an error message.  This can be done by printing an
 *      appropriate message to the standard error output and then
 *      exiting with EXIT_FAILURE, or by having an assert() fail.
 */
Vertex que_remove( Que queue ){
    assert(!que_empty(queue));
    VNode head = queue->VNodeList;
    Vertex vertex = head->vert;
    queue->VNodeList = head->next;
    queue->size--;
    free(head);
    return vertex;
}

//----------------END QUEUE-----------------------------------------------------


//----------------SEARCH-----------------------------------------------------

/**
 * Finds the shortest path of the provided maze if there is one
 * @params lstVertices: an array of Vertex structs that make up the maze
 * @return void
 */
void breadthFirstSearch(Vertex * lstVertices) {  
    Que nodeQue = que_create();
    lstVertices[0]->dist = 0;
    que_insert(nodeQue, lstVertices[0]);
    while(!que_empty(nodeQue)){
        //gets the closest vertex
        Vertex currentVert = que_remove(nodeQue);   
        for (int n = 0; n < currentVert->nborCount; n++){
            int indexOfNeighbor = currentVert->nCoords[n]->listIndex;    
            Vertex currNeighbor = lstVertices[indexOfNeighbor];
            int newDist = currentVert->dist + 1;
            if (newDist < currNeighbor->dist || currNeighbor->dist <= -1){
                currNeighbor->previous = currentVert;
                currNeighbor->dist = newDist;
                que_insert(nodeQue, currNeighbor);    
            }          
        }   
    }
    free(nodeQue);   
}

/**
 * Takes in the resulting shortes path from the BFS and nicely formats it into an
 * array starting from the origin and ending at the ending spot
 * @params lstVertices: the list of ALL of the maze's vertices, stepCount: the total number of vertices in the
 * solution path, totalVerts: the total number of vertices overall
 * @return an array of just the solution vertices
 */
Vertex * formatShortestPath(Vertex * lstVertices, int stepCount, int totalVerts){
    Vertex * reversedList = malloc(stepCount * sizeof(struct vertex));
    Vertex curr = lstVertices[totalVerts-1];
    for (int i = stepCount-1; i >= 0; i--){
        reversedList[i] = curr;
        curr = curr->previous;
    }
    return reversedList;
}

/**
 * Counts the total amount of vertices in the solution path
 * @params lstVertices: an array of all of the vertices in the maze, totalVerts: the total number of vertices
 * @return integer of the total amount of vertices in the solution
 */
int countShortestPath(Vertex * lstVertices, int totalVerts){
    Vertex curr = lstVertices[totalVerts-1];
    int steps = 1;
    while (curr->coords->x != 0 || curr->coords->y != 0){
        curr = curr->previous;
        steps++;
    }
    return steps;
}

//----------------END SEARCH-----------------------------------------------------

//----------------FILE READ FUNCTIONS-----------------------------------------------------

/**
 * Creates a new Vertex Structure
 * @params xCoord: The Vertex's xCoordinte, yCoord: same for y, index: the Vertex's index
 * in the overall vertex list
 * @return a fresh off the presses Vertex
 */
Vertex createVertex(int xCoord, int yCoord, int index){
    Vertex newVertex = malloc(sizeof(struct vertex));
    if (newVertex == NULL){
        return NULL;
    }
    newVertex->coords = malloc(sizeof(struct Coords));
    newVertex->coords->x = xCoord;
    newVertex->coords->y = yCoord;
    newVertex->coords->listIndex = index;
    newVertex->dist = -1;
    newVertex->nCoords = malloc(4 * sizeof(struct Coords));
    newVertex->nborCount = 0;
    newVertex->previous = NULL;
    return newVertex;
}

/**
 * Reads the input file from either the provided file pointer, or standard in if one isn't provided
 * @params fp: file pointer to input file, or NULL if reading from stdin
 * @return A linked list representation of the vertices present in the file
 */
FileDataLinkedList * readFile(FILE *fp){
    int xSize, ySize, xCoord, VertexCount;
    xSize = 0;
    ySize = 0;
    xCoord = 0;
    VertexCount = 0;
    char character,prevChar;
    FileDataLinkedList * list = malloc(sizeof(FileDataLinkedList));
    ListNode tempHead;
    list->data = NULL;
    if (fp == NULL){
        character = fgetc(stdin);
    }else{
        character = fgetc(fp);
    }
    if (character != -1){
        do{
            if (character == '\n'){
                xSize = xCoord;
                xCoord = 0;
                ySize++;
            }
            if (character == '0'){
                VertexCount++;
                ListNode newNode = malloc(sizeof(struct Node));
                newNode->x = xCoord;
                newNode->y = ySize;
                newNode->next = NULL;
                if (list->data == NULL){
                    tempHead = newNode;
                    list->data = newNode;
                }
                else{
                    list->data->next = newNode;
                    list->data = newNode;
                }
                xCoord++;
            }
            if (character == '1'){
                xCoord++;
            }
            prevChar = character;
            if (fp == NULL){
                character = fgetc(stdin);
            }else{
                character = fgetc(fp);
            }
        }while (character != -1);
    }
    if (prevChar != '\n'){
        ySize++;
    }
    list->totalVertices = VertexCount;
    list->data = tempHead;
    list->xSize = xSize;
    list->ySize = ySize;
    return list;
}

/**
 * Creates an adjacency list for the provided Vertex
 * @params xMax: the maximum X size of the maze, yMax: the maximum Y size of the maze, v: the vertex to
 * create the list for, boardArray: 2d matrix representation of the maze
 * @return void
 */
void createNeighborArray(int xMax, int yMax, Vertex v, Vertex ** boardArray){
    int xCoord = v->coords->x;
    int yCoord = v->coords->y;
    if (!(xCoord+1 >= xMax) && (boardArray[xCoord+1][yCoord] != 0)){
        v->nCoords[v->nborCount++] = boardArray[xCoord+1][yCoord]->coords;
    }
    if (!(yCoord+1 >= yMax) && (boardArray[xCoord][yCoord+1] != 0)){
        v->nCoords[v->nborCount++] = boardArray[xCoord][yCoord+1]->coords;
    }
    if (!(xCoord-1 < 0) && (boardArray[xCoord-1][yCoord] != 0)){
        v->nCoords[v->nborCount++] = boardArray[xCoord-1][yCoord]->coords;
    }
    if (!(yCoord-1 < 0) && (boardArray[xCoord][yCoord-1] != 0)){
        v->nCoords[v->nborCount++] = boardArray[xCoord][yCoord-1]->coords;
    }
}   

/**
 * Turns the linked list read in from the file into a traditional array
 * @params list: linked list read from the file
 * @return array of vertex structures
 */
Vertex * createVertexArray(FileDataLinkedList * list){
    int X = list->xSize;
    int Y = list->ySize;
    int index = 0;
    Vertex * resultsArray = malloc(list->totalVertices * sizeof(Vertex));
    Vertex ** array = malloc(X * sizeof(Vertex*));
    for (int i = 0; i < X; i++){
        array[i] = malloc(Y * sizeof(Vertex));
        for (int j = 0; j < Y; j++){
            array[i][j] = 0;
        }
    }
    ListNode curr = list->data;
    ListNode holder = curr;
    while(curr != NULL){
        Vertex newVertex = createVertex(curr->x,curr->y,index);
        resultsArray[index++] = newVertex;
        array[curr->x][curr->y] = newVertex;
        curr = curr->next;
        free(holder);
        holder = curr;
    }
    for (int i = 0; i < X; i++){
        for (int j = 0; j < Y; j++){
            if (array[i][j] != 0){
                createNeighborArray(X, Y, array[i][j], array);
            }
        }
    }
    free(list);
    for (int i = 0; i < X; i++){
        free(array[i]);
    }
    free(array);
    return resultsArray;
}

//----------------END FILE READ-----------------------------------------------------


//----------------DISPLAY-----------------------------------------------------

/**
 * Displays the board to stdout
 * @params vertexArray: array of vertices, xMax: maximum X size, yMax: maximum Y size, borders: boolean
 * indicating whether the user requested boards on the print or not
 * @return none
 */
void displayRawBoard(Vertex * vertexArray, int xMax, int yMax, FILE * fp, int totalVerts){
    int vIndex = 0;
    for (int i = 0; i < yMax; i++){
        fprintf(fp,"\n");
        for (int j = 0; j < xMax; j++){
            if (vIndex < totalVerts){
                Vertex curr = vertexArray[vIndex];
                if (curr->coords->x == j && curr->coords->y == i){
                    vIndex++;
                    fprintf(fp,"0 ");
                }
                else{
                    fprintf(fp,"1 ");
                }
            }
            else{
                fprintf(fp,"1 ");
            }
        }
    }
    fprintf(fp,"\n");
}

/**
 * Displays the board to stdout
 * @params vertexArray: array of vertices, xMax: maximum X size, yMax: maximum Y size, borders: boolean
 * indicating whether the user requested boards on the print or not
 * @return none
 */
void displayNoSolutionBoard(int xMax, int yMax, bool borders, FILE* fp){
    int vIndex = 0;
    if (borders){
        for (int i = 0; i < (2*xMax)+3; i++){
            fprintf(fp,"O");
        }
    }
    for (int i = 0; i < yMax; i++){
        fprintf(fp,"\n");
        if (borders && i != 0){
            fprintf(fp,"O ");
        }
        for (int j = 0; j < xMax; j++){
            fprintf(fp,"O ");
        }
        if (borders && i != yMax-1){
            fprintf(fp,"O");
        }
    }
    if (borders){
        fprintf(fp,"\n");
        for (int i = 0; i < (2*xMax)+3; i++){
            fprintf(fp,"O");
        }
    }
    fprintf(fp,"\n");
}

/**
 * Displays And/Or writes to the provided output file the solution
 * @params solVertices: an array of the vertices involve in the solution path, nonSolVertices: an array of all of the vertices including
 * the ones not in the solution, xMax: the maximum X size of the maze, yMax: the maximum Y size of the max, totalSolVerts: the total number of
 * solution vertices, totalNonSolVerts: the total number of all vertices in the maze, fp: file pointer to the file to write to, NULL if no file is provided
 * @return nothing just outputs things
 */
void displayAndOrWriteSolution(Vertex * solVertices,Vertex * nonSolVerticecs, int xMax, int yMax, int totalSolVerts, int totalNonSolVerts, FILE *fp, bool pSteps){
    char ** array = malloc(xMax * sizeof(char*));
    for (int i = 0; i < xMax; i++){
        array[i] = malloc(yMax * sizeof(char));
        memset(array[i], 'O', yMax * sizeof(char));
    }
    for (int i = 0; i < totalSolVerts; i++){
        Vertex curr = solVertices[i];
        array[curr->coords->x][curr->coords->y] = '+';
    }
    for (int i = 0; i < totalNonSolVerts;i++){
        Vertex curr = nonSolVerticecs[i];
        if (array[curr->coords->x][curr->coords->y] != '+'){
            array[curr->coords->x][curr->coords->y] = ' ';
        }
    }
    for (int i = 0; i < (2*xMax)+3; i++){
        fprintf(fp, "O");
    }
    if (pSteps){
        fprintf(fp, "\n+ ");
    }else{
        fprintf(fp, "\n  ");
    }
    for (int i = 0; i < yMax; i++){
        if (i > 0){
            fprintf(fp,"O ");
        }
        for (int j = 0; j < xMax; j++){
            char currChar = array[j][i];
            if (pSteps){
                fprintf(fp, "%c ", currChar);
            }else{
                if (currChar == '+') fprintf(fp, "  ");
                else{fprintf(fp, "%c ", currChar);}
            }
        }
        if (i == yMax-1){
            if (pSteps){
                fprintf(fp, "+\n");
            }
            else{
                fprintf(fp, " \n");
            }
        }
        else{
            fprintf(fp, "O\n");
        }
    }
    for (int i = 0; i < (2*xMax)+3; i++){
        fprintf(fp, "O");
    }
    fprintf(fp, "\n");
    for (int i = 0; i < xMax; i++){
        free(array[i]);
    }
    free(array);
}

//----------------END DISPLAY-----------------------------------------------------

/**
 * Main function for the maze solving program. Reads command line arguments and properly
 * decides what actions to take based on that
 */
int main(int argc, char * argv[]){
    bool borders,pSteps,pMatrix,pSolution;
    borders = false;pSteps=false;pMatrix=false;pSolution=false;
    char infile[256],outfile[256];
    infile[0] = '~';
    outfile[0] = '~';
    int opt;
    while ((opt = getopt(argc,argv,"hbsmpi:o:"))!= -1){
        switch (opt){
            case 'h':
                printf("Options:\n"
                        "-h Print this helpful message to stdout.\n"
                        "-b Add borders and pretty print.  (Default off.)\n"
                        "-s Print shortest solution steps. (Default off.)\n"
                        "-m Print matrix after reading.    (Default off.)\n"
                        "-p Print solution with path.      (Default off.)\n"
                        "-i INFILE Read maze from INFILE.  (Default stdin.)\n"
                        "-o OUTFILE Write maze to OUTFILE. (Default stdout.)\n");
                return 0;
            case 'b':
                borders = true;
                continue;
            case 's':
                pSteps = true;
                continue;
            case 'm':
                pMatrix = true;
                continue;
            case 'p':
                pSolution = true;
                continue;
            case 'i':
                strcpy(infile,optarg);
                continue;
            case 'o':
                strcpy(outfile,optarg);
                continue;
        }
    }
    FILE *fp;
    if (infile[0] == '~'){
        fp = NULL;
    }else{
        fp = fopen(infile,"r");
        if (errno != 0){
            printf("File Not Found.");
            return errno;
        }
    }
    FileDataLinkedList * list = readFile(fp);
    if (fp != NULL){
        fclose(fp);
    }
    FILE *outfp;
    if (outfile[0] == '~'){
        outfp = stdout;
    }else{
        outfp = fopen(outfile, "w");
    }
    int xMax = list->xSize;
    int yMax = list->ySize;
    int totalVerts = list->totalVertices;
    //Checks if the maze doesn't have any open spots
    if (list->totalVertices != 0){
        Vertex * array = createVertexArray(list);
        //First check to see if the maze is unsolveable, checks 2 things:
        //                      1) The first vertex visited is not the origin 
        //                      2) The last vertex visited is the ending spot
        if ( (array[0]->coords->x != 0 || array[0]->coords->y != 0) || (array[totalVerts-1]->coords->x != xMax-1 || array[totalVerts-1]->coords->y != yMax-1)){
            fprintf(outfp, "No solution.\n");
            freeContentsOfList(array, totalVerts);
            free(array);
            return 0;
        }
        //Prints back the raw matrix if requested
        if (pMatrix){
            fprintf(outfp, "Read this matrix:");
            displayRawBoard(array, xMax, yMax,outfp, totalVerts);
        }
        breadthFirstSearch(array);
        //Last check if the maze is unsolveable, checks if the ending spot is ever reached
        if (array[totalVerts-1]->previous == NULL){
            fprintf(outfp, "No solution.\n");
            freeContentsOfList(array, totalVerts);
            free(array);
            return 0;
        }
        //Gets the amount of steps required for the most efficient solution and prints it if requested
        int steps = countShortestPath(array, totalVerts);
        Vertex * solutionArray = formatShortestPath(array, steps, totalVerts);
        if (pSteps){
            fprintf(outfp,"Solution in %d steps.\n",steps);
        }
        if (pSolution || borders){
            displayAndOrWriteSolution(solutionArray, array, xMax, yMax, steps, totalVerts, outfp, pSolution);
        }
        //Frees all of the memory associated with the data structures used to hold the vertices
        freeContentsOfList(array, totalVerts);
        free(array);
        free(solutionArray);
    }else{
        if (borders || pMatrix){
            displayNoSolutionBoard(xMax,yMax,borders,outfp);
        }
        free(list);
        if (pSteps){
            fprintf(outfp, "No solution.\n");
        }
    }
    if (outfp != stdout){
        fclose(outfp);
    }
    return 0;
}
