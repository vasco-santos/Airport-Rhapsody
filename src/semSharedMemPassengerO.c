/**
 *  \file semSharedMemPassenger.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the passenger threads:
 *     \li what should I do
 *     \li go collect a bag
 *     \li report missing bags
 *     \li go home
 *     \li take a bus
 *     \li enter the bus
 *     \li leave the bus
 *     \li prepare next leg.
 *
 *  \author António Rui Borges - December 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>

#include "probConst.h"
#include "probDataStruct.h"
#include "cam.h"
#include "queue.h"
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

/** \brief the passenger has this airport as her final destination and has bags to collect */
#define FDBTC          0

/** \brief the passenger has this airport as her final destination with no bags to collect */
#define FDNBTC         1

/** \brief the passenger is in transit */
#define INTRAN         2

/** \brief the passenger has collected all her bags */
#define YES            0

/** \brief the passenger has not yet collected all her bags */
#define NO             1

/** \brief the passenger has missing bags */
#define MB             3

/** \brief what should I do operation */
static unsigned int whatShouldIDo (unsigned int k, unsigned int id);

/** \brief go collect a bag operation */
static unsigned int goCollectABag (unsigned int k, unsigned int id);

/** \brief report missing bags operation */
static void reportMissingBags (unsigned int k, unsigned int id);

/** \brief go home operation */
static void goHome (unsigned int k, unsigned int id);

/** \brief take a bus operation */
static void takeABus (unsigned int k, unsigned int id);

/** \brief enter the bus operation */
static void enterTheBus (unsigned int k, unsigned int id);

/** \brief leave the bus operation */
static void leaveTheBus (unsigned int k, unsigned int id);

/** \brief prepare next leg operation */
static void prepareNextLeg (unsigned int k, unsigned int id);

/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem:
 *  the horse/jockey pair.
 */

int main (int argc, char *argv[])
{
  int key;                                                           /*access key to shared memory and semaphore set */
  char *tinp;                                                                      /* numerical parameters test flag */
  unsigned int k;                                                                                   /* flight number */
  unsigned int p;                                                                        /* passenger identification */
  unsigned int stat;                                                                          /* status of operation */

  /* validation of command line parameters */

  if (argc != 5)
     { freopen ("error_GPA", "a", stderr);
       fprintf (stderr, "Number of parameters is incorrect!\n");
       return EXIT_FAILURE;
     }
     else freopen (argv[4], "w", stderr);
  p = (unsigned int) strtol (argv[1], &tinp, 0);
  if ((*tinp != '\0') || (p >= N))
     { fprintf (stderr, "Passenger process identification is wrong!\n");
       return EXIT_FAILURE;
     }
  strcpy (nFic, argv[2]);
  key = (unsigned int) strtol (argv[3], &tinp, 0);
  if (*tinp != '\0')
     { fprintf (stderr, "Error on the access key communication!\n");
       return EXIT_FAILURE;
     }

  /* connection to the semaphore set and the shared memory region and mapping the shared region onto the
     process address space */

  if ((semgid = semConnect (key)) == -1)
     { perror ("error on connecting to the semaphore set");
       return EXIT_FAILURE;
     }
  if ((shmid = shmemConnect (key)) == -1)
     { perror ("error on connecting to the shared memory region");
       return EXIT_FAILURE;
     }
  if (shmemAttach (shmid, (void **) &sh) == -1)
     { perror ("error on mapping the shared region on the process address space");
       return EXIT_FAILURE;
     }

  /* simulation of the life cycle of the passenger */

  for (k = 0; k < K; k++)
    switch (whatShouldIDo (k, p))                                          /* the passenger decides on her next move */
    { case FDBTC:                                /* she has arrived to her final destination and has bags to collect */
                                /* the passenger goes to the luggage collection point to pick up her bags one by one */
        while ((stat = goCollectABag (k, p)) == NO);
        if (stat == MB)                                                     /* the passenger checks for missing bags */
           reportMissingBags (k, p);          /* the passenger go to the baggage reclaim office to fill the form for
                                                                                                        missing bags */
        goHome (k, p);                                                           /* the passenger leaves the airport */
        break;
      case FDNBTC:                            /* she has arrived to her final destination and has no bags to collect */
        goHome (k, p);                                                           /* the passenger leaves the airport */
        break;
      case INTRAN:                                                                              /* she is in transit */
        takeABus (k, p);                             /* the passenger goes to the arrival transfer terminal to queue
                                                                          for taking a bus to the departure terminal */
        enterTheBus (k, p);                       /* the passenger goes on board of the bus as it starts its journey */
        leaveTheBus (k, p);      /* the passenger comes out of the bus as it reaches the departure transfer terminal */
        prepareNextLeg (k, p);              /* the passenger enters the departure and does the check in for the next
                                                                                                  leg of the journey */
        break;
    }

  /* unmapping the shared region off the process address space */

  if (shmemDettach (sh) == -1)
     { perror ("error on unmapping the shared region off the process address space");
       return EXIT_FAILURE;;
     }

  return EXIT_SUCCESS;
}

/**
 *  \brief What should I do.
 *
 *  The passenger descending from the plane decides what should she do according to her situation (has this airport
 *  as her final destination and has bags to collect / has this airport as her final destination with no bags to
 *  collect / is in transit) and updates statistical information.
 *
 *  If she is the very last passenger descending from the plane, she must inform the porter that a new plane has
 *  landed. If she is the very first passenger, she sets the number of the plane landing.
 *
 *  State should only be saved by the very first passenger descending from the plane.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 *
 *  \return \c FDBTC, if she has this airport as her final destination and has bags to collect
 *  \return \c FDNBTC, if she has this airport as her final destination with no bags to collect
 *  \return \c INTRAN, if she is in
 */

static unsigned int whatShouldIDo (unsigned int k, unsigned int id)
{
	unsigned int stat;
	int i;
	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//if she is in transit
	if(sh->fSt.st.passStat[k][id].sit==TRT){
		//update statistical information
		sh->fSt.nToTPassTST++;
		stat= INTRAN;
	}
	else{
		//if she is in her final destination
		sh->fSt.nToTPassFD++;
		//and no bags to collect
		if(sh->fSt.st.passStat[k][id].nBagsReal==0)
			stat= FDNBTC;
		//or has bags to collect
		else
			stat= FDBTC;
	}

	//if she is the very first passenger descending from the plane
	if(sh->nPassP % N == 0)
	{
		//she sets the number of the plane landing
		sh->fSt.nLand++;
		for (i = 0; i < N; i++)
		{
			sh->fSt.st.passStat[k][i].stat = AT_THE_DISEMBARKING_ZONE;
		}
		//state should only be saved by the first passenger descending from plane
		saveState (nFic,k,&(sh->fSt));
	}
	//update statistical information
	sh->nPassP++;

	//if she is the very last passenger descending
	if(sh->nPassP==N)
		//she wakes up the porter
		if (semUp(semgid, sh->waitingFlight) == -1)
		{
			perror ("error on the down operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	return stat;
}

/**
 *  \brief Go collect a bag.
 *
 *  The passenger waits until she receives a call from the porter. She then checks if there is a bag on the belt
 *  conveyor that belongs to her. If so, she picks it up and updates the number of bags already recovered. If not,
 *  she updates statistical information. Upon exit, a decision is taken about her present situation.
 *
 *  State may be be saved twice. The first of the two only if there is a change of state of the passenger.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 *
 *  \return \c YES, if she has collected all her bags
 *  \return \c NO, if she has not yet collected all her bags
 *  \return \c MB, if she has missing bags
 */

static unsigned int goCollectABag (unsigned int k, unsigned int id)
{
	unsigned int retorno;
	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	if (sh->fSt.st.passStat[k][id].stat == AT_THE_DISEMBARKING_ZONE)
	{
		sh->fSt.st.passStat[k][id].stat = AT_THE_LUGGAGE_COLLECTION_POINT;
		saveState (nFic,k,&(sh->fSt));
	}

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */

	// The passenger waits until she receives a call from the porter.
	if (semDown (semgid, sh->pass[id]) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}

	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//If the belt is empty
	if(camEmpty(&sh->fSt.convBelt))
	{
		//and the passenger doesnt have all her bags
		if(sh->fSt.st.passStat[k][id].nBagsAct < sh->fSt.st.passStat[k][id].nBagsReal)
			//she has missing bags
			retorno = MB;
		else
			//otherwise she has no more bags to collect
			retorno = YES;
	}
	else
	{
		//goCollectBag
		//if there is a bag in the belt
		if(camSearch(&sh->fSt.convBelt,id))
		{
			//she collects it
			camOut(&sh->fSt.convBelt,id);
			sh->fSt.st.passStat[k][id].nBagsAct++;

			if(sh->fSt.st.passStat[k][id].nBagsAct < sh->fSt.st.passStat[k][id].nBagsReal)
				//she has more bags to collect
				retorno = NO;
			else
				//otherwise there are no more bags to collect
				retorno = YES;
		}
		//if there isn't
		else
		{
			//and she doesnt have all her bags
			if(sh->fSt.st.passStat[k][id].nBagsAct < sh->fSt.st.passStat[k][id].nBagsReal)
				//she has missing bags
				retorno = MB;
			else
				//otherwise there are no more bags to collect
				retorno = YES;
		}
	}
	sh->nCalls[id]--;
	saveState (nFic,k,&(sh->fSt));

	/* Exit Critical Region */
	if (semUp(semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	return retorno;
}

/**
 *  \brief Report missing bags.
 *
 *  The passenger fills the form for missing bags.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

static void reportMissingBags (unsigned int k, unsigned int id)
{
	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */

	// Change State
	sh->fSt.st.passStat[k][id].stat = AT_THE_BAGGAGE_RECLAIM_OFFICE;
	saveState (nFic,k,&(sh->fSt));

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Go home.
 *
 *  The passenger exits the airport.
 *
 *  However, before actually doing that, she waits for all other passengers being ready to either exit the airport or
 *  also enter the departure terminal and, if she is the very last passenger of the very last flight, she informs the
 *  bus driver that his day's work is finished.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

void goHome (unsigned int k, unsigned int id)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	int counter=0,i;
	//state change
	sh->fSt.st.passStat[k][id].stat = EXITING_THE_ARRIVAL_TERMINAL;
	saveState (nFic,k,&(sh->fSt));


	//She checks if all passengers are ready leave the airport
	for(i=0; i<N; i++)
	{
		if(sh->fSt.st.passStat[k][i].stat==ENTERING_THE_DEPARTURE_TERMINAL|| sh->fSt.st.passStat[k][i].stat==EXITING_THE_ARRIVAL_TERMINAL)
		{
			counter++;
		}
	}
	//if they are
	if(counter==N)
	{
		for(i=0; i<N; i++)
		{
			if(i!=id)
				//semUp
				if (semUp (semgid, sh->pass[i]) == -1)
				{
					perror ("error on the up operation for semaphore access (PA)");
					exit (EXIT_FAILURE);
				}
		}
		//last passenger of last flight
		if(k==K-1)
		{
			sh->fSt.dayEnded = true;
		}
	}
	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//if not all can leave the airport
	if(counter!=N)
	{
		//she waits
		if (semDown (semgid, sh->pass[id]) == -1)
		{
			perror ("error on the down operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}
	}
}

/**
 *  \brief Take a bus.
 *
 *  The transit passenger queues at the arrival transfer terminal to take a bus to the departure transfer terminal.
 *
 *  However, before waiting her turn, she informs the bus driver it is time to start boarding if the number of queueing
 *  passengers is equal to the number of seats in the bus.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

static void takeABus (unsigned int k, unsigned int id)
{

	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	int i,counter=0;

	//she queues
	queueIn(&sh->fSt.busQueue,id);

	for(i=0;i<T;i++)
	{
		//if the number of queueing passengers
		if(queuePeek(&sh->fSt.busQueue,i) != EMPTYPOS)
		{
			counter++;
		}
	}
	//equals the number os seats in the bus
	if(counter==T)
	{
		//she informs the bus driver it is time start boarding
		if (semUp (semgid, sh->waitingDrive) == -1)
		{
			perror ("error on the up operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}
	}
	// Change State
	sh->fSt.st.passStat[k][id].stat = AT_THE_ARRIVAL_TRANSFER_TERMINAL;
	saveState (nFic,k,&(sh->fSt));

	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//she waits her turn
	if (semDown (semgid, sh->pass[id]) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}

}

/**
 *  \brief Enter the bus.
 *
 *  The passenger enters the bus that it is going to take her to the departure transfer terminal and occupies
 *  an available seat.
 *  If it is already packed full, she issues an error message.
 *  If she is the last one to board the bus for the ride, she informs the driver he may start the journey.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

static void enterTheBus (unsigned int k, unsigned int id)
{
	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	int i;

	if(sh->fSt.bus.nOccup==T)
	{
		perror ("error, the bus is full");
		exit (EXIT_FAILURE);
	}

	//leave the queue and enter the bus
	for(i=0;i<T;i++)
	{
		if(sh->fSt.bus.seat[i]==EMPTYST)
		{
			sh->fSt.bus.seat[i]=id;
			sh->fSt.bus.nOccup++;
			break;
		}
	}
	sh->nPassD--;

	//if she is the last one
	if (sh->nPassD==0)
	{
		//wake up the driver
		if (semUp (semgid, sh->waitingPass) == -1)
		{
			perror ("error on the up operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}
	}
	//save state
	sh->fSt.st.passStat[k][id].stat = TERMINAL_TRANSFER;
	saveState (nFic,k,&(sh->fSt));

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
    /* insert your code here */
	//sleep me
	if (semDown (semgid, sh->pass[id]) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Leave the bus.
 *
 *  The passenger checks the bus occupation. If it is already empty, she issues an error message.
 *  The transit passenger leaves the bus that has taken her to the departure transfer terminal.
 *  If she is the last one to exit, she informs the driver he may start the journey back.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

static void leaveTheBus (unsigned int k, unsigned int id)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	int i;

	//if the bus is already empty
	if(sh->fSt.bus.nOccup==0)
	{
		//she issues an error message
		perror ("error, the bus is already empty");
		exit (EXIT_FAILURE);
	}
	//she leaves the bust
	for(i=0;i<T;i++)
	{
		if(sh->fSt.bus.seat[i] == id)
		{
			sh->fSt.bus.seat[i] = EMPTYST;
			sh->fSt.bus.nOccup--;
			break;
		}
	}
	sh->nPassD++;

	//if she is the last one to exit
	if(sh->fSt.bus.nOccup==0){
		if (semUp (semgid, sh->waitingPass) == -1)
		{
			perror ("error on the up operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}
	}
	//change state
	sh->fSt.st.passStat[k][id].stat = AT_THE_DEPARTURE_TRANSFER_TERMINAL;
	saveState (nFic,k,&(sh->fSt));

	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}

}

/**
 *  \brief Prepare next leg.
 *
 *  The passenger enters the departure terminal.
 *
 *  However, before actually doing that, she waits for all other passengers being ready to either exit the airport or
 *  also enter the departure terminal and, if she is the very last passenger of the very last flight, she informs the
 *  bus driver that his day's work is finished.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param id passenger identification
 */

static void prepareNextLeg (unsigned int k, unsigned int id)
{
	int counter=0,i;
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//state change
	sh->fSt.st.passStat[k][id].stat = ENTERING_THE_DEPARTURE_TERMINAL;
	saveState (nFic,k,&(sh->fSt));

	//She checks if all passengers are ready leave the airport or also enter the departure terminal
	for(i=0; i<N; i++)
	{
		if(sh->fSt.st.passStat[k][i].stat==ENTERING_THE_DEPARTURE_TERMINAL || sh->fSt.st.passStat[k][i].stat==EXITING_THE_ARRIVAL_TERMINAL)
		{
			counter++;
		}
	}
	//if they are
	if(counter==N)
	{
		for(i=0; i<N; i++)
		{
			if(i!=id)
				//semUp
				if (semUp (semgid, sh->pass[i]) == -1)
				{
					perror ("error on the up operation for semaphore access (PA)");
					exit (EXIT_FAILURE);
				}
		}
		//last passenger of last flight
		if(k==K-1)
		{
			sh->fSt.dayEnded = true;
		}
	}

	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PA)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//if not all can leave the airport
	if(counter!=N)
	{
		//she waits
		if (semDown (semgid, sh->pass[id]) == -1)
		{
			perror ("error on the down operation for semaphore access (PA)");
			exit (EXIT_FAILURE);
		}
	}
}
