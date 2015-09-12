/**
 *  \file semSharedMemDriver.h (interface file)
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
 */

#ifndef SEMSHAREDMEMDRIVER_H_
#define SEMSHAREDMEMDRIVER_H_

#include  <stdbool.h>

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

extern bool hasDaysWorkEnded (void);

/**
 *  \brief Announcing bus boarding.
 *
 *  The bus driver checks if the queue is empty. If it is, he issues an error message.
 *  He then proceeds to summon passengers in the queue to board the bus. If some passenger identity is unknown, he
 *  issues an error message. He finally waits for the boarding to be complete.
 *
 *  No state should be saved.
 */

extern void announcingBusBoarding (void);

/**
 *  \brief Go to departure terminal.
 *
 *  The bus driver takes the bus to the departure transfer terminal.
 *
 *  State should be saved.
 */

extern void goToDepartureTerminal (void);

/**
 *  \brief Park the bus and let passengers off.
 *
 *  The bus driver checks if the bus is empty or overcrowded. If it is, he issues an error message.
 *  He then proceeds to summon passengers in the bus to exit. If some passenger identity is unknown, he
 *  issues an error message. He finally waits for the exit to be complete.
 *
 *  State should be saved.
 */

extern void parkTheBusAndLetPassOff (void);

/**
 *  \brief Go to arrival terminal.
 *
 *  The bus driver takes the bus back to the arrival transfer terminal.
 *
 *  State should be saved.
 */

extern void goToArrivalTerminal (void);

/**
 *  \brief Park the bus.
 *
 *  The parks the bus at the arrival transfer terminal.
 *
 *  State should be saved.
 */

extern void parkTheBus (void);

#endif /* SEMSHAREDMEMDRIVER_H_ */
