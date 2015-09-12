/**
 *  \file probSemSharedMemAirportRhapsody.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  Generator process of the intervening entities.
 *
 *  Upon execution, one parameter is requested:
 *    \li name of the logging file.
 *
 *  \author António Rui Borges - December 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
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

/** \brief name of the porter process */
#define   PORTER          "./porter"

/** \brief name of the passenger process */
#define   PASSENGER       "./passenger"

/** \brief name of the bus driver process */
#define   DRIVER         "./driver"

/**
 *  \brief Main program.
 *
 *  Its role is starting the simulation by generating the intervening entities processes (porter, passengers and bus
 *  driver) and waiting for their termination.
 */

int main (int argc, char *argv[])
{
  char nFic[51];                                                                              /*name of logging file */
  char nFicErr[12];                                                                           /* name of error files */
  FILE *fic;                                                                                      /* file descriptor */
  int shmid,                                                                      /* shared memory access identifier */
      semgid;                                                                     /* semaphore set access identifier */
  int t;                                                                               /* keyboard reading test flag */
  char opt;                                                                                                /* answer */
  unsigned int k, p, i, j, n;                                                                  /* counting variables */
  SHARED_DATA *sh;                                                                /* pointer to shared memory region */
  unsigned int nTot;                                                                        /* plane load per flight */
  int pid[2+N];                                                                             /* processes identifiers */
  int key;                                                           /*access key to shared memory and semaphore set */
  char num[2][12];                                                     /* numeric value conversion (up to 10 digits) */
  int status,                                                                                    /* execution status */
      info;                                                                                   /* info identification */
  bool term;                                                                             /* process termination flag */

  /* getting logging file name */

  do
  { do
    { printf ("\nLog file name? ");
      t = scanf ("%20[^\n]", nFic);
      scanf ("%*[^\n]");
      scanf ("%*c");
    } while (t == 0);
    fic = fopen (nFic, "r");
    if (fic != NULL)
       { fclose (fic);
         printf ("There is already a file with this name! ");
         do
         { printf ("Overwrite? ");
           scanf ("%c", &opt);
           if (opt != '\n')
              { scanf ("%*[^\n]");
                scanf ("%*c");
              }
         } while ((opt == '\n') || ((opt != 'Y') && (opt != 'y') && (opt != 'N') && (opt != 'n')));
         if ((opt == 'Y') || (opt == 'y')) break;
       }
  } while (fic != NULL);

  /* composing command line */

  sprintf (num[0], "%s", "0");
  if ((key = ftok (".", 'a')) == -1)
     { perror ("error on generating the key");
       exit (EXIT_FAILURE);
     }
  sprintf (num[1], "%d", key);

  /* creating and initializing the shared memory region and the logging file */

  if ((shmid = shmemCreate (key, sizeof (SHARED_DATA))) == -1)
     { perror ("error on creating the shared memory region");
       return EXIT_FAILURE;
     }
  if (shmemAttach (shmid, (void **) &sh) == -1)
     { perror ("error on mapping the shared region on the process address space");
       return EXIT_FAILURE;
     }

  srandom ((unsigned int) getpid ());                                          /* initialize random number generator */

  sh->fSt.nLand = 0;                                                                     /* initialize plane landing */
  sh->fSt.st.porterStat = WAITING_FOR_A_PLANE_TO_LAND;        /* the porter is reading a newspaper while waiting for
                                                                                                     next assignment */
  for (k = 0; k < K; k++)
  { nTot = 0;                                                                    /* initialize plane load per flight */
    for (p = 0; p < N; p++)
    { sh->fSt.st.passStat[k][p].stat = AT_THE_DISEMBARKING_ZONE;         /* the passenger is coming out of the plane
                                                                                                       after landing */
      if (((unsigned int) floor (9.0*random ()/RAND_MAX+1.5)) < 4)
         sh->fSt.st.passStat[k][p].sit = TRT;                                         /* the passenger is in transit */
         else sh->fSt.st.passStat[k][p].sit = FD;         /* the passenger has this airport as her final destination */
                                                       /* number of pieces of luggage she is supposed to be carrying */
      sh->fSt.st.passStat[k][p].nBagsReal = (unsigned int) floor (((double) M)*random ()/RAND_MAX+0.5);
                                                             /* number of pieces of luggage she is actually carrying */
      if ((sh->fSt.st.passStat[k][p].sit == TRT) || (sh->fSt.st.passStat[k][p].nBagsReal == 0))
         sh->fSt.st.passStat[k][p].nBagsAct = sh->fSt.st.passStat[k][p].nBagsReal;            /* no need to consider
                                                                                                        missing bags */
         else if (((unsigned int) floor (9.0*random ()/RAND_MAX+1.5)) < 5)
                                                                                   /* the passenger has lost one bag */
                 sh->fSt.st.passStat[k][p].nBagsAct = sh->fSt.st.passStat[k][p].nBagsReal - 1;
                 else sh->fSt.st.passStat[k][p].nBagsAct = sh->fSt.st.passStat[k][p].nBagsReal;     /* the passenger
                                                                                                          has no bag */
      nTot += sh->fSt.st.passStat[k][p].nBagsAct;                                               /* update plane load */
    }
    sh->fSt.plHold[k].nBags = nTot;                                             /* set number of bags for plane load */
    i = 0;
    while (i < nTot)                                                                           /* fill in plane load */
    { for (p = 0; p < N; p++)
      { for (j = 0; j < sh->fSt.st.passStat[k][p].nBagsAct; j++, i++)
          sh->fSt.plHold[k].bag[i].id = p;
        sh->fSt.st.passStat[k][p].nBagsAct = 0;                /* reset number of pieces of luggage the passenger is
                                                                                                   actually carrying */
      }
    }
  }
  sh->fSt.st.driverStat = PARKING_AT_THE_ARRIVAL_TERMINAL;          /* the driver has parked at the arrival transfer
                                                                        terminal waiting for passengers to transport */
  camInit (&(sh->fSt.convBelt));                                                 /* set conveyor belt to empty state */
  queueInit (&(sh->fSt.busQueue));                                                   /* set bus queue to empty state */
  sh->fSt.bus.nOccup = 0;                                                               /* initialize bus occupation */
  for (i = 0; i < T; i++)
    sh->fSt.bus.seat[i] = EMPTYST;                                                             /* list seat as empty */
  sh->fSt.nToTPassFD = 0;  /* initialize total number of passengers for whom the airport was their final destination */
  sh->fSt.nToTPassTST = 0;                                       /* initialize total number of passengers in transit */
  sh->fSt.nToTBagsPCB = 0;                            /* initialize total number of bags placed in the belt conveyor */
  sh->fSt.nToTBagsPSR = 0;                                /* initialize total number of bags placed in the storeroom */
  sh->fSt.nToTMBags = 0;                                                  /* initialize total number of missing bags */
  sh->fSt.dayEnded = false;                                           /* initialize flag signaling driver day's work */

  sh->nPassP = 0;                   /* initialize number of passengers who have executed the operation whatShouldIDo
                                                                                               in each plane landing */
  for (p = 0; p< N; p++)
    sh->nCalls[p] = 0;                            /* initialize number of calls made by the porter to each passenger */
  sh->nPassD = 0;              /* initialize number of passengers who have executed either the operation enterTheBus
                                                                                 or leaveTheBus in each bus transfer */

  sh->access = ACCESS;                                                /* identification of critical region semaphore */
  sh->waitingFlight = WAITINGFLIGHT;                          /* identification of porter waiting for work semaphore */
  sh->waitingDrive = WAITINGDRIVE;      /* identification of bus driver waiting for starting a new journey semaphore */
  sh->waitingPass = WAITINGPASS; /* identification of bus driver waiting for passengers to board / unboard semaphore */
  for (p = 0; p < N; p++)
    sh->pass[p] = B_PASS + p;                                               /* identification of passenger semaphore */

  createLog (nFic);                                                                       /* create the logging file */

  /* creating and initializing the semaphore set (all semaphores but the critical region one are set to red state) */

  if ((semgid = semCreate (key, SEM_NU)) == -1)
     { perror ("error on creating the semaphore set");
       return EXIT_FAILURE;
     }
  if (semUp (semgid, sh->access) == -1)                                          /* enable access to critical region */
     { perror ("error on the up operation for semaphore access");
       return EXIT_FAILURE;
     }

  /* generating the intervening entities processes */

  if ((pid[0] = fork ()) < 0)
     { perror ("error on the fork operation for the porter");
       return EXIT_FAILURE;
     }
  strcpy (nFicErr, "error_PO");
  if (pid[0] == 0)
     if (execl (PORTER, PORTER, nFic, num[1], nFicErr, NULL) < 0)
          { perror ("error on the generation of the porter process");
            return EXIT_FAILURE;
          }

  if ((pid[1] = fork ()) < 0)
     { perror ("error on the fork operation for the bus driver");
       return EXIT_FAILURE;
     }
  strcpy (nFicErr, "error_DR");
  if (pid[1] == 0)
     if (execl (DRIVER, DRIVER, nFic, num[1], nFicErr, NULL) < 0)
          { perror ("error on the generation of the bus driver process");
            return EXIT_FAILURE;
          }

  strcpy (nFicErr, "error_PA_");
  for (p = 2; p < N+2; p++)
  { if ((pid[p] = fork ()) < 0)
       { perror ("error on the fork operation for the passenger");
         return EXIT_FAILURE;
       }
    num[0][0] = '0' + p - 2;
    nFicErr[8] = '0' + p - 2;
    if (pid[p] == 0)
       if (execl (PASSENGER, PASSENGER, num[0], nFic, num[1], nFicErr, NULL) < 0)
          { perror ("error on the generation of the passenger process");
            return EXIT_FAILURE;
          }
  }

  /* signal start of operations */

  if (semSignal (semgid) == -1)
     { perror ("error on signaling start of operations");
       return EXIT_FAILURE;
     }

  /* wait for the termination of the thieves processes */

  printf ("\nFinal report\n");
  n = 0;
  do
  { info = wait (&status);
    term = false;
    for (i = 0; i < 2+N; i++)
      if (info == pid[i])
         { term = true;
           break;
         }
    if (!term)
       { perror ("error on waiting for an intervening process");
         return EXIT_FAILURE;
       }
    if (i == 0)
       printf ("porter process has terminated: ");
       else if (i == 1)
               printf ("bus driver process has terminated: ");
               else printf ("passenger process, with id %u, has terminated: ", i-2);
    if (WIFEXITED (status))
       printf ("its status was %d\n", WEXITSTATUS (status));
    n += 1;
  } while (n < (2+N));

  /* print final report */

  finalReport (nFic, &(sh->fSt));

  /* destroy the semaphore set and the shared region */

  if (semDestroy (semgid) == -1)
     { perror ("error on destroying the semaphore set");
       return EXIT_FAILURE;
     }
  if (shmemDettach (sh) == -1)
     { perror ("error on unmapping the shared region off the process address space");
       return EXIT_FAILURE;
     }
  if (shmemDestroy (shmid) == -1)
     { perror ("error on destroying the shared region");
       return EXIT_FAILURE;
     }

  return EXIT_SUCCESS;
}
