/**
 * Implementation of the Queue Abstract Data Type outlined in the QueueADT Header File
 * @author Zach Morgan
 * RIT Username : zdm9127
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>


#define _QUEUE_IMPL_

/**
 * type definition for the compare function used to order the queue
 */
typedef int (*cmp)(const void * a, const void * b);

/**
 * Node struct used to create the linked list that holds the queue's data
 */
typedef struct Node{
    void * data;
    struct Node * next;
} Node;

/**
 * Queue structure: 
 * fields: funct=compare function used to order, dataList= pointer to the head of the linked list
 * size: indicates the amount of items the queue holds
 */
struct queueStruct  {
    cmp funct;
    struct Node * dataList;
    int size;
};

/**
 * Establishes that the Abstract Data Type implementation is a pointer to the queueStructure
 */
typedef struct queueStruct * QueueADT;


/**
 * Helper function that inserts a value in the correct spot in the linked list
 * @params queue : the q to add to, data : generic data to add to the correct spot
 * @returns boolean indicating success or failure
 */
bool insertList(QueueADT queue, void * data){
    Node * newNode = malloc(sizeof(Node));
    bool beforeFirstNode = true;
    Node * list = queue->dataList;
    Node * previousNode;
    if (newNode == NULL){
        return false;
    }
    if (queue->funct == NULL){
        while (list->next != NULL){
            list = list->next;
            beforeFirstNode = false;
        }
        newNode->next = NULL;
        list->next = newNode;
    }
    else{
        while (list != NULL && (queue->funct != NULL && queue->funct(data, list->data) > 0)){
            previousNode = list;
            list = list->next;
            beforeFirstNode = false;
        }
        if (beforeFirstNode) {
            newNode->next = list;
            queue->dataList = newNode;
        }
        else{
            newNode->next = previousNode->next;
            previousNode->next = newNode;
        }
    }
    newNode->data = data;
    return true;
}



#include "queueADT.h"

/// Create a QueueADT which uses the supplied function as a comparison
/// routine.
///
/// The comparison function takes two void * parameters, and returns
/// an integer result which indicates the relationship between the
/// two things:
///
///	Result	Relationship
///	======	===========
///	 < 0	a < b
///	 = 0	a == b
///	 > 0	a > b
///
/// where ">" and "<" are dependent upon the data being compared
///
/// @param cmp the address of the comparison function to be used for
///    ordering this queue, or NULL if standard FIFO behavior is desired
/// @return a QueueADT instance, or NULL if the allocation fails
QueueADT que_create( int (*cmp)(const void * a, const void * b) ){
    QueueADT instance = malloc(sizeof(QueueADT));
    if (instance == NULL){
        return NULL;
    }
    instance->funct = cmp;
    instance->size = 0;
    return instance;
}

/* Compare function used during development
int sampleCompareFunc(const void * a, const void * b){
    long int c = *(long int*)a;
    long int d = *(long int*)b;
    if (c == d) return 0;
    if (c < d) return -1;
    if (c > d) return 1;
    return 0;
}
*/

/**
 * Helper function that iterates through the linked list containing
 * the Queue's contents and frees the memory allocated for each
 * @param queue : the queue to free the contents from
 */
void freeContentsOfList(QueueADT queue){
    Node * curr = queue->dataList;
    Node * tempToHold;
    if (curr != NULL){
        tempToHold = curr->next;
    }
    while(curr != NULL){
        free(curr);
        curr = tempToHold;
        tempToHold = tempToHold->next;
    }
}

/// Tear down and deallocate the supplied QueuADT.
///
/// @param queue - the QueueADT to be manipulated
void que_destroy( QueueADT queue ){
   freeContentsOfList(queue);
   free(queue);
}

/// Remove all contents from the supplied QueueADT.
///
/// @param queue - the QueueADT to be manipuated
void que_clear( QueueADT queue ){
    queue->size = 0;
    freeContentsOfList(queue);
}

/// Insert the specified data into the Queue in the appropriate place
///
/// Uses the queue's comparison function to determine the appropriate
/// place for the insertion.
///
/// @param queue the QueueADT into which the value is to be inserted
/// @param data the data to be inserted
/// @exception If the value cannot be inserted into the queue for
///     whatever reason (usually implementation-dependent), the program
///     should terminate with an error message.  This can be done by
///     printing an appropriate message to the standard error output and
///     then exiting with EXIT_FAILURE, or by having an assert() fail.
void que_insert( QueueADT queue, void * data ){
    if (queue->size == 0){
        Node * headOfList = malloc(sizeof(Node));
        if (headOfList == NULL) fprintf(stderr, "Unable to Insert Data.\n");
        headOfList->data = data;
        headOfList->next = NULL;
        queue->dataList = headOfList;
    }
    else {
        bool result = insertList(queue, data);
        if (!result) fprintf(stderr, "Unable to Insert Data.\n");
    }
    queue->size++;
}

/// Indicate whether or not the supplied Queue is empty
///
/// @param the QueueADT to be tested
/// @return true if the queue is empty, otherwise false
bool que_empty( QueueADT queue ){
    if (queue->size > 0) {return false;}
    else {return true;}
}

/// Remove and return the first element from the QueueADT.
///
/// @param queue the QueueADT to be manipulated
/// @return the value that was removed from the queue
/// @exception If the queue is empty, the program should terminate
///     with an error message.  This can be done by printing an
///     appropriate message to the standard error output and then
///     exiting with EXIT_FAILURE, or by having an assert() fail.
void * que_remove( QueueADT queue ){
    assert(!que_empty(queue));
    Node * head = queue->dataList;
    void * value = head->data;
    queue->dataList = head->next;
    free(head);
    queue->size--;
    return value;
}

//Test Functions used during development
/**
void displayQueueTest(QueueADT queue){
    printf("The size is: %d\n", queue->size);
    Node * curr = queue->dataList;
    int position = 1;
    while(curr != NULL){
        printf("Item #%d : %ld\n", position++, *((long int*)curr->data));
        curr = curr->next;
    }
}
int main(void){
    QueueADT instance = que_create(&sampleCompareFunc);
    long int testData[10] = {5,4,3,2,7,1,8,10,9,6};
    for (int i = 0; i < 10; i++){
        que_insert(instance, (void*)&testData[i]);
        displayQueueTest(instance);
    }
     for (int i = 0; i < 10; i++){
        que_remove(instance);
        displayQueueTest(instance);
    }
}
*/