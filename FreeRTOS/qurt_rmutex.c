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
                           
   qurt_rmutex.c

   GENERAL DESCRIPTION 
     QuRT recursive mutex functions

 ======================================================================*/

#ifdef FEATURE_MUTEX_RECURSIVE

#include "qurt_internal.h"
#include "FreeRTOS.h"
#include "semphr.h"

/*======================================================================
                          EXTERNAL
 ======================================================================*/

extern uint32 qurt_system_get_ipsr( void );

/*======================================================================
                        MACROS
 ======================================================================*/

/*======================================================================
                          TYPEDEFS
 ======================================================================*/

/** Signal Object */
typedef struct _qurt_mutex_t {
   SemaphoreHandle_t binary_sem;
   TaskHandle_t owner;
   uint32 count;
}_qurt_mutex_t;

/*======================================================================
                          FUNCTIONS
=======================================================================*/

void qurt_mutex_init(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   lock_internal->owner = NULL;
   lock_internal->count = 0;
   lock_internal->binary_sem = xSemaphoreCreateBinary();
   xSemaphoreGive(lock_internal->binary_sem);
}

int qurt_mutex_lock_timed(qurt_mutex_t *lock, qurt_time_t timeout)
{
   TaskHandle_t curr_task; 
   BaseType_t ret ;
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   
   /* mutex lock timed is not supported from ISR */
   ASSERT(0 == qurt_system_get_ipsr());
   if(NULL == lock_internal)
   {
    return QURT_EINVALID;
   }
   curr_task = xTaskGetCurrentTaskHandle();
   if(lock_internal->owner == curr_task)
   {
     lock_internal->count++;
     return QURT_EOK;
   }
   
   ret = xSemaphoreTake(lock_internal->binary_sem , (TickType_t)timeout);
   if(pdPASS != ret)
   {
      return QURT_EFAILED_TIMEOUT;
   }
   lock_internal->count++;
   lock_internal->owner = xTaskGetCurrentTaskHandle();
   return QURT_EOK;
}

void qurt_mutex_lock(qurt_mutex_t *lock)
{
   /* mutex lock timed is not supported from ISR */
   ASSERT(0 == qurt_system_get_ipsr());
   qurt_mutex_lock_timed(lock, portMAX_DELAY);
}

void qurt_mutex_unlock(qurt_mutex_t *lock)
{
   TaskHandle_t curr_task; 
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   BaseType_t ret ;
   
   curr_task = xTaskGetCurrentTaskHandle();
   if(lock_internal->owner == curr_task)
   {
      lock_internal->count--;
      if( 0 == lock_internal->count)
      {
         lock_internal->owner = NULL;
         /* if running in thread mode */
         if( 0 == qurt_system_get_ipsr())
         {
            ret = xSemaphoreGive(lock_internal->binary_sem);
         }
         else
         {
            /*higher_prio_task_woken must be initialised to pDFALSE */
            BaseType_t higher_prio_task_woken = pdFALSE;
            ret = xSemaphoreGiveFromISR(lock_internal->binary_sem, &higher_prio_task_woken);
            
            /*It is possible that a semaphore will have one or more task blocked 
            on it waiting to give semaphore. Calling xSemaphoreGive FromISR will 
            make task waiting on blocked semaphore leave the blocked state. If that
            happens to be task with priority equal or greater then ISR task context , 
            higher_prio_task_woken is now set to pdTrue. If higher_prio_task_woken
            is true then a context switch should be requested */
            portYIELD_FROM_ISR(higher_prio_task_woken);
         }
      }
   }
   return;
}

int qurt_mutex_try_lock(qurt_mutex_t *lock)
{
   TaskHandle_t curr_task; 
   BaseType_t ret ;
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
      
   if(NULL == lock_internal)
   {
    return QURT_EINVALID;
   }
   curr_task = xTaskGetCurrentTaskHandle();
   if(lock_internal->owner == curr_task)
   {
     lock_internal->count++;
     return QURT_EOK;
   }
   
   /* if running in thread mode */
   if( 0 == qurt_system_get_ipsr())
   {
      ret = xSemaphoreTake(lock_internal->binary_sem , 0);
      if(pdPASS != ret)
      {
         return QURT_EFAILED;
      }
   }
   else
   {
      /*higher_prio_task_woken must be initialised to pDFALSE */
      BaseType_t higher_prio_task_woken = pdFALSE;
      ret = xSemaphoreTakeFromISR(lock_internal->binary_sem , &higher_prio_task_woken);
      if(pdPASS != ret)
      {
         return QURT_EFAILED;
      }
      
      /*It is possible (although unlikely and dependent on semaphore type) 
      that a semaphore will have one or more task blocked on it waiting to
      give semaphore. Calling xSemaphoreTakeFromISR will make task waiting
      on blocked semaphore leave the blocked state. If that happens to be
      task with priority equal or greater then ISR task context , 
      higher_prio_task_woken is now set to pdTrue. If higher_prio_task_woken
      is true then a context switch should be requested */ 
      portYIELD_FROM_ISR(higher_prio_task_woken);
   } 

   return QURT_EOK;
}

void qurt_mutex_destroy(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   if(NULL != lock_internal)
   {
      vSemaphoreDelete(lock_internal->binary_sem);
      lock_internal->binary_sem = NULL;
      lock_internal->owner = NULL;
      lock_internal->count = 0;
   }
}

#endif /* FEATURE_MUTEX_RECURSIVE */
