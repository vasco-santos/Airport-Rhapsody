/**
 *  \file logging.h (interface file)
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

#ifndef LOGGING_H_
#define LOGGING_H_

#include "probConst.h"

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

extern void createLog (char *nFic);

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

extern void saveState (char *nFic, unsigned int k, FULL_STAT* p_fSt);

/**
 *  \brief Writing the final report.
 *
 *  If <tt>nFic</tt> is a null pointer or a null string, the lines are appended to a file under the predefined
 *  name <em>log</em>.
 *
 *  \param nFic name of the logging file
 *  \param p_fSt pointer to the location where the full internal state is stored
 */

extern void finalReport (char *nFic, FULL_STAT* p_fSt);

#endif /* LOGGING_H_ */
