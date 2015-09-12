/**
 *  \file sharedDataSync.h (interface file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Synchronization based on semaphores and shared memory.
 *  Implementation with SVIPC.
 *
 *  \brief Definition of the shared data and the synchronization devices.
 *
 *  Both the format of the shared data, which represents the full state of the problem, and the identification of
 *  the different semaphores, which carry out the synchronization among the intervening entities, are provided.
 *
 *  \author António Rui Borges - December 2013
 */

#ifndef SHAREDDATASYNC_H_
#define SHAREDDATASYNC_H_

#include "probConst.h"
#include "probDataStruct.h"

/**
 *  \brief Definition of <em>shared information</em> data type.
 */
typedef struct
        { /** \brief full state of the problem */
          FULL_STAT fSt;
          /** \brief identification of critical region semaphore */
          unsigned int access;
          /** \brief identification of porter waiting for work semaphore */
          unsigned int waitingFlight;
          /** \brief number of passengers who have executed the operation whatShouldIDo in each plane landing */
          unsigned int nPassP;
          /** \brief identification of passengers semaphore array (one per passenger) */
          unsigned int pass[N];
          /** \brief array of the number of calls made by the porter to each passenger */
          unsigned int nCalls[N];
          /** \brief identification of bus driver waiting for starting a new journey semaphore */
          unsigned int waitingDrive;
          /** \brief identification of bus driver waiting for passengers to board / unboard semaphore */
          unsigned int waitingPass;
          /** \brief number of passengers who have executed either the operation enterTheBus or
           *  leaveTheBus in each bus transfer
           */
          unsigned int nPassD;
        } SHARED_DATA;

/** \brief number of semaphores in the set */
#define SEM_NU                 (N+4)

/** \brief index of critical region semaphore */
#define ACCESS                     1

/** \brief index of porter waiting for work semaphore */
#define WAITINGFLIGHT              2

/** \brief index of bus driver waiting for starting a new journey semaphore */
#define WAITINGDRIVE               3

/** \brief index of bus driver waiting for passengers to board / unboard semaphore */
#define WAITINGPASS                4

/** \brief base index of passengers semaphore array (one per passenger) */
#define B_PASS                     5

#endif /* SHAREDDATASYNC_H_ */
