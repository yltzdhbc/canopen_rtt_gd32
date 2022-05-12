
/*!
** @file   emcy.h
** @author HBUT
** @date  
**
** @brief Declarations of the functions that manage EMCY (emergency) messages
**
**
*/
 
/** @defgroup emcyo Emergency Object
 *  Emergency Object is used to communicate device and application failures.
 *  @ingroup comobj
 */
					 
#ifndef __emcy_h__
#define __emcy_h__


#include <applicfg.h>

/* The error states 
 * ----------------- */
typedef enum enum_errorState {
  Error_free		= 0x00, 
  Error_occurred	= 0x01
} e_errorState;

typedef struct {
	UNS16 errCode;
	UNS8 errRegMask;
	UNS8 active;
} s_errors;

#include "data.h"


typedef void (*post_emcy_t)(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
void _post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);

/*************************************************************************
 * Functions
 *************************************************************************/

/** 
 * @ingroup emcy
 * @brief Sets a new error with code errCode. Also sets corresponding bits in Error register (1001h)
 * @param *d Pointer on a CAN object data structure
 * @param errCode The error code
 * @param errRegMask
 * @param addInfo
 * @return
 */
UNS8 EMCY_setError(CO_Data* d, UNS16 errCode, UNS8 errRegMask, UNS16 addInfo);

/**
 * @ingroup emcy 
 * @brief Indicates it has recovered from error errCode. Also clears corresponding bits in Error register (1001h)
 * @param *d Pointer on a CAN object data structure
 * @param errCode The error code
 */
void EMCY_errorRecovered(CO_Data* d, UNS16 errCode);

/**
 * @ingroup emcy 
 * @brief Start EMCY consumer and producer
 * @param *d Pointer on a CAN object data structure
 */
void emergencyInit(CO_Data* d);

/** 
 * @ingroup emcy
 * @brief Stop EMCY producer and consumer
 * @param *d Pointer on a CAN object data structure 
 */
void emergencyStop(CO_Data* d);

/** 
 * @ingroup emcy
 * @brief This function is responsible to process an EMCY canopen-message
 * @param *d Pointer on a CAN object data structure 
 * @param *m Pointer on the CAN-message which has to be analysed.
 */
void proceedEMCY(CO_Data* d, Message* m);

#endif /*__emcy_h__ */
