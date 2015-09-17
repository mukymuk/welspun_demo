
#include "audio_play.h"

typedef struct
{
  uint32_t   ChunkID;       /* 0 */ 
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16 */  
  uint16_t   AudioFormat;   /* 20 */ 
  uint16_t   NbrChannels;   /* 22 */   
  uint32_t   SampleRate;    /* 24 */
  
  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */  
  uint16_t   BitPerSample;  /* 34 */  
  uint32_t   SubChunk2ID;   /* 36 */   
  uint32_t   SubChunk2Size; /* 40 */    

}WAVE_FormatTypeDef;

#define AUDIO_FILE_SIZE               0x70000
#define AUDIO_FILE_ADDRESS            0x08080000   /* Audio file address */  
  
extern __IO uint8_t UserPressButton;


WAVE_FormatTypeDef *waveformat =  NULL;

/* Variables used in normal mode to manage audio file during DMA transfer */
uint32_t AudioTotalSize           = 0xFFFF; /* This variable holds the total size of the audio file */
uint32_t AudioRemSize             = 0xFFFF; /* This variable holds the remaining data in audio file */
uint16_t *CurrentPos ;             /* This variable holds the current position of audio pointer */

extern uint16_t WrBuffer[WR_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief Test Audio Hardware.
  *   The main objective of this test is to check the hardware connection of the 
  *   Audio peripheral.
  * @param  None
  * @retval None
  */
void AudioPlay_Test(void)
{  
  /* Initial volume level (from 0 (Mute) to 100 (Max)) */
  __IO uint8_t volume = 75;
 
  /* Turn ON LED6: start of Audio file play */
  BSP_LED_On(LED6);
  
  /* Retrieve Wave Sample rate */
  waveformat = (WAVE_FormatTypeDef*) AUDIO_FILE_ADDRESS;

  /* Initialize Audio Device */
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, volume, waveformat->SampleRate) != 0)
  {
    Error_Handler();
  }
  
  /* Set variable used to stop player before starting */
  UserPressButton = 0;
 
  /* Set the total number of data to be played */
  AudioTotalSize = AUDIO_FILE_SIZE;  
  /* Set the current audio pointer position */
  CurrentPos = (uint16_t *)(AUDIO_FILE_ADDRESS);
  /* Start the audio player */
  BSP_AUDIO_OUT_Play(CurrentPos, AudioTotalSize);  
  /* Update the remaining number of data to be played */
  AudioRemSize = AudioTotalSize - AUDIODATA_SIZE * DMA_MAX(AudioTotalSize);  
  /* Update the current audio pointer position */
  CurrentPos += DMA_MAX(AudioTotalSize);
  
  while (!UserPressButton);
  
  /* Stop Player before close Test */
  if (BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW) != AUDIO_OK)
  {
    /* Audio Stop error */
    Error_Handler();
  }
}

/*--------------------------------
Callbacks implementation:
The callbacks prototypes are defined in the stm32f4_discovery_audio.h file
and their implementation should be done in the user code if they are needed.
Below some examples of callback implementations.
--------------------------------------------------------*/
/**
* @brief  Calculates the remaining file size and new position of the pointer.
* @param  None
* @retval None
*/
void BSP_AUDIO_OUT_TransferComplete_CallBack()
{
  uint32_t replay = 0;
  
  if (AudioRemSize > 0)
  {
    /* Replay from the current position */
    BSP_AUDIO_OUT_ChangeBuffer((uint16_t*)CurrentPos, DMA_MAX(AudioRemSize/AUDIODATA_SIZE));
    
    /* Update the current pointer position */
    CurrentPos += DMA_MAX(AudioRemSize);        
    
    /* Update the remaining number of data to be played */
    AudioRemSize -= AUDIODATA_SIZE * DMA_MAX(AudioRemSize/AUDIODATA_SIZE);  
  }
  else
  {
    /* Request to replay audio file from beginning */
    replay = 1;
  }
  
  /* Audio sample used for play */
  if((replay == 1))
  {
    /* Replay from the beginning */
    /* Set the current audio pointer position */
    CurrentPos = (uint16_t *)(AUDIO_FILE_ADDRESS);
    /* Replay from the beginning */
    BSP_AUDIO_OUT_Play(CurrentPos, AudioTotalSize);
    /* Update the remaining number of data to be played */
    AudioRemSize = AudioTotalSize - AUDIODATA_SIZE * DMA_MAX(AudioTotalSize);  
    /* Update the current audio pointer position */
    CurrentPos += DMA_MAX(AudioTotalSize);
  }
}

/**
  * @brief  Manages the DMA FIFO error interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_Error_CallBack(void)
{
  /* Stop the program with an infinite loop */
  Error_Handler();
}
