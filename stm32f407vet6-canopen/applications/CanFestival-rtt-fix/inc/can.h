
#ifndef __can_h__
#define __can_h__

#include "applicfg.h"

/** 
 * @brief The CAN message structure 
 * @ingroup can
 */
typedef struct {
  UNS16 cob_id;	/**< message's ID */
  UNS8 rtr;		/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
  UNS8 len;		/**< message's length (0 to 8) */
  UNS8 data[8]; /**< message's datas */
} Message;

#define Message_Initializer {0,0,0,{0,0,0,0,0,0,0,0}}

typedef UNS8 (*canSend_t)(Message *);

#endif /* __can_h__ */
