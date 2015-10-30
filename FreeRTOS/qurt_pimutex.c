/*=======================================================================================
    Copyright (c) 2015 Linux Foundation.
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without modification, 
    are permitted (subject to the limitations in the disclaimer below)provided that 
    the following conditions are met:

    1.Redistributions of source code must retain the above copyright notice, this list
      of conditions and the following disclaimer.
    
    2.Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other 
      materials provided with the distribution.
    
    3.Neither the name of Linux Foundation nor the names of its contributors may be
      used to endorse or promote products derived from this software without specific 
      prior written permission.
       
    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS 
    LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR APARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER ORCONTRIBUTORS BE LIABLEFOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIALDAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
    OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE  OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
    OF SUCH DAMAGE.
                           
    qurt_pimutex.c 

    GENERAL DESCRIPTION 
        QuRT recursive mutex with priority inheritance functions

 ======================================================================================*/

#ifdef FEATURE_MUTEX_PI_RECURSIVE

#include "qurt_mutex.h"
#include "qurt_error.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "assert.h"

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
   SemaphoreHandle_t pimutex;
}_qurt_mutex_t;


/*======================================================================
                          FUNCTIONS
=======================================================================*/

void qurt_mutex_init(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   lock_internal->pimutex = xSemaphoreCreateRecursiveMutex();
}

void qurt_mutex_lock(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   
   /* mutex lock timed is not supported from ISR */
   ASSERT(0 == qurt_system_get_ipsr());
   xSemaphoreTakeRecursive(lock_internal->pimutex , portMAX_DELAY);
}

int qurt_mutex_lock_timed(qurt_mutex_t *lock, qurt_time_t timeout)
{
   BaseType_t ret; 
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   
   /* mutex lock timed is not supported from ISR */
   ASSERT(0 == qurt_system_get_ipsr());
   if(NULL == lock_internal)
   {
    return QURT_EINVALID;
   }
   ret = xSemaphoreTakeRecursive(lock_internal->pimutex , (TickType_t)timeout);
   if(pdPASS != ret)
   {
    return QURT_EFAILED_TIMEOUT;
   }
   return QURT_EOK;
}

void qurt_mutex_unlock(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   xSemaphoreGiveRecursive(lock_internal->pimutex );
}

int qurt_mutex_try_lock(qurt_mutex_t *lock)
{
   BaseType_t ret; 
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;   
   
   if(NULL == lock_internal)
   {
    return QURT_EINVALID;
   }
   ret = xSemaphoreTakeRecursive(lock_internal->pimutex , 0);
   if(pdPASS != ret)
   {
    return QURT_EFAILED;
   }
   return QURT_EOK;
}

void qurt_mutex_destroy(qurt_mutex_t *lock)
{
   _qurt_mutex_t *lock_internal = (_qurt_mutex_t *)lock;
   vSemaphoreDelete(lock_internal->pimutex);
   lock_internal->pimutex = NULL;
}

#endif /* FEATURE_MUTEX_PI_RECURSIVE */
