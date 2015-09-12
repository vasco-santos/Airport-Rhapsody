/**
 *  \file cam.h (interface file)
 *
 *  \brief Problem name: Airport rhapsody.
 *
 *  \brief Concept: António Rui Borges
 *
 *  CAM management.
 *
 *  The following operations are defined:
 *     \li initialization
 *     \li search for a value
 *     \li insertion of a value
 *     \li retrieval of a value
 *     \li test for CAM full
 *     \li test for CAM empty.
 *
 *  \author António Rui Borges - November 2013
 */

#ifndef CAM_H_
#define CAM_H_

#include <stdbool.h>

#include "probDataStruct.h"

/**
 *  \brief CAM initialization.
 *
 *         CAM will be empty after it.
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 */

extern void camInit (CAM *p_c);

/**
 *  \brief Search for a value in the CAM.
 *
 *         The key is in this case the value itself.
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be searched
 *
 *  \return \c true, if the value is found
 *  \return \c false, otherwise
 */

extern bool camSearch (CAM *p_c, unsigned int val);

/**
 *  \brief Insertion of a value into the CAM.
 *
 *         The key is in this case the value itself to be stored.
 *         The function fails if a null pointer is passed as a parameter or the CAM is full.
 *         Nothing is stored if the function fails.
 *         If an equal value is already stored in the CAM, the new value is stored in FIFO order.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be stored
 */

extern void camIn (CAM *p_c, unsigned int val);

/**
 *  \brief Retrieval of a value from the CAM.
 *
 *         The key is in this case the value itself to be retrieved.
 *         The function fails if a null pointer is passed as a parameter or the CAM is empty or no value is presently
 *         stored there.
 *         Nothing is retrieved if the function fails.
 *         If more than one value is stored in the CAM, the extracted value is retrieved in FIFO order.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *  \param val value to be retrieved
 */

extern void camOut (CAM *p_c, unsigned int val);

/**
 *  \brief Test for CAM full.
 *
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *
 *  \return \c true, if the CAM is full
 *  \return \c false, otherwise
 */

extern bool camFull (CAM *p_c);

/**
 *  \brief Test for CAM empty.
 *
 *         The function fails if a null pointer is passed as a parameter.
 *
 *  \param p_c pointer to the location where the CAM is stored
 *
 *  \return \c true, if the CAM is empty
 *  \return \c false, otherwise
 */

extern bool camEmpty (CAM *p_c);

#endif /* CAM_H_ */
