/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN
AT91 Port: Peter CHRISTEN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef __CAN_CANFESTIVAL__
#define __CAN_CANFESTIVAL__

#include "applicfg.h"
#include "can_driver.h"
#include "data.h"

//#include "objdict.h"

extern UNS32 DAY;
extern UNS32 MS;
extern UNS32 SillySlave_obj1012;
extern UNS8 SillySlave_obj1800_Transmission_Type;
extern void IO_Set(CAN_PORT ctrl, Message *RxMsg);

#define CANFESTIVAL_CAN_DEVICE_NAME "can1"
#define CANFESTIVAL_TIMER_DEVICE_NAME "timer13"
#define CANFESTIVAL_TIMER_THREAD_PRIO 5
extern uint16_t adc_init(void);
// ---------  to be called by user app ---------
void initTimer(void);
CAN_PORT canOpen(s_BOARD *board, CO_Data * d);
void StartTimerLoop(TimerCallback_t _init_callback);
UNS8 canSend(CAN_PORT canHandle, Message *m);
UNS8 canReceive(Message *m);
#endif
