/**
  ******************************************************************************
  * @file    BSP/Src/qspi.c 
  * @author  MCD Application Team
  * @brief   This example code shows how to use the QSPI Driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drv_qspi_flash.h"
#include "drv_qspi.h"
#include "drv_config.h"
/** @addtogroup STM32F7xx_HAL_Examples
* @{
*/

/** @addtogroup BSP
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE         ((uint32_t)0x0200)
#define WRITE_READ_ADDR     ((uint32_t)0x0050)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/

static void     Fill_Buffer (uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t  Buffercmp   (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  QSPI Demo
* @param  None
* @retval None
*/
int QSPI_demo (void)
{ 
  /* QSPI info structure */
  static QSPI_Info pQSPI_Info;
  uint8_t status;

  
  /*##-1- Configure the QSPI device ##########################################*/
  /* QSPI device configuration */ 
  status = BSP_QSPI_Init();
  
  if (status == QSPI_NOT_SUPPORTED)
  {
    rt_kprintf("QSPI Initialization : FAILED.\n");
    rt_kprintf("QSPI Test Aborted.\n");
  }
  else if (status == QSPI_ERROR)
  {
    rt_kprintf("QSPI Initialization : FAILED.\n");
    rt_kprintf("QSPI Test Aborted.\n");
  }
  else
  {
    rt_kprintf("QSPI Initialization : OK.\n");
    
    /*##-2- Read & check the QSPI info #######################################*/
    /* Initialize the structure */
    pQSPI_Info.FlashSize        = (uint32_t)0x00;
    pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
    pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
    pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
    pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
    
    /* Read the QSPI memory info */
    BSP_QSPI_GetInfo(&pQSPI_Info);
    
    /* Test the correctness */
    if((pQSPI_Info.FlashSize != 0x4000000) || (pQSPI_Info.EraseSectorSize != 0x1000)  ||
       (pQSPI_Info.ProgPageSize != 0x100)  || (pQSPI_Info.EraseSectorsNumber != 16384) ||
       (pQSPI_Info.ProgPagesNumber != 262144))
    {
      rt_kprintf("QSPI GET INFO : FAILED.\n");
      rt_kprintf("QSPI Test Aborted.\n");
    }
    else
    {
      rt_kprintf("QSPI GET INFO : OK.\n");
      
      /*##-3- Erase QSPI memory ################################################*/ 
      if(BSP_QSPI_Erase_Block(WRITE_READ_ADDR) != QSPI_OK)
      {
        rt_kprintf("QSPI ERASE : FAILED.\n");
        rt_kprintf("QSPI Test Aborted.\n");
      }
      else
      {
        rt_kprintf("QSPI ERASE : OK.\n");
        
        /*##-4- QSPI memory read/write access  #################################*/   
        /* Fill the buffer to write */
        Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, 0xD20F);   
        
        /* Write data to the QSPI memory */
        if(BSP_QSPI_Write(qspi_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != QSPI_OK)
        {
          rt_kprintf("QSPI WRITE : FAILED.\n");
          rt_kprintf("QSPI Test Aborted.\n");
        }
        else
        {
          rt_kprintf("QSPI WRITE : OK.\n");
          
          /* Read back data from the QSPI memory */
          if(BSP_QSPI_Read(qspi_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE) != QSPI_OK)
          {
            rt_kprintf("QSPI READ : FAILED.\n");
            rt_kprintf("QSPI Test Aborted.\n");
          }
          else
          {
            rt_kprintf("QSPI READ :  OK.\n");
            
            /*##-5- Checking data integrity ############################################*/  
            if(Buffercmp(qspi_aRxBuffer, qspi_aTxBuffer, BUFFER_SIZE) > 0)
            {
              rt_kprintf("QSPI COMPARE : FAILED.\n");
              rt_kprintf("QSPI Test Aborted.\n");
            }
            else
            {    
              rt_kprintf("QSPI Test : OK.\n");
            }  
          }
        }
      }
    }
  }
 
}
INIT_APP_EXPORT(QSPI_demo);

/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLenght: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;
  
  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval 1: pBuffer identical to pBuffer1
*         0: pBuffer differs from pBuffer1
*/
static uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }
    
    pBuffer1++;
    pBuffer2++;
  }
  
  return 0;
}
/**
* @}
*/ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
