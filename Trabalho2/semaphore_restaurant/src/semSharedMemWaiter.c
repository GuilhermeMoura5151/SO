/**
 *  \file semSharedWaiter.c (implementation file)
 *
 *  \brief Problem name: Restaurant
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the waiter:
 *     \li waitForClientOrChef
 *     \li informChef
 *     \li takeFoodToTable
 *
 *  \author Nuno Lau - December 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "probConst.h"
#include "probDataStruct.h"
#include "logging.h"
#include "sharedDataSync.h"
#include "semaphore.h"
#include "sharedMemory.h"

/** \brief logging file name */
static char nFic[51];

/** \brief shared memory block access identifier */
static int shmid;

/** \brief semaphore set access identifier */
static int semgid;

/** \brief pointer to shared memory region */
static SHARED_DATA *sh;

/** \brief waiter waits for next request */
static request waitForClientOrChef ();

/** \brief waiter takes food order to chef */
static void informChef(int group);

/** \brief waiter takes food to table */
static void takeFoodToTable (int group);




/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem: the waiter.
 */
int main (int argc, char *argv[])
{
    int key;                                            /*access key to shared memory and semaphore set */
    char *tinp;                                                       /* numerical parameters test flag */

    /* validation of command line parameters */
    if (argc != 4) { 
        freopen ("error_WT", "a", stderr);
        fprintf (stderr, "Number of parameters is incorrect!\n");
        return EXIT_FAILURE;
    }
    else { 
        freopen (argv[3], "w", stderr);
        setbuf(stderr,NULL);
    }

    strcpy (nFic, argv[1]);
    key = (unsigned int) strtol (argv[2], &tinp, 0);
    if (*tinp != '\0') {
        fprintf (stderr, "Error on the access key communication!\n");
        return EXIT_FAILURE;
    }

    /* connection to the semaphore set and the shared memory region and mapping the shared region onto the
       process address space */
    if ((semgid = semConnect (key)) == -1) { 
        perror ("error on connecting to the semaphore set");
        return EXIT_FAILURE;
    }
    if ((shmid = shmemConnect (key)) == -1) { 
        perror ("error on connecting to the shared memory region");
        return EXIT_FAILURE;
    }
    if (shmemAttach (shmid, (void **) &sh) == -1) { 
        perror ("error on mapping the shared region on the process address space");
        return EXIT_FAILURE;
    }

    /* initialize random generator */
    srandom ((unsigned int) getpid ());              

    /* simulation of the life cycle of the waiter */
    int nReq=0;
    request req;
    while( nReq < sh->fSt.nGroups*2 ) {
        req = waitForClientOrChef();
        switch(req.reqType) {
            case FOODREQ:
                   informChef(req.reqGroup);
                   break;
            case FOODREADY:
                   takeFoodToTable(req.reqGroup);
                   break;
        }
        nReq++;
    }

    /* unmapping the shared region off the process address space */
    if (shmemDettach (sh) == -1) {
        perror ("error on unmapping the shared region off the process address space");
        return EXIT_FAILURE;;
    }

    return EXIT_SUCCESS;
}

/**
 *  \brief waiter waits for next request 
 *
 *  Waiter updates state and waits for request from group or from chef, then reads request.
 *  The waiter should signal that new requests are possible.
 *  The internal state should be saved.
 *
 *  \return request submitted by group or chef
 */
static request waitForClientOrChef() // Waiter fica a' espera de pedidos da parte do CHEF ou GROUP e avisa quando pode receber pedidos
{
    // \brief used by groups to store request to receptionist
    request req; 

    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Waiter passa a esperar por pedidos, WAIT_FOR_REQUEST
    sh->fSt.st.waiterStat = WAIT_FOR_REQUEST;
    saveState(nFic, &sh->fSt);
    
    if (semUp (semgid, sh->mutex) == -1)      {                                             /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Quando o GROUPS ou CHEF estao a fazer as suas funcoes
    // waiterRequest - \brief identification of semaphore used by waiter to wait for requests – val = 0
        if (semDown (semgid, sh->waiterRequest) == -1) 
    {
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // \brief used by groups to store request to receptionist
    req = sh->fSt.waiterRequest; 

    if (semUp (semgid, sh->mutex) == -1) {                                                  /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Avisar que o Receptionist ja pode receber pedidos!

    // waiterRequestPossible - \brief identification of semaphore used by groups and chef to wait before issuing waiter request - val = 1
    if (semUp (semgid, sh->waiterRequestPossible) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }


    return req;

}

/**
 *  \brief waiter takes food order to chef 
 *
 *  Waiter updates state and then takes food request to chef.
 *  Waiter should inform group that request is received.
 *  Waiter should wait for chef receiving request.
 *  The internal state should be saved.
 *
 */
static void informChef (int n) // Waiter avisa ao grupo que o pedido foi recebido e entrega ao pedido ao CHEF e espera por ele
{
    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Waiter passar para o estado de ir informar o chef, INFORM_CHEF
    sh->fSt.st.waiterStat = INFORM_CHEF;
    saveState(nFic, &sh->fSt);

    // Guarda o numero do grupo que pediu, e aponta que tem um pedido de comida
    sh->fSt.foodGroup = n;
    sh->fSt.foodOrder = 1;   
    
    // Tem que saber de que mesa vem o pedido
    int id_mesa = sh->fSt.assignedTable[n];

    if (semUp (semgid, sh->mutex) == -1)                                                   /* exit critical region */
    { perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    
    // TODO insert your code here

    // Notificar ao grupo que o pedido foi recebido

    // requestReceived[id_mesa] - \brief identification of semaphore used by groups to wait for waiter ackowledge – val = 0 
    if (semUp (semgid, sh->requestReceived[id_mesa]) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }

    // O pedido foi entregue ao CHEF

    // waitOrder - \brief identification of semaphore used by chef to wait for order – val = 0
    if (semUp (semgid, sh->waitOrder) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }

    // Waiter fica a espera de uma resposta do CHEF

    // orderReceived - \brief identification of semaphore used by waiter to wait for chef – val = 0
    if (semDown (semgid, sh->orderReceived) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }

}

/**
 *  \brief waiter takes food to table 
 *
 *  Waiter updates its state and takes food to table, allowing the meal to start.
 *  Group must be informed that food is available.
 *  The internal state should be saved.
 *
 */

static void takeFoodToTable (int n) // Waiter leva a comida para a mesa, e o grupo ( depois de saber ) pode comecar a comer
{
    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Waiter passa a levar a comida 'a mesa,  TAKE_TO_TABLE
    sh->fSt.st.waiterStat = TAKE_TO_TABLE;
    saveState(nFic, &sh->fSt);

    // Informa ao grupo da mesa que veio o pedido que ja pode comer

    // foodArrived [sh->fSt.assignedTable[n]] - \brief identification of semaphore used by groups to wait for food – val = 0
    if (semUp (semgid, sh->foodArrived [sh->fSt.assignedTable[n]]) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }
    
    if (semUp (semgid, sh->mutex) == -1)  {                                                  /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }
}
