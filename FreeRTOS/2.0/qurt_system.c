/*======================================================================
   Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
    
   Redistribution and use in source and binary forms, with or without
   modification, are permitted (subject to the limitations in the
   disclaimer below) provided that the following conditions are met:
    
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
    
      * Redistributions in binary form must reproduce the above
        copyright notice, this list of conditions and the following
        disclaimer in the documentation and/or other materials provided
        with the distribution.
    
      * Neither the name of The Linux Foundation nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.
    
   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
   HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
                           
   qurt_system.c 

   GENERAL DESCRIPTION 
     QuRT CPU specific port functions

 ======================================================================*/

#include "qurt_internal.h"
#include "FreeRTOS.h"
#include "task.h"

/*======================================================================
                          EXTERNAL
 ======================================================================*/
/*======================================================================
                          MACROS
 ======================================================================*/

/*======================================================================
                          TYPEDEFS
 ======================================================================*/
#if defined (__GNUC__)

uint32 qurt_system_get_ipsr( void ) __attribute__ (( naked ));

#endif

#if defined(__ARMCC_VERSION)

uint32 qurt_system_get_ipsr( void );

#endif
/*======================================================================
                          FUNCTIONS
=======================================================================*/
#if defined (__GNUC__)

uint32 qurt_system_get_ipsr( void )
{
  __asm volatile
  (
		"	mrs r0, ipsr   \n" 
		"	bx r14   \n" 
  );
}

#endif /* __GNUC__ */

#if defined(__ARMCC_VERSION)

__asm uint32 qurt_system_get_ipsr( void )
{
	PRESERVE8

	mrs r0, ipsr
	bx r14
}

#endif /* __ARMCC_VERSION */

uint32 qurt_system_power_systick_restore( uint64 tsleep, uint32 syst_rvr, uint32 syst_cvr )
{
   uint32 tick_missed_abs;
   uint32 tick_missed_partial;
   uint32 reload_prog;
   
   /* if number of clock cycle in sleep is greater then current count in cvr */
   if( tsleep > syst_cvr)
   {
      tick_missed_abs = 1 + (tsleep - syst_cvr)/syst_rvr;
      tick_missed_partial = (tsleep - syst_cvr)%syst_rvr; 
   }
   else
   {
      tick_missed_abs = 1 + (syst_cvr - tsleep )/syst_rvr;
      tick_missed_partial = (syst_cvr- tsleep)%syst_rvr; 
   }
   reload_prog = syst_rvr - tick_missed_partial;
   vTaskStepTick(tick_missed_abs);
   return reload_prog;
}
