/**
 *  \file semSharedMemPorter.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the porter process:
 *     \li take a rest
 *     \li try to collect a bag
 *     \li carry it to the appropriate store
 *     \li no more bags to collect.
 *
 *  \author António Rui Borges - December 2013
 *
 *  \developed by
 *  63832 - Miguel Vicente
 *  64191 - Vasco Santos
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

/** \brief take a rest operation */
static void takeARest (unsigned int k);

/** \brief try to collect a bag operation */
static bool tryToCollectABag (unsigned int k, BAG *p_bag);

/** \brief carry it to an appropriate store operation */
static void carryItToAppropriateStore (unsigned int k, BAG *p_bag);

/** \brief no more bags operation */
static void noMoreBagsToCollect (unsigned int k);

/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem: the porter.
 */

int main (int argc, char *argv[])
{
  int key;                                                           /*access key to shared memory and semaphore set */
  char *tinp;                                                                      /* numerical parameters test flag */
  unsigned int k;                                                                                   /* flight number */
  BAG bag;                                                                       /* piece of luggage to be processed */

  /* validation of command line parameters */

  if (argc != 4)
     { freopen ("error_GPO", "a", stderr);
       fprintf (stderr, "Number of parameters is incorrect!\n");
       return EXIT_FAILURE;
     }
     else freopen (argv[3], "w", stderr);
  strcpy (nFic, argv[1]);
  key = (unsigned int) strtol (argv[2], &tinp, 0);
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

  /* simulation of the life cycle of the porter */

  for (k = 0; k < K; k++)
  { takeARest (k);                                                           /* the porter waits for a plane to land */
    while (tryToCollectABag (k, &bag))          /* the porter checks if there is still any luggage to collect at the
                                                                          plane's hold, if it is so he picks a piece */
      carryItToAppropriateStore (k, &bag);                         /* the porter carries it to the appropriate store
                                                                                 (luggage conveyor belt / storeroom) */
    noMoreBagsToCollect (k);                                                /* the porter goes back to the rest room */
  }

  /* unmapping the shared region off the process address space */

  if (shmemDettach (sh) == -1)
     { perror ("error on unmapping the shared region off the process address space");
       return EXIT_FAILURE;;
     }

  return EXIT_SUCCESS;
}

/**
 *  \brief Take a rest.
 *
 *  The porter reads the newspaper while waiting for next assignment.
 *  He waits while not all the N passengers have left the plane which has just landed. He proceeds otherwise.
 *
 *  No state should be saved.
 *
 *  \param k plane landing number
 */

static void takeARest (unsigned int k)
{
	/* insert your code here */
	// wait for the last passenger whatShouldIDo
	if (semDown (semgid, sh->waitingFlight) == -1)
	{
		perror ("error on the down operation for semaphore waitingFlight (PO)");
		exit (EXIT_FAILURE);
	}

	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
	  perror ("error on the down operation for semaphore access (PO)");
	  exit (EXIT_FAILURE);
	}
	/* insert your code here */
	// Number of whatShouldIDo
	sh->nPassP -= N;

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Try to collect a bag.
 *
 *  The porter goes to the plane's hold and checks if there are bags still left to be collected. If so, he picks up one
 *  and leaves. If not, and before leaving, he informs the passengers who may be still waiting that there are no more
 *  bags left.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param p_bag pointer to the location where the retrieved bag should be stored
 *
 *  \return \c true, if he has picked up a bag
 *  \return \c false, otherwise
 */

static bool tryToCollectABag (unsigned int k, BAG *p_bag)
{
	bool ret = false;
	/* Enter Critical Region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	// Any Bag?
	if (sh->fSt.plHold[k].nBags != 0)
	{
		*p_bag = sh->fSt.plHold[k].bag[sh->fSt.plHold[k].nBags - 1]; // save bag
		sh->fSt.plHold[k].nBags--; // Decrease bags number
		// Return var
		ret = true;
	}
	else{
		int i = 0;
		for (i = 0; i < N; i++)
		{
			// Passenger waiting for bags
			if ((sh->fSt.st.passStat[k][i].sit == FD) &&
			          		((sh->fSt.st.passStat[k][i].nBagsAct + sh->nCalls[i]) < sh->fSt.st.passStat[k][i].nBagsReal))
			{
				// Inform Passenger Missing Bags
				if (semUp (semgid, sh->pass[i]) == -1)
				{
					perror ("error on the up operation for semaphore Passenger[i] (PO)");
					exit (EXIT_FAILURE);
				}
				// increase number of calls
				sh->nCalls[i]++;
			}
		}
	}
	/* Change State */
	sh->fSt.st.porterStat = AT_THE_PLANES_HOLD;
	saveState (nFic,k,&(sh->fSt));

	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
	return ret;
}

/**
 *  \brief Carry it to the appropriate store.
 *
 *  The porter checks the bag identification. If it is unknown, he issues an error message.
 *  He then checks the passenger flight situation. If she has this airport as her final destination, he deposits the
 *  bag on the belt conveyor and informs the passenger. Otherwise, he takes the bag to the storeroom for temporary
 *  storage. He also updates statistical data in both cases.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 *  \param p_bag pointer to the location where the retrieved bag should be stored
 */

static void carryItToAppropriateStore (unsigned int k, BAG *p_bag)
{
	if (semDown (semgid, sh->access) == -1)                                                   /* enter critical region */
	{
		perror ("error on the down operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
	// The porter checks the bag identification. If it is unknown, he issues an error message.
	if (p_bag->id > N)
	{
		// Error Message
		perror ("ID Unknown");
		exit (EXIT_FAILURE);
	}
	if (sh->fSt.st.passStat[k][p_bag->id].sit == FD)
	{
		// Update Statistical Data
		sh->fSt.nToTBagsPCB++;
		// Inform Passenger
		sh->nCalls[p_bag->id]++;
		// Update CAM (camIn bag)
		camIn (&sh->fSt.convBelt, p_bag->id);
		// Change State
		sh->fSt.st.porterStat = AT_THE_LUGGAGE_BELT_CONVEYOR;
		saveState (nFic,k,&(sh->fSt));
		// Wake Up Passenger
		if (semUp (semgid, sh->pass[p_bag->id]) == -1)
		{
			perror ("error on the up operation for semaphore Passenger[i] (PO)");
			exit (EXIT_FAILURE);
		}
	}
	// if passenger is in transit
	else if (sh->fSt.st.passStat[k][p_bag->id].sit == TRT)
	{
		// Update Statistical Data
		sh->fSt.nToTBagsPSR++;
		// Change State
		sh->fSt.st.porterStat = AT_THE_STOREROOM;
		saveState (nFic,k,&(sh->fSt));
	}
	// error case
	else
	{
		printf ("Error Situation");
		// exit (EXIT_FAILURE);
	}
	/* Exit Critical Region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief No more bags to collect.
 *
 *  The porter goes back to his office.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 */

static void noMoreBagsToCollect (unsigned int k)
{
	if (semDown (semgid, sh->access) == -1)                                                   /* enter critical region */
	{
		perror ("error on the down operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}

	// Change State
	sh->fSt.st.porterStat = WAITING_FOR_A_PLANE_TO_LAND;
	saveState (nFic,k,&(sh->fSt));

	if (semUp (semgid, sh->access) == -1)                                                      /* exit critical region */
	{
		perror ("error on the up operation for semaphore access (PO)");
		exit (EXIT_FAILURE);
	}
}
