#/***************************************************************************************
#    Copyright (c) 2015 Linux Foundation.
#    All rights reserved.
#    
#    Redistribution and use in source and binary forms, with or without modification, 
#    are permitted (subject to the limitations in the disclaimer below)provided that 
#    the following conditions are met:
#
#    1.Redistributions of source code must retain the above copyright notice, this list
#      of conditions and the following disclaimer.
#    
#    2.Redistributions in binary form must reproduce the above copyright notice, this list
#      of conditions and the following disclaimer in the documentation and/or other 
#      materials provided with the distribution.
#    
#    3.Neither the name of Linux Foundation nor the names of its contributors may be
#      used to endorse or promote products derived from this software without specific 
#      prior written permission.
#       
#    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS 
#    LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
#    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
#    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR APARTICULAR PURPOSE ARE
#    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER ORCONTRIBUTORS BE LIABLEFOR ANY
#    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIALDAMAGES (INCLUDING,
#    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
#    OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
#    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE  OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
#    OF SUCH DAMAGE.
#
#****************************************************************************************/
#    MakeFile to build QuRT library with GCC, GNU tool chain.
#
#    Please use this as reference file for building QuRT Library. 
#
#    In order to build QuRT library following dependencies has to be met - 
#    1. Update to INCLUDES PATH with location of QuRT header files and other header files 
#        supplied by SDK.
#    2. Update to INCLUDES PATH with location of FreeRTOS header files.
#
#****************************************************************************************/
default: all

OBJDIR = obj
OUTDIR = output
CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar
RM = rm -rf

#Specify COPT FLAGS
COPTS := -c -g -mcpu=cortex-m4 -mthumb -fno-inline-functions

#Specify preprocessor flag/feature defines
DEFINES := -D FEATURE_MUTEX_PI_RECURSIVE -D QURT_THREAD_MAX_PRIORITIES=32 -D QURT_THREAD_ATTR_PRIORITY_DEFAULT=16  -D QURT_THREAD_ATTR_NAME_MAXLEN=16 -D QURT_THREAD_MAX_PRIORITIES=32 -D QURT_THREAD_DEFAULT_STACK_SIZE=128 \

#Specify Include paths here
INCLUDES := \
  -I. \
  -I../FreeRTOS/FreeRTOS/Demo/QUARTZ \
  -I../FreeRTOS/FreeRTOS/Source/include \

#Set CFLAGS
CFLAGS = $(COPTS) $(DEFINES) $(INCLUDES)

#Specify Source files here
CSRCS = $(wildcard *.c)

#List of obj files   
OBJS := $(CSRCS:.c=.o)

# Rule : all
all: qurt.lib

# Rule : qurt.lib 
qurt.lib: $(OBJS)
	$(AR) -r $(OUTDIR)/$@ $(OUTDIR)/*.o
    
# Generate object files
%.o: %.c
	@echo Compiling $<	
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c -o $(OUTDIR)/$(@F) $^
    
# Rule : clean
clean :
	rm -rf $(OUTDIR)

