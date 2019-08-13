/**
 *  \file semSharedReceptionist.c (implementation file)
 *
 *  \brief Problem name: Restaurant
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the receptionist:
 *     \li waitForGroup
 *     \li provideTableOrWaitingRoom
 *     \li receivePayment
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

/* constants for groupRecord */
#define TOARRIVE 0
#define WAIT     1
#define ATTABLE  2
#define DONE     3

/** \brief receptioninst view on each group evolution (useful to decide table binding) */
static int groupRecord[MAXGROUPS];


/** \brief receptionist waits for next request */
static request waitForGroup ();

/** \brief receptionist waits for next request */
static void provideTableOrWaitingRoom (int n);

/** \brief receptionist receives payment */
static void receivePayment (int n);



/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem: the receptionist.
 */
int main (int argc, char *argv[])
{
    int key;                                            /*access key to shared memory and semaphore set */
    char *tinp;                                                       /* numerical parameters test flag */

    /* validation of command line parameters */
    if (argc != 4) { 
        freopen ("error_RT", "a", stderr);
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

    /* initialize internal receptionist memory */
    int g;
    for (g=0; g < sh->fSt.nGroups; g++) {
       groupRecord[g] = TOARRIVE;
    }

    /* simulation of the life cycle of the receptionist */
    int nReq=0;
    request req;
    while( nReq < sh->fSt.nGroups*2 ) {
        req = waitForGroup();
        switch(req.reqType) {
            case TABLEREQ:
                   provideTableOrWaitingRoom(req.reqGroup); //TODO param should be groupid
                   break;
            case BILLREQ:
                   receivePayment(req.reqGroup);
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
 *  \brief decides table to occupy for group n or if it must wait.
 *
 *  Checks current state of tables and groups in order to decide table or wait.
 *
 *  \return table id or -1 (in case of wait decision)
 */
static int decideTableOrWait(int n) // Associa grupo a uma mesa, mas ter atencao as mesas disponiveis
{
     //TODO insert your code here

    int id_mesa = 0;
    int num_tables = 0;

    // Atribui mesa ou mete em espera aos grupos ( duas mesas )
    for (int i = 0; i < MAXGROUPS; i++) 
     { 
         // Tentar dar mesa ao grupo - em sucesso associa mesa ao grupo
         if(sh->fSt.assignedTable[i] == 1)
        {   
            if (num_tables == 0)
            {
                id_mesa = 0;
                num_tables ++;
            }
            else
                // espera
                id_mesa = -1;
            
        }
        else if (sh->fSt.assignedTable[i] == 0)
        {
            if (num_tables == 0)
            {
                id_mesa = 1;
                num_tables ++;
            }
            else
            // espera
                id_mesa = -1;
        }
        
     }

    // retorna o valor da mesa ou -1 caso esteja em espera
    // Guardar grupos por ordem para mais tarde associar mesa que ficar livre
     if (id_mesa != -1)
     {
         groupRecord[n] = ATTABLE;
         return id_mesa;
     } 
    else
    {
         groupRecord[n] = WAIT;
         sh->fSt.groupsWaiting = sh->fSt.groupsWaiting + 1;
         return -1;
    }

     return -1;
}

/**
 *  \brief called when a table gets vacant and there are waiting groups 
 *         to decide which group (if any) should occupy it.
 *
 *  Checks current state of tables and groups in order to decide group.
 *
 *  \return group id or -1 (in case of wait decision)
 */
static int decideNextGroup() // Decidir mesa para o grupo que estiver a seguir - So acontece com grupos em WAITING
{
     //TODO insert your code here

     if (sh->fSt.groupsWaiting != 0)
     {
        for (int i = 0; i < MAXGROUPS; i++)
        {
             if (groupRecord[i] == WAIT)
             {   
                 // Passar grupo para a mesa e tirar da lista de espera
                 groupRecord[i] = ATTABLE;
                 sh->fSt.groupsWaiting = sh->fSt.groupsWaiting - 1;

                 return i;
            }
        }
        return -1;
     }
    

     return -1;
}

/**
 *  \brief receptionist waits for next request 
 *
 *  Receptionist updates state and waits for request from group, then reads request,
 *  and signals availability for new request.
 *  The internal state should be saved.
 *
 *  \return request submitted by group
 */
static request waitForGroup() // Receptionist espera por pedidos dos grupos
{
    // \brief used by groups to store request to receptionist
    request ret; 

    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Receptionist fica a espera que facam pedidos
    sh->fSt.st.receptionistStat = WAIT_FOR_REQUEST;
    saveState(nFic, &sh->fSt);

    
    if (semUp (semgid, sh->mutex) == -1)      {                                             /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Receptionist fica a espera dos pedidos do grupo
    // receptionistReq - brief identification of semaphore used by receptionist to wait for groups - val = 0
    if (semDown (semgid, sh->receptionistReq) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }


    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // \brief used by groups to store request to receptionist
    ret = sh->fSt.receptionistRequest; 

    if (semUp (semgid, sh->mutex) == -1) {                                                  /* exit critical region */
     perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // receptionistRequestPossible - brief identification of semaphore used by groups to wait before issuing receptionist request - val = 1
    if (semUp (semgid, sh->receptionistRequestPossible) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }

    return ret;

}

/**
 *  \brief receptionist decides if group should occupy table or wait
 *
 *  Receptionist updates state and then decides if group occupies table
 *  or waits. Shared (and internal) memory may need to be updated.
 *  If group occupies table, it must be informed that it may proceed. 
 *  The internal state should be saved.
 *
 */
static void provideTableOrWaitingRoom (int n) // Receptionist mete grupo na mesa ou em espera. Grupo e' informado se tiver mesa
{
    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Receptionist passa ao estado de ASSIGNTABLE, associar mesas
    sh->fSt.st.receptionistStat = ASSIGNTABLE;
    saveState(nFic, &sh->fSt);

    // Ver as mesas e verificar se entrega mesa ou se mete na lista de espera. Se existe mesa avisa o grupo
    int table_id = decideTableOrWait(n);
    if (table_id != -1)
    {
        sh->fSt.assignedTable[n] = table_id;

        // waitForTable[n] - brief identification of semaphore used by groups to wait for table – val = 0
        if (semUp (semgid, sh->waitForTable[n]) == -1)
        {
            perror ("error on the up operation for semaphore access (PT)");
            exit (EXIT_FAILURE);
        }
    }

    if (semUp (semgid, sh->mutex) == -1) {                                               /* exit critical region */
        perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

}

/**
 *  \brief receptionist receives payment 
 *
 *  Receptionist updates its state and receives payment.
 *  If there are waiting groups, receptionist should check if table that just became
 *  vacant should be occupied. Shared (and internal) memory should be updated.
 *  The internal state should be saved.
 *
 */

static void receivePayment (int n) // Receptionist recebe pagamento, de seguida se ha grupos em espera mete os na mesa que fica livre
{
    if (semDown (semgid, sh->mutex) == -1)  {                                                  /* enter critical region */
        perror ("error on the up operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Receptionist passa para o estado RECVPAY
    sh->fSt.st.receptionistStat = RECVPAY;
    saveState(nFic, &sh->fSt);

    // Verificar a mesa que ficou livre
    int id_mesa = sh->fSt.assignedTable[n];

    int group_id = decideNextGroup();

    if (group_id != -1)
    {
         // Grupo seguinte fica com a mesa livre
         sh->fSt.assignedTable[group_id] = id_mesa;
         groupRecord[n] = DONE;
    
         // waitForTable[group_id] - \brief identification of semaphore used by groups to wait for table – val = 0 
        if (semUp (semgid, sh->waitForTable[group_id]) == -1)
        {
            perror ("error on the up operation for semaphore access (PT)");
            exit (EXIT_FAILURE);
        }
  
    }

    sh->fSt.assignedTable[n] = -1;

    if (semUp (semgid, sh->mutex) == -1)  {                                                  /* exit critical region */
     perror ("error on the down operation for semaphore access (WT)");
        exit (EXIT_FAILURE);
    }

    // TODO insert your code here

    // Nao esquecer de avisar os grupos que estao a' espera do pagamento!

    // tableDone[id_mesa] - \brief identification of semaphore used by groups to wait for payment completed – val = 0
    if (semUp (semgid, sh->tableDone[id_mesa]) == -1)
    {
        perror ("error on the up operation for semaphore access (PT)");
        exit (EXIT_FAILURE);
    }
}

