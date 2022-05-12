
/*!
** @file   emcy.c
** @author HBUT
** @date   2020.12.10
**
** @brief Definitions of the functions that manage EMCY (emergency) messages
**
**
*/

#include <data.h>
#include "emcy.h"
#include "canfestival.h"
#include "sysdep.h"



UNS32 OnNumberOfErrorsUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);

#define Data data  /* temporary fix */


/*! This is called when Index 0x1003 is updated.
**
**
** @param d
** @param unsused_indextable
** @param unsused_bSubindex
**
** @return
**/
UNS32 OnNumberOfErrorsUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
    UNS8 index;

    // if 0, reset Pre-defined Error Field
    // else, don't change and give an abort message (eeror code: 0609 0030h)
    if (*d->error_number == 0)
        for (index = 0; index < d->error_history_size; ++index)
            *(d->error_first_element + index) = 0;		/* clear all the fields in Pre-defined Error Field (1003h) */
    else
        ;// abort message

    return 0;
}

/*! start the EMCY mangagement.
**
**
** @param d
**/
void emergencyInit(CO_Data* d)
{
    RegisterSetODentryCallBack(d, 0x1003, 0x00, &OnNumberOfErrorsUpdate);

    *d->error_number = 0;
}

/*!
**
**
** @param d
**/
void emergencyStop(CO_Data* d)
{

}


/*!
 **
 ** @param d
 ** @param cob_id
 **
 ** @return
 **/
UNS8 sendEMCY(CO_Data* d, UNS16 errCode, UNS8 errRegister, const void *Specific, UNS8 SpecificLength)
{
    Message m;

    MSG_WAR(0x3051, "sendEMCY", 0);

    m.cob_id = (UNS16)(*(UNS32*)d->error_cobid);
    m.rtr = NOT_A_REQUEST;
    m.Data[0] = errCode & 0xFF;        /* LSB */
    m.Data[1] = (errCode >> 8) & 0xFF; /* MSB */
    m.Data[2] = errRegister;

    if(Specific == NULL)
    {
        m.Data[3] = 0;		/* Manufacturer specific Error Field omitted */
        m.Data[4] = 0;
        m.Data[5] = 0;
        m.Data[6] = 0;
        m.Data[7] = 0;
        SpecificLength = 5;
    }
    else
    {
        if(SpecificLength > 5) SpecificLength = 5;

        memcpy(&m.Data[3], Specific, SpecificLength);
    }

    m.len = SpecificLength + 3;

    return canSend(d->canHandle, &m);
}

/*! Sets a new error with code errCode. Also sets corresponding bits in Error register (1001h)
 **
 **
 ** @param d
 ** @param errCode Code of the error
 ** @param errRegMask
 ** @param addInfo
 ** @return 1 if error, 0 if successful
 */
UNS8 EMCY_setError(CO_Data* d, UNS16 errCode, UNS8 errRegMask, UNS16 addInfo)
{
    UNS8 index;
    UNS8 errRegister_tmp;

    //设备能记录错误总数，这里我的EMCY_MAX_ERRORS定义为8.
    for (index = 0; index < EMCY_MAX_ERRORS; ++index)
    {
        if (d->error_data[index].errCode == errCode)		/* error already registered *///这个错误之前已经注册过
        {
            if (d->error_data[index].active)//错误还是有效的，设备还没恢复，给他点时间
            {
                MSG_WAR(0x3052, "EMCY message already sent", 0);
                return 0;
            }
            else d->error_data[index].active = 1;		/* set as active error */  //如果没有这个error,激活当前error

            break;
        }
    }

    //检查indexs是否达到上限,如果已经达到，查找是否有error已经解除，如果解除了，则index就不是EMCY_MAX_ERRORS
    if (index == EMCY_MAX_ERRORS)		/* if errCode not already registered */
        for (index = 0; index < EMCY_MAX_ERRORS; ++index) if (d->error_data[index].active == 0) break;	/* find first inactive error */

    //经过上面的判断，如果index还是等于EMCY_MAX_ERRORS，则说明系统错误处理已经满仓了。直接返回，当前消息不处理了。
    if (index == EMCY_MAX_ERRORS)		/* error_data full */
    {
        MSG_ERR(0x3053, "error_data full", 0);
        return 1;
    }

    //下面将error信息，记录到数据字典的error域，
    d->error_data[index].errCode = errCode;
    d->error_data[index].errRegMask = errRegMask;
    d->error_data[index].active = 1;

    /* set the new state in the error state machine *///设置当前设备错误状态
    d->error_state = Error_occurred;

    /* set Error Register (1001h) */
    //这里会遍历所有错误，并将所有errRegMask按位或，其实这个操作，也算是记录所有错误标记位。
    for (index = 0, errRegister_tmp = 0; index < EMCY_MAX_ERRORS; ++index)
        if (d->error_data[index].active == 1) errRegister_tmp |= d->error_data[index].errRegMask;

    *d->error_register = errRegister_tmp;//将所有错误保存到error_register准备发送

    /* set Pre-defined Error Field (1003h) */
    //这里记录当前error,覆盖之前旧的。开始往前一个回滚
    for (index = d->error_history_size - 1; index > 0; --index)
        *(d->error_first_element + index) = *(d->error_first_element + index - 1);

    *(d->error_first_element) = errCode | ((UNS32)addInfo << 16); //将当前错误放在第一个错误位置

    if(*d->error_number < d->error_history_size) ++(*d->error_number); //增加error数量

    /* send EMCY message */
    if (d->CurrentCommunicationState.csEmergency) //如果使能了emcy，下面会放emcy
        return sendEMCY(d, errCode, *d->error_register, NULL, 0);
    else return 1;
}


/*! Deletes error errCode. Also clears corresponding bits in Error register (1001h)
 **
 **
 ** @param d
 ** @param errCode Code of the error
 ** @param errRegister Bits of Error register (1001h) to be set.
 ** @return 1 if error, 0 if successful
 */
void EMCY_errorRecovered(CO_Data* d, UNS16 errCode)
{
    UNS8 index;
    UNS8 errRegister_tmp;
    UNS8 anyActiveError = 0;

    for (index = 0; index < EMCY_MAX_ERRORS; ++index)
        if (d->error_data[index].errCode == errCode) break;		/* find the position of the error */


    if ((index != EMCY_MAX_ERRORS) && (d->error_data[index].active == 1))
    {
        d->error_data[index].active = 0;

        /* set Error Register (1001h) and check error state machine */
        for (index = 0, errRegister_tmp = 0; index < EMCY_MAX_ERRORS; ++index)
            if (d->error_data[index].active == 1)
            {
                anyActiveError = 1;
                errRegister_tmp |= d->error_data[index].errRegMask;
            }

        if(anyActiveError == 0)
        {
            d->error_state = Error_free;

            /* send a EMCY message with code "Error Reset or No Error" */
            if (d->CurrentCommunicationState.csEmergency)
                sendEMCY(d, 0x0000, 0x00, NULL, 0);
        }

        *d->error_register = errRegister_tmp;
    }
    else
        MSG_WAR(0x3054, "recovered error was not active", 0);
}

/*! This function is responsible to process an EMCY canopen-message.
 **
 **
 ** @param d
 ** @param m The CAN-message which has to be analysed.
 **
 **/
void proceedEMCY(CO_Data* d, Message* m)
{
    UNS8 nodeID;
    UNS16 errCode;
    UNS8 errReg;

    MSG_WAR(0x3055, "EMCY received. Proceed. ", 0);

    /* Test if the size of the EMCY is ok */
    if ( m->len != 8)
    {
        MSG_ERR(0x1056, "Error size EMCY. CobId  : ", m->cob_id);
        return;
    }

    /* post the received EMCY */
    nodeID = m->cob_id & 0x7F;
    errCode = m->Data[0] | ((UNS16)m->Data[1] << 8);
    errReg = m->Data[2];
    (*d->post_emcy)(d, nodeID, errCode, errReg);
}

void _post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg) {}
