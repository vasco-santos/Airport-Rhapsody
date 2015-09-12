/**
 *  \file semSharedMemPassenger.h (interface file)
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

#ifndef SEMSHAREDMEMPASSENGER_H_
#define SEMSHAREDMEMPASSENGER_H_

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
 *  \return \c INTRAN, if she is in transit
 */

extern unsigned int whatShouldIDo (unsigned int k, unsigned int id);

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

extern unsigned int goCollectABag (unsigned int k, unsigned int id);

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

extern void reportMissingBags (unsigned int k, unsigned int id);

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

extern void goHome (unsigned int k, unsigned int id);

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

extern void takeABus (unsigned int k, unsigned int id);

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

extern void enterTheBus (unsigned int k, unsigned int id);

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

extern void leaveTheBus (unsigned int k, unsigned int id);

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

extern void prepareNextLeg (unsigned int k, unsigned int id);

#endif /* SEMSHAREDMEMPASSENGER_H_ */
