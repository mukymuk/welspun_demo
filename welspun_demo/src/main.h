
#ifndef  __MAIN_H

#define __MAIN_H

#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio.h"
#include "audio_play.h"
#include <stdio.h>


/* Size of the recorder buffer (Multiple of 4096, RAM_BUFFER_SIZE used in BSP) */
#define WR_BUFFER_SIZE           0x7000


/* Exported functions ------------------------------------------------------- */
void Toggle_Leds(void);
void Error_Handler(void);

#endif
