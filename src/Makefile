CC = gcc
CFLAGS = -Wall -D_SVID_SOURCE
OBJS = sharedMemory.o semaphore.o cam.o queue.o logging.o


all:		startClean probSemSharedMemAirportRhapsody semSharedMemPorter semSharedMemPassenger \
		semSharedMemDriver endClean

allPO:		startClean probSemSharedMemAirportRhapsody semSharedMemPorter semSharedMemPassengerBin \
		semSharedMemDriverBin endClean

allPA:		startClean probSemSharedMemAirportRhapsody semSharedMemPorterBin semSharedMemPassenger \
		semSharedMemDriverBin endClean

allBin:		startClean probSemSharedMemAirportRhapsody semSharedMemPorterBin semSharedMemPassengerBin \
		semSharedMemDriverBin endClean

probSemSharedMemAirportRhapsody:	probSemSharedMemAirportRhapsody.o $(OBJS)
					$(CC) -o $@ $^ -lm
					mv probSemSharedMemAirportRhapsody ../run/probSemSharedMemAirportRhapsody

semSharedMemPorter:			semSharedMemPorter.o $(OBJS)
					$(CC) -o $@ $^ -lm
					mv semSharedMemPorter ../run/porter

semSharedMemPorterBin:
					cp porterBin ../run/porter

semSharedMemPassenger:			semSharedMemPassenger.o $(OBJS)
					$(CC) -o $@ $^ -lm
					mv semSharedMemPassenger ../run/passenger

semSharedMemPassengerBin:
					cp passengerBin ../run/passenger

semSharedMemDriver:				semSharedMemDriver.o $(OBJS)
					$(CC) -o $@ $^ -lm
					mv semSharedMemDriver ../run/driver

semSharedMemDriverBin:
					cp driverBin ../run/driver

startClean:
		rm -f *.o probSemSharedMemAirportRhapsody semSharedMemDriver semSharedMemPassenger semSharedMemPorter
		rm -f ../run/probSemSharedMemAirportRhapsody ../run/driver ../run/passenger \
			../run/porter ../run/error*

endClean:
		rm -f *.o
