/**
 *  \file probConst.h (interface file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  Problem simulation parameters.
 *
 *  \author António Rui Borges - November 2013
 */

#ifndef PROBCONST_H_
#define PROBCONST_H_

/* Generic parameters */

/** \brief number of plane landings */
#define  K           5
/** \brief number of passengers per flight */
#define  N           6
/** \brief maximum number of pieces of luggage per passenger */
#define  M           2
/** \brief number of seats in the bus */
#define  T           3

/* Porter state constants */

/** \brief the porter is reading a newspaper while waiting for next assignment */
#define  WAITING_FOR_A_PLANE_TO_LAND         0
/** \brief the porter is at the plane's hold looking for bags */
#define  AT_THE_PLANES_HOLD                  1
/** \brief the porter is depositing the bag retrieved from the plane's hold on the belt conveyor */
#define  AT_THE_LUGGAGE_BELT_CONVEYOR        2
/** \brief the porter is placing the bag retrieved from the plane's hold in temporary storage */
#define  AT_THE_STOREROOM                    3

/* Passenger state constants */

/** \brief the passenger is coming out of the plane after landing */
#define  AT_THE_DISEMBARKING_ZONE            0
/** \brief the passenger is waiting at the luggage collection point to take possession of her baggages */
#define  AT_THE_LUGGAGE_COLLECTION_POINT     1
/** \brief the passenger is at the baggage reclaim office filling a form for missing bags */
#define  AT_THE_BAGGAGE_RECLAIM_OFFICE       2
/** \brief the passenger is exiting the airport after the flight to go to her final destination */
#define  EXITING_THE_ARRIVAL_TERMINAL        3
/** \brief the passenger is queueing at the arrival transfer terminal to take a bus to the departure terminal */
#define  AT_THE_ARRIVAL_TRANSFER_TERMINAL    4
/** \brief the passenger is traveling in the bus between transfer terminals */
#define  TERMINAL_TRANSFER                   5
/** \brief the passenger is leaving the bus at the departure transfer terminal */
#define  AT_THE_DEPARTURE_TRANSFER_TERMINAL  6
/** \brief the passenger is entering the departure terminal lobby to do the check-in for the next leg of the journey */
#define  ENTERING_THE_DEPARTURE_TERMINAL     7

/* Bus driver state constants */

/** \brief the driver has parked at the arrival transfer terminal waiting for passengers to transport */
#define  PARKING_AT_THE_ARRIVAL_TERMINAL     0
/** \brief the driver is taking the bus to the departure transfer terminal */
#define  DRIVING_FORWARD                     1
/** \brief the driver has parked at the departure transfer terminal to let the passengers off the bus */
#define  PARKING_AT_THE_DEPARTURE_TERMINAL   2
/** \brief the driver is taking the bus back to the arrival transfer terminal */
#define  DRIVING_BACKWARD                    3

#endif /* PROBCONST_H_ */
