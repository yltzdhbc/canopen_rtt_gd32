
#include "data.h"

#define DCF_STATUS_INIT         0
#define DCF_STATUS_READ_CHECK   1
#define DCF_STATUS_WRITE        2
#define DCF_STATUS_SAVED        3
#define DCF_STATUS_VERIF_OK     4

/** 
 * @brief Init the consise dcf in CO_Data for nodeId
 *
 * @param *d Pointer on a CAN object data structure
 * @param nodeId Id of the slave node
 * @return 1: dcf check started
 *         0: nothing to do   
 */
UNS8 init_consise_dcf(CO_Data* d, UNS8 nodeId);

/**
 * @brief Function to be called from post_SlaveBootup 
 * for starting the configuration manager
 *
 * @param *d Pointer on a CAN object data structure
 * @param nodeId Id of the slave node
 * @return 0: configuration manager busy
 *         1: nothing to check, node started
 *         2: dcf check started
*/
UNS8 check_and_start_node(CO_Data* d, UNS8 nodeId);

