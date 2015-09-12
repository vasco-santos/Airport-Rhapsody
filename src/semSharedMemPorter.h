/**
 *  \file semSharedMemPorter.h (interface file)
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
 */

#ifndef SEMSHAREDMEMPORTER_H_
#define SEMSHAREDMEMPORTER_H_

#include  <stdbool.h>

#include  "probDataStruct.h"

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

extern takeARest (unsigned int k);

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

extern bool tryToCollectABag (unsigned int k, BAG *p_bag);

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

extern void carryItToAppropriateStore (unsigned int k, BAG *p_bag);

/**
 *  \brief No more bags to collect.
 *
 *  The porter goes back to his office.
 *
 *  State should be saved.
 *
 *  \param k plane landing number
 */

extern void noMoreBagsToCollect (unsigned int k);

#endif /* SEMSHAREDMEMPORTER_H_ */
