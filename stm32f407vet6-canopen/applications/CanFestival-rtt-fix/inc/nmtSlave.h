
/** @defgroup nmtslave NMT Slave
 *  @brief The NMT Slave methods are called automatically when a NMT message from Master are received. 
 *  @ingroup networkmanagement
 */
 
#ifndef __nmtSlave_h__
#define __nmtSlave_h__

#include <applicfg.h>
#include "data.h"

/** 
 * @brief Threat the reception of a NMT message from the master.
 * @param *d Pointer to the CAN data structure
 * @param *m Pointer to the message received
 * @return 
 *  -  0 if OK 
 *  - -1 if the slave is not allowed, by its state, to receive the message
 */
void proceedNMTstateChange (CO_Data* d, Message * m);

/** 
 * @brief Transmit the boot-Up frame when the slave is moving from initialization
 * state to pre_operational state.
 * @param *d Pointer on the CAN data structure
 * @return canSend(bus_id,&m)
 */
UNS8 slaveSendBootUp (CO_Data* d);


#endif /* __nmtSlave_h__ */
