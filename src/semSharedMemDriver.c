/**
 *  \file semSharedMemDriver.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Definition of the operations carried out by the bus driver thread:
 *     \li has day's work ended
 *     \li announcing bus boarding
 *     \li go to departure terminal
 *     \li park the bus and let passengers off
 *     \li go to arrival terminal
 *     \li park the bus.
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
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

#include "probConst.h"
#include "probDataStruct.h"
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

/** \brief has day's work ended operation */
static bool hasDaysWorkEnded (void);

/** \brief announcing bus boarding operation */
static void announcingBusBoarding (void);

/** \brief go to the departure terminal */
static void goToDepartureTerminal (void);

/** \brief park the bus and let passengers off operation */
static void parkTheBusAndLetPassOff (void);

/** \brief go to the arrival terminal */
static void goToArrivalTerminal (void);

/** \brief park the bus operation */
static void parkTheBus (void);

/** \brief signal service function */
static void alarmCk (int signum);

/**
 *  \brief Main program.
 *
 *  Its role is to generate the life cycle of one of intervening entities in the problem: the porter.
 */

int main (int argc, char *argv[])
{
  int key;                                                           /*access key to shared memory and semaphore set */
  char *tinp;                                                                      /* numerical parameters test flag */

  /* validation of command line parameters */

  if (argc != 4)
     { freopen ("error_GDR", "a", stderr);
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

  /* set the timer */

  struct sigaction act, oact;                                         /* action to be introduced and existing action */
  struct itimerval titv, otitv;                         /* time interval to be introduced and existing time interval */

  act.sa_handler = alarmCk;                                              /* specification of signal service function */
  sigemptyset (&act.sa_mask);                /* no other signal will be blocked during the processing of this signal */
  act.sa_flags = 0;                                                                              /* default behavior */
  if (sigaction (SIGALRM, &act, &oact) != 0)                                                 /* function registering */
     { perror ("error on registering the signal function (DR)");
       return EXIT_FAILURE;
     }
  titv.it_interval.tv_sec = titv.it_value.tv_sec = 0;                                    /* time interval definition */
  titv.it_interval.tv_usec = titv.it_value.tv_usec = 100000;
  if (setitimer (ITIMER_REAL, &titv, &otitv) != 0)
     { perror ("error on registering the time interval for interruption (DR)");
       return EXIT_FAILURE;
     }

  /* simulation of the life cycle of the bus driver */

  while (!hasDaysWorkEnded ())
  { announcingBusBoarding ();             /* the driver invites the passengers forming the queue to board the bus up
                                                     to it is packed full or there is at least one passenger waiting */
    goToDepartureTerminal ();                                  /* the driver takes the bus to the departure terminal */
    parkTheBusAndLetPassOff ();             /* the driver parks the bus at the terminal and let the passengers leave */
    goToArrivalTerminal ();                                 /* the driver takes the bus back to the arrival terminal */
    parkTheBus ();                                                               /* the driver parks at the terminal */
  }

  /* unmapping the shared region off the process address space */

  if (shmemDettach (sh) == -1)
     { perror ("error on unmapping the shared region off the process address space");
       return EXIT_FAILURE;;
     }

  return EXIT_SUCCESS;
}

/**
 *  \brief Has days work ended.
 *
 *  The bus driver keeps waiting for passengers to transfer until his day's work has come to an end. He only proceeds
 *  if his day's work is indeed finished or there are passengers needing to be serviced.
 *
 *  No state should be saved.
 *
 *  \return \c true, if the day has come to the end
 *  \return \c false, if there are passengers needing to be serviced
 */

// REMARK
// when waiting for service or for leaving, use here
//
//     if (semDown (semgid, sh->waitingDrive) == -1)                           /* wait for the right moment to check */
//        { if (errno != EINTR)                     /* check if it is not departure time according to the time table */
//             { perror ("error on the down operation for semaphore waitingDrive (DR)");
//               exit (EXIT_FAILURE);
//             }
//        }

static bool hasDaysWorkEnded (void)
{
	bool retorno= false;
	// if his day's work is indeed finished or there are passengers needing to be serviced
	do{
		/* enter critical region */
		if (semDown (semgid, sh->access) == -1)
		{
			perror ("error on the down operation for semaphore access (DR)");
			exit (EXIT_FAILURE);
		}
		/* insert your code here */
		// Verify if driver day has ended
		if (sh->fSt.dayEnded)
		{
			retorno = true;
		}
		/* exit critical region */
		if (semUp (semgid, sh->access) == -1)
		{
			perror ("error on the up operation for semaphore access (DR)");
			exit (EXIT_FAILURE);
		}
		// If his day's work is indeed finished
		if(retorno)
			break;
	}while(queueEmpty(&sh->fSt.busQueue));
	// If day is not ended -> Sleep Me
	if (!retorno)
	{
		/* wait for the right moment to check */
		if (semDown (semgid, sh->waitingDrive) == -1)
		{
			/* check if it is not departure time according to the time table */
			if (errno != EINTR)
			{
				perror ("error on the down operation for semaphore waitingDrive (DR)");
				exit (EXIT_FAILURE);
			}
		}
	}
	return retorno;
}

/**
 *  \brief Announcing bus boarding.
 *
 *  The bus driver checks if the queue is empty. If it is, he issues an error message.
 *  He then proceeds to summon passengers in the queue to board the bus. If some passenger identity is unknown, he
 *  issues an error message. He finally waits for the boarding to be complete.
 *
 *  No state should be saved.
 */

static void announcingBusBoarding (void)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	unsigned int i,id;

	//The bus driver checks if the queue is empty
	if(queueEmpty(&sh->fSt.busQueue))
	{
		//if it is, he issues an error message
		perror("the bus queue is already empty");
		exit(EXIT_FAILURE);
	}
	//he then procceeds to summon passengers in the queue to board
	for(i=0;i<T;i++)
	{
		// If bus queue not empty
		if (!queueEmpty(&sh->fSt.busQueue))
		{
			// Summon passenger
			queueOut(&sh->fSt.busQueue,&id);
			// Increment nPassD
			sh->nPassD++;
			// Wake up Passenger
			if(id < N)
			{
				if(semUp (semgid, sh->pass[id]) == -1)
				{
					perror ("error on the up operation for semaphore Passenger[i] (PA)");
					exit (EXIT_FAILURE);
				}
			}
			// Unknown Passenger
			else{
				perror("passenger identity unknown");
				exit(EXIT_FAILURE);
			}
		}
	}
	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	//he finally waits for the boarding to be complete
	if (semDown (semgid, sh->waitingPass) == -1)
	{
		perror ("error on the down operation for semaphore waitingPass (DR)");
		exit (EXIT_FAILURE);
	}

}

/**
 *  \brief Go to departure terminal.
 *
 *  The bus driver takes the bus to the departure transfer terminal.
 *
 *  State should be saved.
 */

static void goToDepartureTerminal (void)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	// Change State
	sh->fSt.st.driverStat = DRIVING_FORWARD;
	// Save State
	saveState (nFic, sh->fSt.nLand, &(sh->fSt));

	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Park the bus and let passengers off.
 *
 *  The bus driver checks if the bus is empty or overcrowded. If it is, he issues an error message.
 *  He then proceeds to summon passengers in the bus to exit. If some passenger identity is unknown, he
 *  issues an error message. He finally waits for the exit to be complete.
 *
 *  State should be saved.
 */

static void parkTheBusAndLetPassOff (void)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	int i;
	// Change State
	sh->fSt.st.driverStat = PARKING_AT_THE_DEPARTURE_TERMINAL;

	// the bus driver checks if the bus driver is empty or overcrowded
	if (sh->fSt.bus.nOccup > T || sh->fSt.bus.nOccup == 0)
	{
		perror ("Bus is Empty or OverCrowded");
		exit (EXIT_FAILURE);
	}
	// summon passengers in the bus to exit
	for (i = 0; i < sh->fSt.bus.nOccup; i++)
	{
		if (sh->fSt.bus.seat[i] < N)
		{
			// Increment nPassD
			sh->nPassD++;
			// Wake Up Passenger
			if (semUp (semgid, sh->pass[sh->fSt.bus.seat[i]]) == -1)
			{
				perror ("Error on the up operation for semaphore access (DR)");
				exit (EXIT_FAILURE);
			}
		}
		// Unknown Passenger
		else{
			perror ("Invalid ID");
			exit (EXIT_FAILURE);
		}
	}
	//save
	saveState (nFic, sh->fSt.nLand, &(sh->fSt));
	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	if (semDown (semgid, sh->waitingPass) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Go to arrival terminal.
 *
 *  The bus driver takes the bus back to the arrival transfer terminal.
 *
 *  State should be saved.
 */

static void goToArrivalTerminal (void)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert your code here */
	// Change State
	sh->fSt.st.driverStat = DRIVING_BACKWARD;
	// Save State
	saveState (nFic, sh->fSt.nLand, &(sh->fSt));

	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Park the bus.
 *
 *  The parks the bus at the arrival transfer terminal.
 *
 *  State should be saved.
 */

static void parkTheBus (void)
{
	/* enter critical region */
	if (semDown (semgid, sh->access) == -1)
	{
		perror ("error on the down operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
	/* insert youy code here */
	// Change State
	sh->fSt.st.driverStat = PARKING_AT_THE_ARRIVAL_TERMINAL;
	// Save State
	saveState (nFic, sh->fSt.nLand, &(sh->fSt));

	/* exit critical region */
	if (semUp (semgid, sh->access) == -1)
	{
		perror ("error on the up operation for semaphore access (DR)");
		exit (EXIT_FAILURE);
	}
}

/**
 *  \brief Signal service function.
 *
 *  Inform the bus driver he should check whether it is the right time to start the journey.
 */

static void alarmCk (int signum)
{
  if (signum == SIGALRM)
     { if (semDown (semgid, sh->access) == -1)                                              /* enter critical region */
          { perror ("error on the down operation for semaphore access (DR)");
            exit (EXIT_FAILURE);
          }

       if (semUp (semgid, sh->waitingDrive) == -1)               /* inform the bus driver he should check whether it
                                                                              is the right time to start the journey */
          { perror ("error on the down operation for semaphore waitingDrive (DR)");
            exit (EXIT_FAILURE);
          }

       if (semUp (semgid, sh->access) == -1)                                                 /* exit critical region */
          { perror ("error on the up operation for semaphore access (DR)");
            exit (EXIT_FAILURE);
          }
     }
     else { fprintf (stderr, "a signal other than SIGALRM was received (DR)!\n");
            exit (EXIT_FAILURE);
          }
}
