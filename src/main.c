//
//  main.c
//  bbm203-second-ass
//
//  Created by AhmetBayrak on 17.11.2018.
//  Copyright © 2018 AhmetBayrak. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/*
typedef struct
{
    char *sender_mac_address;
    char *receiver_mac_address;
    
}physicalLayer;

typedef struct
{
    char *sender_IP_address;
    char *receiver_IP_address;
    
}networkLayer;

typedef struct
{
    char *sender_port_number;
    char *reciever_port_number;
    
}transportLayer;

typedef struct
{
    char *sender_ID;
    char *reciever_ID;
    char *message_chunk;
    
}applicationLayer;

typedef struct
{
    physicalLayer physical_layer;
    transportLayer transport_layer;
    networkLayer network_layer;
    applicationLayer application_layer;
    
}layer;

 */

typedef struct
{
    char senderInfo[255];
    char receiverInfo[255];
    char messageChunk[255];
    
}layer;

typedef struct
{
    int top;
    layer *array;
    int capacity;
    
}stack;

typedef struct
{
    int rear;
    int front;
    int capacity;
    int size;
    stack *array;
    
}queue;

typedef struct
{
    int clientID;
    char **routingTableD;
    stack *frame;
    queue *incomingQueue;
    queue *outgoingQueue;
    char log[5555];
    char clientName;
    char IPAddressC[100];
    char macAddressC[100];
    char *outgoing_port_number;
    char *incoming_port_number;
}client;

stack *createStack(int capacity)
{
    stack *st = (stack*)malloc(sizeof(stack));
    st->capacity = capacity;
    st->top = -1;
    st->array = (layer*)malloc(st->capacity * sizeof(layer));
    return st;
}

int isFullStack(stack *st)
{
    return st->top == st->capacity - 1;
}

int isEmptyStack(stack *st)
{
    return st->top == -1;
}

void pushStack(stack *st, layer lyr)
{
    st->array[++st->top] = lyr;
}

layer popTakeStack(stack *st)
{
    return st->array[st->top--];
}

void popStack(stack *st, layer *popped)
{
        *popped = st->array[st->top];
        st->top--;
}

queue *createQueue(int capacity)
{
    queue *que = (queue*) malloc(sizeof(queue));
    que->capacity = capacity;
    que->front = que->size = 0;
    //que->rear = capacity - 1;
    que->rear = 0;
    que->array = (stack*) malloc(que->capacity * sizeof(stack));
    return que;
}

void enqueue(queue *que, stack frame)
{
    que->array[que->rear++] = frame;
    que->size = que->size + 1;
}

//removes from queue and takes it
stack *dequeue(queue *que)
{
    stack *dequeuedStack = createStack(100);
    dequeuedStack = que->array;
    que->front = que->front+1;
    return dequeuedStack;
}

// Fills the clients with the information in the clients.dat and routing.dat
void createClients(FILE *file1, client *clients, int clientNumber, FILE *file2, const char *sender_port_number, const char *receiver_port_number)
{
    int i = 0;
    int k = 0;
    int j = 0;
    
    char line[60];
    fgets(line, 60, file1);
    
    for (i=0; i<clientNumber; i++)
    {
        clients[i].clientID = i; //(i+1)
        clients[i].outgoing_port_number = sender_port_number;
        clients[i].incoming_port_number = receiver_port_number;
        
        clients[i].routingTableD = (char**)malloc(sizeof(char*)*(clientNumber));    // creates 2D array dynamically for routing table
        for(j = 0 ; j < (clientNumber) ; j++)
        {
            clients[i].routingTableD[j] = (char*)malloc(sizeof(char)*(2));
        }
        
        fgets(line, 60, file1);
        char *abc = strtok(line, " ");
        
        // Fills the clients name, IP address MAC address
        while( abc != NULL )
        {
            clients[i].clientName = abc[0];
            abc = strtok(NULL, " \n");
            strcpy(clients[i].IPAddressC, abc);
            abc = strtok(NULL, " \n");
            strcpy(clients[i].macAddressC, abc);
            abc = strtok(NULL, " \n");
        }
    }
    
    char line1[60];
    char *asd;
    
    int h = 0;
    
    // Fills the routing table
    for(i = 0 ; i < clientNumber; i++)
    {
        for(k = 0 ; k < clientNumber; k++)
        {
            fgets(line1, 60, file2);
            asd = strtok(line1, " \n\r-");
            h = 0;
            if(asd == NULL)
            {
                break;
            }
            clients[i].routingTableD[k][h] = asd[0];
            
            while( asd != NULL )
            {
                asd = strtok(NULL, "\n\r-");
                if(asd != NULL)
                {
                    clients[i].routingTableD[k][h+1] = asd[0];
                }
            }
        }
    }
    fclose(file1);
    fclose(file2);
}

// Finds the next client to reach intended client
char findPath(client *clients, char source, char destination, int clientNumber)
{
    int i = 0;
    int j = 0;
    for(i = 0 ; i < clientNumber ; i++)
    {
        if(clients[i].clientName == source)
        {
            for(j = 0 ; j < clientNumber ; j++)
            {
                if(clients[i].routingTableD[j][0] == destination)
                {
                    return clients[i].routingTableD[j][1];
                }
            }
        }
    }
    return NULL;
}

// Returns the index of desired client according to name
int findByID(client *clients, char clientName, int clientNumber)
{
    int i = 0;
    for (i = 0 ; i < clientNumber; i++)
    {
        if(clients[i].clientName == clientName)
        {
            return i;
        }
    }
    return NULL;
}

// Creates frames and places them to outgoing queue
void messageCommand(char *senderID, char *receiverID, char *message, int max_message_size, client *clients, int clientNumber, const char *sender_port_number, const char *receiver_port_number)
{
    int i = 0;
    int j = 0;
    int k = 0;
    char message_chunk[max_message_size];
    int m = 0;
    int h = 0;
    int f = 0;
    int s = 0;
    char totalMessage[255];
    int b = 0;
    char number_of_frames[2];
    char number_of_hops[2];
    char timestamp[20];
    
    int senderIndex;
    senderIndex = findByID(clients, senderID[0], clientNumber);
    
    char abc;
    
    int frameNumber = strlen(message)/max_message_size;
    
    for(s = 0 ; s < clientNumber ; s++)
    {
        clients[s].incomingQueue = createQueue(frameNumber+1);
        clients[s].outgoingQueue = createQueue(frameNumber+1);
    }
    
    for(i = 0 ; i < frameNumber+1 ; i++)
    {
        stack *tempStack = createStack(100);
        h = i * max_message_size;
        for(m = 0 ; m < max_message_size ; m++)
        {
            message_chunk[m] = message[h];
            h++;
        }
        message_chunk[20] = NULL;
        for(j = 0 ; j < clientNumber ; j++)
        {
            for(k = 0 ; k < clientNumber; k++)
            {
                if(senderID[0] == clients[j].clientName && receiverID[0] == clients[k].clientName)
                {
                    printf("Frame #%d\n", i+1);

                    // Cretaes Application Layer and push it to stack
                    layer *application_layer = (layer*) malloc(sizeof(layer));
                    strcpy(application_layer->receiverInfo, receiverID);
                    strcpy(application_layer->senderInfo, senderID);
                    strcpy(application_layer->messageChunk, message_chunk);
                    
                    pushStack(tempStack, *application_layer);
                    free(application_layer);

                    // Cretaes Transport Layer and push it to stack
                    layer *transport_layer = (layer*) malloc(sizeof(layer));
                    strcpy(transport_layer->receiverInfo, receiver_port_number);
                    strcpy(transport_layer->senderInfo, sender_port_number);

                    
                    pushStack(tempStack, *transport_layer);
                    free(transport_layer);
                    
                    // Cretaes Network Layer and push it to stack
                    layer *network_layer = (layer*) malloc(sizeof(layer));
                    strcpy(network_layer->receiverInfo, clients[k].IPAddressC);
                    strcpy(network_layer->senderInfo, clients[j].IPAddressC);

                    
                    pushStack(tempStack, *network_layer);
                    free(network_layer);
                    
                    abc = findPath(clients, senderID[0], receiverID[0], clientNumber);  // Finds next client to reach destination
                    
                    for(f = 0 ; f < clientNumber ; f++)
                    {
                        if(clients[f].clientName == abc)
                        {
                            // Cretaes Physical Layer and push it to stack
                            layer *physical_layer = (layer*) malloc(sizeof(layer));
                            strcpy(physical_layer->receiverInfo, clients[f].macAddressC);
                            strcpy(physical_layer->senderInfo, clients[j].macAddressC);
                            
                            pushStack(tempStack, *physical_layer);
                            free(physical_layer);
                        }
                    }
                    
                    enqueue(clients[j].outgoingQueue, *tempStack);  // Adds frames to outgoing queue
                    
                    printf("Sender MAC Address: %s, ", tempStack->array[3].senderInfo);
                    printf("Receiver MAC Address: %s\n", tempStack->array[3].receiverInfo);
                    
                    printf("Sender IP Address: %s, ", tempStack->array[2].senderInfo);
                    printf("Receiver IP Address: %s\n", tempStack->array[2].receiverInfo);
                    
                    printf("Sender port number: %s, ", tempStack->array[1].senderInfo);
                    printf("Receiver port number: %s\n", tempStack->array[1].receiverInfo);
                    
                    printf("Sender ID: %s, ", tempStack->array[0].senderInfo);
                    printf("Receiver ID: %s\n", tempStack->array[0].receiverInfo);
                    
                    printf("Message chunk carried: %s\n", tempStack->array[0].messageChunk);
                    
                    for(j = 0 ; j < 8; j++)
                    {
                        printf("-");
                    }
                    printf("\n");
                }
            }
        }
        
        // Add this activity to log of sender client
        strcpy(totalMessage, clients[senderIndex].outgoingQueue->array[0].array[0].messageChunk);
        
        for( b = 1 ; b < clients[senderIndex].outgoingQueue->rear; b++)
        {
            strcat(totalMessage, clients[senderIndex].outgoingQueue->array[b].array[0].messageChunk);
        }
        
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        
        sprintf(timestamp, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        
        strcpy(clients[senderIndex].log, "Log Entry #1:\nTimestamp: ");
        strcat(clients[senderIndex].log, timestamp);
        strcat(clients[senderIndex].log, "\nMessage: ");
        strcat(clients[senderIndex].log, totalMessage);
        strcat(clients[senderIndex].log, "\nNumber of frames: ");
        
        sprintf(number_of_frames, "%d", clients[senderIndex].outgoingQueue->rear);
        sprintf(number_of_hops, "%d", 0);
        strcat(clients[senderIndex].log, number_of_frames);
        strcat(clients[senderIndex].log, "\nNumber of hops: ");
        strcat(clients[senderIndex].log, number_of_hops);
        strcat(clients[senderIndex].log, "\nSender ID: ");
        strcat(clients[senderIndex].log, clients[senderIndex].outgoingQueue->array[0].array[0].senderInfo);
        strcat(clients[senderIndex].log, "\nReceiver ID: ");
        strcat(clients[senderIndex].log, clients[senderIndex].outgoingQueue->array[0].array[0].receiverInfo);
        strcat(clients[senderIndex].log, "\nActivity: Message Forwarded.\nSuccess: Yes");
    }

}

// Prints information of desired frame if frame exists
void showFrameInfo(char *clientName, char *queueType, char *frameNumber, client *clients, int clientNumber, int hops)
{
    int i = 0;
    for(i = 0 ; i < clientNumber ; i++)
    {
        if(clients[i].clientName == clientName[0])
        {
            if(strcmp(queueType, "in") == 0)
            {
                // Check whether frame exists
                if(clients[i].incomingQueue->array[atoi(frameNumber)-1].array[0].senderInfo != NULL && atoi(frameNumber) <=  clients[i].incomingQueue->rear)
                {
                    printf("Current Frame #%s on the incoming queue of client %s\n", frameNumber, clientName);
                    printf("Carried Message: \"%s\"\n", clients[i].incomingQueue->array[atoi(frameNumber)-1].array[0].messageChunk);
                    printf("Layer 0 info: Sender ID: %s, Receiver ID: %s\n", clients[i].incomingQueue->array[atoi(frameNumber)-1].array[0].senderInfo, clients[i].incomingQueue->array[atoi(frameNumber)-1].array[0].receiverInfo);
                    printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n", clients[i].incomingQueue->array[atoi(frameNumber)-1].array[1].senderInfo, clients[i].incomingQueue->array[atoi(frameNumber)-1].array[1].receiverInfo);
                    printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n", clients[i].incomingQueue->array[atoi(frameNumber)-1].array[2].senderInfo, clients[i].incomingQueue->array[atoi(frameNumber)-1].array[2].receiverInfo);
                    printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n", clients[i].incomingQueue->array[atoi(frameNumber)-1].array[3].senderInfo, clients[i].incomingQueue->array[atoi(frameNumber)-1].array[3].receiverInfo);
                    printf("Number of hops so far: %d\n", hops);
                    
                }
                else
                {
                    printf("No such frame.\n");
                }            }
            else if (strcmp(queueType, "out") == 0)
            {
                // Check whether frame exists
                if(clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[0].senderInfo != NULL && atoi(frameNumber) <= clients[i].outgoingQueue->rear)
                {
                    printf("Current Frame #%s on the outgoing queue of client %s\n", frameNumber, clientName);
                    printf("Carried Message: \"%s\"\n", clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[0].messageChunk);
                    printf("Layer 0 info: Sender ID: %s, Receiver ID: %s\n", clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[0].senderInfo, clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[0].receiverInfo);
                    printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n", clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[1].senderInfo, clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[1].receiverInfo);
                    printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n", clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[2].senderInfo, clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[2].receiverInfo);
                    printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n", clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[3].senderInfo, clients[i].outgoingQueue->array[atoi(frameNumber)-1].array[3].receiverInfo);
                    printf("Number of hops so far: %d\n", hops);
                }
                else
                {
                    printf("No such frame.\n");
                }
            }
        }
    }
}

// Prints number of frames in the desired queue
void showQInfo(char *clientName, char *queueType, client *clients, int clientNumber)
{
    int i = 0;
    for(i = 0 ; i < clientNumber ; i++)
    {
        
        if(clients[i].clientName == clientName[0])
        {
            if(strcmp(queueType, "in") == 0)
            {
                printf("Client %c Incoming Queue Status\n",clients[i].clientName);
                printf("Current total number of frames: %d\n",clients[i].incomingQueue->rear);
                
            }
            else if (strcmp(queueType, "out") == 0)
            {

                printf("Client %c Outgoing Queue Status\n",clients[i].clientName);
                printf("Current total number of frames: %d\n",clients[i].outgoingQueue->rear);
            }
        }
    }
}

// This command comes after message command so first outgoing queue would be ready when this comamnd comes. this command will start from outgoing queue of clientName and it will call itself recursively until reaches the intended client. If the intended client is not reachable prints error and messages will be dropped.
void sendCommand(client *clients, int clientNumber, char clientName, int hops)
{
    int i = 0;
    int f = 0;
    int m = 0;
    int b = 0;
    char totalMessage[255];
    int p = 0;
    char number_of_frames[2];
    char number_of_hops[2];
    char nextHopName;
    char timestamp[20];
 
    // Find index of sender client
    int senderIndex;
    senderIndex = findByID(clients, clientName, clientNumber);
    
    // Find index of intended receiver client
    int realReciverIndex;
    realReciverIndex = findByID(clients, clients[senderIndex].outgoingQueue->array[0].array[0].receiverInfo[0], clientNumber);

    // Find index of netx receiver client
    int nextHopIndex;
    nextHopIndex = findByID(clients, clients[senderIndex].outgoingQueue->array[0].array[3].receiverInfo[0], clientNumber);
    
    // if intended receiver is not directly neigbour of sender client
    if(realReciverIndex != nextHopIndex)
    {
        printf("A message received by client %c, but intended for client %c. Forwarding...\n", clients[nextHopIndex].clientName, clients[realReciverIndex].clientName);
        hops++;
        
        for(i = 0 ; i < clients[senderIndex].outgoingQueue->rear; i++)
        {
            enqueue(clients[nextHopIndex].incomingQueue, clients[senderIndex].outgoingQueue->array[i]);
            enqueue(clients[nextHopIndex].outgoingQueue, clients[nextHopIndex].incomingQueue->array[i]);
        }
        
        strcpy(totalMessage, clients[nextHopIndex].outgoingQueue->array[0].array[0].messageChunk);
        
        for( b = 1 ; b < clients[nextHopIndex].outgoingQueue->rear; b++)
        {
            strcat(totalMessage, clients[nextHopIndex].outgoingQueue->array[b].array[0].messageChunk);
        }

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        
        // Adds to log
        sprintf(timestamp, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        
        strcpy(clients[nextHopIndex].log, "Log Entry #1:\nTimestamp: ");
        strcat(clients[nextHopIndex].log, timestamp);
        strcat(clients[nextHopIndex].log, "\nMessage: ");
        strcat(clients[nextHopIndex].log, totalMessage);
        strcat(clients[nextHopIndex].log, "\nNumber of frames: ");
        
        sprintf(number_of_frames, "%d", clients[nextHopIndex].outgoingQueue->rear);
        sprintf(number_of_hops, "%d", hops);
        strcat(clients[nextHopIndex].log, number_of_frames);
        strcat(clients[nextHopIndex].log, "\nNumber of hops: ");
        strcat(clients[nextHopIndex].log, number_of_hops);
        strcat(clients[nextHopIndex].log, "\nSender ID: ");
        strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].senderInfo);
        strcat(clients[nextHopIndex].log, "\nReceiver ID: ");
        strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].receiverInfo);
        strcat(clients[nextHopIndex].log, "\nActivity: Message Received.\nSuccess: Yes\n");
        
        nextHopName = findPath(clients, clients[nextHopIndex].clientName, clients[realReciverIndex].clientName, clientNumber);
        
        // if intended receiver is not reachable
        if(nextHopName == '-' || nextHopName == '\0')
        {
            // adds to log
            for(p = 0 ; p < 14; p++)
            {
                strcat(clients[nextHopIndex].log, "-");
            }
            
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            
            sprintf(timestamp, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            
            strcat(clients[nextHopIndex].log, "\nLog Entry #2:\nTimestamp: ");
            strcat(clients[nextHopIndex].log, timestamp);
            strcat(clients[nextHopIndex].log, "\nMessage: ");
            strcat(clients[nextHopIndex].log, totalMessage);
            strcat(clients[nextHopIndex].log, "\nNumber of frames: ");
            
            sprintf(number_of_frames, "%d", clients[nextHopIndex].outgoingQueue->rear);
            sprintf(number_of_hops, "%d", hops);
            strcat(clients[nextHopIndex].log, number_of_frames);
            strcat(clients[nextHopIndex].log, "\nNumber of hops: ");
            strcat(clients[nextHopIndex].log, number_of_hops);
            strcat(clients[nextHopIndex].log, "\nSender ID: ");
            strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].senderInfo);
            strcat(clients[nextHopIndex].log, "\nReceiver ID: ");
            strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].receiverInfo);
            strcat(clients[nextHopIndex].log, "\nActivity: Message Forwarded.\nSuccess: No");
            
            printf("Error: Unreachable destination. Packets are dropped after %d hops!\n", hops);
        }
        else
        {
            int nextHopHopIndex;
            
            nextHopHopIndex = findByID(clients, nextHopName, clientNumber);     // find neighbour of next receiver index
            
            // adds to log
            for(f = 0; f < clients[nextHopIndex].outgoingQueue->rear ; f++)
            {
                strcpy(clients[nextHopIndex].outgoingQueue->array[f].array[3].senderInfo, clients[nextHopIndex].macAddressC);
                
                strcpy(clients[nextHopIndex].outgoingQueue->array[f].array[3].receiverInfo, clients[nextHopHopIndex].macAddressC);
                
                printf("\tFrame #%d MAC address change: New sender MAC %s, new receiver MAC %s\n", f+1, clients[nextHopIndex].outgoingQueue->array[f].array[3].senderInfo,clients[nextHopIndex].outgoingQueue->array[f].array[3].receiverInfo);
            }
            
            for(p = 0 ; p < 14; p++)
            {
                strcat(clients[nextHopIndex].log, "-");
            }
            
            strcat(clients[nextHopIndex].log, "\nLog Entry #2:\nTimestamp: ");
            strcat(clients[nextHopIndex].log, timestamp);
            strcat(clients[nextHopIndex].log, "\nMessage: ");
            strcat(clients[nextHopIndex].log, totalMessage);
            strcat(clients[nextHopIndex].log, "\nNumber of frames: ");
            
            sprintf(number_of_frames, "%d", clients[nextHopIndex].outgoingQueue->rear);
            sprintf(number_of_hops, "%d", hops);
            strcat(clients[nextHopIndex].log, number_of_frames);
            strcat(clients[nextHopIndex].log, "\nNumber of hops: ");
            strcat(clients[nextHopIndex].log, number_of_hops);
            strcat(clients[nextHopIndex].log, "\nSender ID: ");
            strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].senderInfo);
            strcat(clients[nextHopIndex].log, "\nReceiver ID: ");
            strcat(clients[nextHopIndex].log, clients[nextHopIndex].outgoingQueue->array[0].array[0].receiverInfo);
            strcat(clients[nextHopIndex].log, "\nActivity: Message Forwarded.\nSuccess: Yes");

            // Call itself recursively until reach the intended receiver client
            sendCommand(clients, clientNumber, clients[nextHopIndex].clientName, hops);
        }
    }
    // if the intended receiver client is directly neighbour of sender client
    else
    {
        char totalMessage1[255];

        
        hops++;
        for(i = 0 ; i < clients[senderIndex].outgoingQueue->rear; i++)
        {
            enqueue(clients[realReciverIndex].incomingQueue, clients[senderIndex].outgoingQueue->array[i]);
        }
        
        printf("A message received by client %c from client %c after a total of %d hops.\n", clients[realReciverIndex].clientName, clients[realReciverIndex].incomingQueue->array[0].array[0].senderInfo[0], hops);

        strcpy(totalMessage1, clients[realReciverIndex].incomingQueue->array[0].array[0].messageChunk);
        
        for( b = 1 ; b < clients[realReciverIndex].incomingQueue->rear; b++)
        {
            strcat(totalMessage1, clients[realReciverIndex].incomingQueue->array[b].array[0].messageChunk);
        }
        
        printf("Message: ");

        for( m = 0 ; m < clients[realReciverIndex].incomingQueue->rear; m++)
        {
            printf("%s", clients[realReciverIndex].incomingQueue->array[m].array[0].messageChunk);

        }
        printf("\n");
        
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        
        sprintf(timestamp, "%d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        
        strcpy(clients[realReciverIndex].log, "Log Entry #1:\nTimestamp: ");
        strcat(clients[realReciverIndex].log, timestamp);
        strcat(clients[realReciverIndex].log, "\nMessage: ");
        strcat(clients[realReciverIndex].log, totalMessage1);
        strcat(clients[realReciverIndex].log, "\nNumber of frames: ");
        
        sprintf(number_of_frames, "%d", clients[realReciverIndex].incomingQueue->rear);
        sprintf(number_of_hops, "%d", hops);
        strcat(clients[realReciverIndex].log, number_of_frames);
        strcat(clients[realReciverIndex].log, "\nNumber of hops: ");
        strcat(clients[realReciverIndex].log, number_of_hops);
        strcat(clients[realReciverIndex].log, "\nSender ID: ");
        strcat(clients[realReciverIndex].log, clients[realReciverIndex].incomingQueue->array[0].array[0].senderInfo);
        strcat(clients[realReciverIndex].log, "\nReceiver ID: ");
        strcat(clients[realReciverIndex].log, clients[realReciverIndex].incomingQueue->array[0].array[0].receiverInfo);
        strcat(clients[realReciverIndex].log, "\nActivity: Message Received.\nSuccess: Yes");
    }
}

// Reads command from commdans.dat and parse them. according to commands route them
void readCommands(FILE *commandFile, int  commandNumber, client *clients, int max_message_size, int clientNumber, const char *sender_port_number, const char *receiver_port_number)
{
    int i = 0;
    int j = 0;
    char *commandType;
    char commandMessage[100];
    char *message;
    char *frameNumber;
    char *frameName;
    char *frameType;
    char *senderID;
    char *receiverID;
    int hops = 0;
    int p = 0;
    
    char line[100];
    fgets(line, 100, commandFile);
    
    for(i = 0 ; i < commandNumber; i++)
    {
        fgets(line, 100, commandFile);
        strcpy(commandMessage, line);
        commandType = strtok(line, " ");
        
        if(strcmp(commandType, "MESSAGE") == 0)
        {
            senderID = strtok(NULL, " ");
            receiverID = strtok(NULL, " ");
            message = strtok(NULL, "#");
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");

            }
            printf("\nCommand: %s", commandMessage);

            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            printf("Message to be sent: %s\n\n", message);
            
            messageCommand(senderID, receiverID, message, max_message_size, clients, clientNumber, sender_port_number, receiver_port_number);
        }
        
        else if(strcmp(commandType, "SHOW_FRAME_INFO") == 0)
        {
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\nCommand: %s", commandMessage);
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            frameName = strtok(commandMessage, " ");
            frameName = strtok(NULL, " ");
            frameType = strtok(NULL, " ");
            frameNumber = strtok(NULL, " \n\r");
            
            showFrameInfo(frameName, frameType, frameNumber, clients, clientNumber, hops);
        }
        
        else if(strcmp(commandType, "SHOW_Q_INFO") == 0)
        {
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\nCommand: %s", commandMessage);
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            frameName = strtok(commandMessage, " ");
            frameName = strtok(NULL, " ");
            frameType = strtok(NULL, " \n\r");
            
            showQInfo(frameName, frameType, clients, clientNumber);
        }
        
        else if(strcmp(commandType, "SEND") == 0)
        {
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\nCommand: %s", commandMessage);
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            frameName = strtok(commandMessage, " ");
            frameName = strtok(NULL, " ");
            sendCommand(clients, clientNumber, frameName[0], hops);
        }
        
        else if(strcmp(commandType, "PRINT_LOG") == 0)
        {
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\nCommand: %s", commandMessage);
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            frameName = strtok(commandMessage, " ");
            frameName = strtok(NULL, " ");
            printf("Client %c Logs:\n", frameName[0]);
            for(p = 0 ; p < 14; p++)
            {
                printf("-");
            }
            printf("\n");
            printf("%s\n", clients[findByID(clients, frameName[0], clientNumber)].log);
        }
        // if there would be invalid command
        else
        {
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\nCommand: %s", commandMessage);
            for(j = 0 ; j < strlen(commandMessage) + 9; j++)
            {
                printf("-");
            }
            printf("\n");
            printf("Invalid command.\n");       // newline olmayabilir.
        }
        
    }
    fclose(commandFile);
    free(clients);
}

int main(int argc, const char * argv[]) {

    FILE *clientsFile = fopen(argv[1], "r");
    FILE *routingFile = fopen(argv[2], "r");
    FILE *commandsFile = fopen(argv[3], "r");
    
    int max_message_size = atoi(argv[4]);
    const char *sender_port_number = argv[5];
    const char *receiver_port_number = argv[6];

    int clientNumber = 0;
    fscanf(clientsFile, "%d", &clientNumber);
    
    int commandNumber = 0;
    fscanf(commandsFile, "%d", &commandNumber);
    
    client *clients = (client *) malloc((clientNumber) * sizeof(client));
    
    createClients(clientsFile, clients, clientNumber, routingFile, sender_port_number, receiver_port_number);
    
    readCommands(commandsFile, commandNumber, clients, max_message_size, clientNumber, sender_port_number, receiver_port_number);
    
    return 0;
}
