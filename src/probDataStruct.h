/**
 *  \file probDataStruct.h (interface file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Definition of internal data structures.
 *
 *  They specify internal metadata about the status of the intervening entities.
 *
 *  \author António Rui Borges - November 2013
 */

#ifndef PROBDATASTRUCT_H_
#define PROBDATASTRUCT_H_

#include <stdbool.h>

#include "probConst.h"

/**
 *  \brief Definition of <em>state of the passenger</em> data type.
 */
typedef struct
        { /** \brief internal state */
          unsigned int stat;
          /** \brief present situation (final destination / in transit) */
          unsigned int sit;
          /** \brief number of pieces of luggage she is supposed to be carrying */
          unsigned int nBagsReal;
          /** \brief number of pieces of luggage she is really carrying */
          unsigned int nBagsAct;
        } STAT_PASSENGER;

/** \brief passenger has this airport as her final destination */
#define  FD           0
/** \brief passenger is in transit */
#define  TRT          1

/**
 *  \brief Definition of <em>state of the intervening entities</em> data type.
 */
typedef struct
        { /** \brief state of the porter */
          unsigned int porterStat;
          /** \brief state array of the passengers */
          STAT_PASSENGER passStat[K][N];
          /** \brief state of the bus driver */
          unsigned int driverStat;
        } STAT;

/**
 *  \brief Definition of <em>piece of luggage</em> data type.
 */
typedef struct
        { /** \brief passenger identification */
          unsigned int id;
        } BAG;

/**
 *  \brief Definition of <em>plane load</em> data type.
 */
typedef struct
        { /** \brief number of pieces of luggage in the plane's hold */
          unsigned int nBags;
          /** \brief plane's hold contents */
          BAG bag[M*N];
        } LOAD;

/**
 *  \brief Definition of <em>cam of bags</em> data type.
 */
typedef struct
        { /** \brief storage region */
          BAG mem[M*N];
          /** \brief number of bags presently stored */
          unsigned int n;
        } CAM;

/**
 *  \brief Definition of <em>queue of identification</em> data type.
 */
typedef struct
        { /** \brief storage region */
          unsigned int mem[N];
          /** \brief insertion pointer */
          unsigned int ii;
          /** \brief retrieval pointer */
          unsigned int ri;
          /** \brief flag signaling that the queue is full */
          bool full;
        } QUEUE;

/** \brief queue position is empty */
#define  EMPTYPOS    -1

/**
 *  \brief Definition of <em>participant information for a transfer data</em> data type.
 */
typedef struct
        { /** \brief state of occupation of the seats in the bus (empty / identification of the passenger) */
          int seat[T];
          /** \brief number of seats presently occupied */
          unsigned int nOccup;
        } TRANSF_INFO;

/** \brief seat is empty */
#define  EMPTYST     -1

/**
 *  \brief Definition of <em>full state of the problem</em> data type.
 */
typedef struct
        { /** \brief plane landing number */
          unsigned int nLand;;
          /** \brief state of the intervening entities */
          STAT st;
          /** \brief array of manifests for the planes' hold */
          LOAD plHold[K];
          /** \brief luggage conveyor belt */
          CAM convBelt;
          /** \brief queue for the transfer ride */
          QUEUE busQueue;
          /** \brief bus occupation data */
          TRANSF_INFO bus;
          /** \brief total number of passengers for whom the airport was their final destination */
          unsigned int nToTPassFD;
          /** \brief total number of passengers in transit */
          unsigned int nToTPassTST;
          /** \brief total number of bags placed in the belt conveyor */
          unsigned int nToTBagsPCB;
          /** \brief total number of bags placed in the storeroom */
          unsigned int nToTBagsPSR;
          /** \brief total number of missing bags */
          unsigned int nToTMBags;
          /** \brief driver day's work has ended */
          bool dayEnded;
        } FULL_STAT;

#endif /* PROBDATASTRUCT_H_ */
