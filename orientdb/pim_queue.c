#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Data{
    char dataID[50];
    char dataName[30];
    int chatRoom;
} Data;

typedef struct Node Node;

struct Node{
    Node* next;
    Data* data;
};

typedef struct Queue {
    Node* head;
    Node* tail;
    int size;
} Queue;

void push(Queue* queue, Data* data);
Data* pop(Queue* queue);
Data* peek(Queue* queue);
void display(Queue* queue);
Queue* createQueue();
void freeQueue(Queue* queue);

int main(){
    Queue* queue = createQueue();
    display(queue);
    
    Data* mydata1 = (Data*)malloc(sizeof(Data));
    sprintf(mydata1->dataName,"%s","pimpat");
    sprintf(mydata1->dataID,"%s","XXX-1010");
    mydata1->chatRoom = 1010;
    push(queue,mydata1);
    
    Data* mydata2 = (Data*)malloc(sizeof(Data));
    sprintf(mydata2->dataName,"%s","tanapon");
    sprintf(mydata2->dataID,"%s","XXX-1011");
    mydata2->chatRoom = 1011;
    push(queue,mydata2);
    
    display(queue);
    
    Data *mydata3, *mydata4;
    mydata3 = pop(queue);
    
//    Data* mydata5;
//    mydata5 = peek(queue);
//    printf("\npeek(dataID): %s\n\n",mydata5->dataID);
    
    mydata4 = pop(queue);
    display(queue);
    
//    Data* mydata5;
//    mydata5 = peek(queue);
//    printf("\npeek(dataID): %s\n\n",mydata5->dataID);
    
    free(mydata1);
    free(mydata2);
    free(mydata3);
    free(mydata4);
    
    freeQueue(queue);
}

void push(Queue* queue, Data* data){
    // Create a new node
    Node* mynode = (Node*)malloc(sizeof(Node));
    mynode->data = (Data*)malloc(sizeof(Data));
    memcpy(mynode->data,data,sizeof(Data));
    mynode->next = NULL;

    if (queue->head == NULL) { // no head
        queue->head = mynode;
    } else{
        queue->tail->next = mynode;
    }
    queue->tail = mynode;
    queue->size++;
}

Data* pop(Queue* queue){
    if(queue->size>0){
        // get the first item
        Node* head = queue->head;
        // move head pointer to next node, decrease size
        queue->head = head->next;
        queue->size--;
        
        Data* mydata = (Data*)malloc(sizeof(Data));
        memcpy(mydata,head->data,sizeof(Data));
        
        // free the memory of original head
        free(head->data);
        free(head);
        return mydata;
    }
    else{
        printf("no data in queue!!\n\n");
        return NULL;
    }
}

Data* peek(Queue* queue){
    if(queue->size>0){
        Node* head = queue->head;
        return head->data;
    }
    else{
        printf("no data in queue!!\n\n");
        return NULL;
    }
}

void display (Queue* queue) {
    printf("\nDisplay: ");
    if (queue->size == 0) // no item
        printf("No item in queue.\n\n");
    else { // has item(s)
        Node* head = queue->head;
        int i, size = queue->size;
        printf("%d item(s):\n\n", queue->size);
        for (i = 0; i < size; i++) {
            //if (i > 0)
                //printf(", ");
            printf("[%d]\n",i);
            printf("dataID: %s\n", head->data->dataID);
            printf("dataName: %s\n", head->data->dataName);
            printf("chatRoom: %d\n\n", head->data->chatRoom);
            head = head->next;
        }
    }
    //printf("\n\n");
}

Queue* createQueue(){
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void freeQueue(Queue* queue){
    Node *cur, *next;
    for(cur=queue->head;cur!=NULL;cur=next){
        next = cur->next;
        free(cur->data);
        free(cur);
    }
    free(queue);
}

