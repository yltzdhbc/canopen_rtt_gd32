#ifndef __can_driver_h__
#define __can_driver_h__

struct struct_s_BOARD;

typedef struct struct_s_BOARD s_BOARD;

#include "applicfg.h"
#include "can.h"

/**
 * @brief The CAN board configuration
 * @ingroup can
 */

//struct struct_s_BOARD {
//  char busname[100]; /**< The bus name on which the CAN board is connected */
//  char baudrate[4]; /**< The board baudrate */
//};

struct struct_s_BOARD {
  char * busname;  /**< The bus name on which the CAN board is connected */
  char * baudrate; /**< The board baudrate */
};

#ifndef DLL_CALL
#if !defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#define LIBAPI
#define DLL_CALL(funcname) funcname##_driver
#else
#define LIBAPI __stdcall
//Windows was missing the definition of the calling convention
#define DLL_CALL(funcname) LIBAPI funcname##_driver
#endif
#endif //DLL_CALL

#define LIBPUBLIC

#ifndef FCT_PTR_INIT
#define FCT_PTR_INIT
#endif


UNS8 DLL_CALL(canReceive)(CAN_HANDLE, Message *)FCT_PTR_INIT;
UNS8 DLL_CALL(canSend)(CAN_HANDLE, Message const *)FCT_PTR_INIT;
CAN_HANDLE DLL_CALL(canOpen)(s_BOARD *)FCT_PTR_INIT;
int DLL_CALL(canClose)(CAN_HANDLE)FCT_PTR_INIT;
UNS8 DLL_CALL(canChangeBaudRate)(CAN_HANDLE, char *)FCT_PTR_INIT;

#if defined DEBUG_MSG_CONSOLE_ON || defined NEED_PRINT_MESSAGE
#include "def.h"

#define _P(fc) case fc: MSG(#fc" ");break;

static inline void print_message(Message const *m)
{
    int i;
    UNS8 fc;
    MSG("id:%02x ", m->cob_id & 0x7F);
    fc = m->cob_id >> 7;
    switch(fc)
    {
        case SYNC:
            if(m->cob_id == 0x080)
                MSG("SYNC ");
            else
                MSG("EMCY ");
        break;
#ifdef CO_ENABLE_LSS
        case LSS:
        	if(m->cob_id == 0x7E5)
                MSG("MLSS ");
            else
                MSG("SLSS ");
        break;
#endif
        _P(TIME_STAMP)
        _P(PDO1tx)
        _P(PDO1rx)
        _P(PDO2tx)
        _P(PDO2rx)
        _P(PDO3tx)
        _P(PDO3rx)
        _P(PDO4tx)
        _P(PDO4rx)
        _P(SDOtx)
        _P(SDOrx)
        _P(NODE_GUARD)
        _P(NMT)
    }
    if( fc == SDOtx)
    {
        switch(m->data[0] >> 5)
        {
            /* scs: server command specifier */
            _P(UPLOAD_SEGMENT_RESPONSE)
            _P(DOWNLOAD_SEGMENT_RESPONSE)
            _P(INITIATE_DOWNLOAD_RESPONSE)
            _P(INITIATE_UPLOAD_RESPONSE)
            _P(ABORT_TRANSFER_REQUEST)
        }
    }else if( fc == SDOrx)
    {
        switch(m->data[0] >> 5)
        {
            /* ccs: client command specifier */
            _P(DOWNLOAD_SEGMENT_REQUEST)
            _P(INITIATE_DOWNLOAD_REQUEST)
            _P(INITIATE_UPLOAD_REQUEST)
            _P(UPLOAD_SEGMENT_REQUEST)
            _P(ABORT_TRANSFER_REQUEST)
        }
    }
    MSG(" rtr:%d", m->rtr);
    MSG(" len:%d", m->len);
    for (i = 0 ; i < m->len ; i++)
        MSG(" %02x", m->data[i]);
    MSG("\n");
}

#endif

#endif
