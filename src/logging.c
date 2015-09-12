/**
 *  \file logging.c (implementation file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  \brief Logging the problem internal state into a file.
 *
 *  Defined operations:
 *     \li file initialization
 *     \li writing the present state as a single line at the end of the file
 *     \li writing final report.
 *
 *  \author António Rui Borges - November 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "probConst.h"
#include "probDataStruct.h"
#include "queue.h"

/**
 *  \brief File initialization.
 *
 *  The function creates the logging file and writes its header.
 *  If <tt>nFic</tt> is a null pointer or a null string, the file is created under a predefined name <em>log</em>.
 *
 *  The header consists of
 *       \li a line title
 *       \li a blank line
 *       \li a quad line describing the meaning of the different fields of the state lines.
 *
 *  \param nFic name of the logging file
 */

void createLog (char *nFic)
{
  FILE *fic;                                                                                      /* file descriptor */
  char *dName = "log",                                                                      /* default log file name */
       *fName;                                                                                      /* log file name */

  if ((nFic == NULL) || (strcmp (nFic, "") == 0))
     fName = dName;
     else fName = nFic;
  if ((fic = fopen (fName, "w")) == NULL)
     { perror ("error on the creation of log file");
       exit (EXIT_FAILURE);
     }
  fprintf (fic, "%15cAIRPORT RHAPSODY - Description of the internal state of the problem\n\n", ' ');
  fprintf (fic, "PLANE    PORTER                  DRIVER\n");
  fprintf (fic, "FN BN  Stat CB SR   Stat  Q1 Q2 Q3 Q4 Q5 Q6  S1 S2 S3\n");
  fprintf (fic, "%43c              PASSENGERS\n", ' ');
  fprintf (fic, "St1 Si1 NR1 NA1 St2 Si2 NR2 NA2 St3 Si3 NR3 NA3 St4 Si4 NR4 NA4 St5 Si5 NR5 NA5 ");
  fprintf (fic, "St6 Si6 NR6 NA6\n");
  if (fclose (fic) == EOF)
     { perror ("error on closing the log file");
       exit (EXIT_FAILURE);
     }
}

/**
 *  \brief Writing the present full state as a double line at the end of the file.
 *
 *  If <tt>nFic</tt> is a null pointer or a null string, the lines are appended to a file under the predefined
 *  name <em>log</em>.
 *
 *  The following layout is obeyed for the full state in a double line
 *      \li plane, porter, driver, queue and bus state
 *      \li passengers state (p = 0,..., N-1).
 *
 *  \param nFic name of the logging file
 *  \param k flight number
 *  \param p_fSt pointer to the location where the full internal state is stored
 */

void saveState (char *nFic, unsigned int k, FULL_STAT* p_fSt)
{
  FILE *fic;                                                                          /* file descriptor */
  char *dName = "log",                                                          /* default log file name */
       *fName;                                                                          /* log file name */
  unsigned int p, i;                                                               /* counting variables */

  if ((nFic == NULL) || (strcmp (nFic, "") == 0))
     fName = dName;
     else fName = nFic;
  if ((fic = fopen (fName, "a")) == NULL)
     { perror ("error on opening for appending the log file");
       exit (EXIT_FAILURE);
     }
  fprintf (fic, "%2u %2u", k, p_fSt->plHold[k].nBags);
  switch (p_fSt->st.porterStat)
  { case WAITING_FOR_A_PLANE_TO_LAND:  fprintf (fic, "  WPTL ");
                                       break;
    case AT_THE_PLANES_HOLD:           fprintf (fic, "  APHL ");
                                       break;
    case AT_THE_LUGGAGE_BELT_CONVEYOR: fprintf (fic, "  ALBC ");
                                       break;
    case AT_THE_STOREROOM:             fprintf (fic, "  ASTR ");
                                       break;
  }
  fprintf (fic, "%2u %2u", p_fSt->convBelt.n, p_fSt->nToTBagsPSR);
  switch (p_fSt->st.driverStat)
  { case PARKING_AT_THE_ARRIVAL_TERMINAL:   fprintf (fic, "   PAAT ");
                                            break;
    case DRIVING_FORWARD:                   fprintf (fic, "   DRFW ");
                                            break;
    case PARKING_AT_THE_DEPARTURE_TERMINAL: fprintf (fic, "   PADT ");
                                            break;
    case DRIVING_BACKWARD:                  fprintf (fic, "   DRBW ");
                                            break;
  }
  for (i = 0; i < N; i++)
	if (queuePeek (&(p_fSt->busQueue), i) != EMPTYPOS)
	   fprintf (fic, "%3d", queuePeek (&(p_fSt->busQueue), i));
	   else fprintf (fic, "  -");
  fprintf (fic, " ");
  for (i = 0; i < T; i++)
	//if (i < p_fSt->bus.nOccup)
	  if (p_fSt->bus.seat[i] != EMPTYST)
	   fprintf (fic, "%3u", p_fSt->bus.seat[i]);
       else fprintf (fic, "  -");
  fprintf (fic, "\n");
  for (p = 0; p < N; p++)
  { switch (p_fSt->st.passStat[k][p].stat)
    { case AT_THE_DISEMBARKING_ZONE:           fprintf (fic, "ADZ");
                                               break;
      case AT_THE_LUGGAGE_COLLECTION_POINT:    fprintf (fic, "LCP");
                                               break;
      case AT_THE_BAGGAGE_RECLAIM_OFFICE:      fprintf (fic, "BRO");
                                               break;
      case EXITING_THE_ARRIVAL_TERMINAL:       fprintf (fic, "EAT");
                                               break;
      case AT_THE_ARRIVAL_TRANSFER_TERMINAL:   fprintf (fic, "ATT");
                                               break;
      case TERMINAL_TRANSFER:                  fprintf (fic, "TTF");
                                               break;
      case AT_THE_DEPARTURE_TRANSFER_TERMINAL: fprintf (fic, "DTT");
                                               break;
      case ENTERING_THE_DEPARTURE_TERMINAL:    fprintf (fic, "EDT");
                                               break;
    }
    if (p_fSt->st.passStat[k][p].sit == FD)
       fprintf (fic, " FDT");
       else fprintf (fic, " TRT");
    fprintf (fic, "%3u %3u  ", p_fSt->st.passStat[k][p].nBagsReal, p_fSt->st.passStat[k][p].nBagsAct);
  }
  fprintf (fic, "\n");
  if (fclose (fic) == EOF)
     { perror ("error on closing the log file");
       exit (EXIT_FAILURE);
     }
}

/**
 *  \brief Writing the final report.
 *
 *  If <tt>nFic</tt> is a null pointer or a null string, the lines are appended to a file under the predefined
 *  name <em>log</em>.
 *
 *  \param nFic name of the logging file
 *  \param p_fSt pointer to the location where the full internal state is stored
 */

void finalReport (char *nFic, FULL_STAT* p_fSt)
{
  FILE *fic;                                                                          /* file descriptor */
  char *dName = "log",                                                          /* default log file name */
       *fName;                                                                          /* log file name */

  if ((nFic == NULL) || (strcmp (nFic, "") == 0))
     fName = dName;
     else fName = nFic;
  if ((fic = fopen (fName, "a")) == NULL)
     { perror ("error on opening for appending the log file");
       exit (EXIT_FAILURE);
     }
  fprintf (fic, "Final Report\n");
  fprintf (fic, "Number of plane landings: %1u.\n", p_fSt->nLand + 1);
  fprintf (fic, "Total number of pieces of luggage that should have been processed: %2u.\n",
           p_fSt->nToTBagsPCB + p_fSt->nToTBagsPSR + p_fSt->nToTMBags);
  fprintf (fic, "Total number of pieces of luggage that were actually processed: %2u.\n",
           p_fSt->nToTBagsPCB + p_fSt->nToTBagsPSR);
  fprintf (fic, "Total number of passengers that has this airport as their final destination: %2u.\n",
           p_fSt->nToTPassFD);
  fprintf (fic, "Total number of passengers in transit: %2u.\n", p_fSt->nToTPassTST);
  if (fclose (fic) == EOF)
     { perror ("error on closing the log file");
       exit (EXIT_FAILURE);
     }
}
