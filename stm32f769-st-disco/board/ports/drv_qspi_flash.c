/**
  ******************************************************************************
  * @file    stm32f769i_discovery_qspi.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the MX25L512 QSPI
  *          memory mounted on STM32F769I-Discovery board.
  @verbatim
*****************************************************************************
                                                                              */


/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <drv_qspi.h>
#include <rtdevice.h>
#include <rthw.h>
#include <finsh.h>

#ifdef BSP_USING_QSPI_FLASH

#include "spi_flash.h"
#include "spi_flash_sfud.h"

char mx25l51_read_status_register2(struct rt_qspi_device *device)
{
    char instruction = 0x35, status;
    rt_qspi_send_then_recv(device, &instruction, 1, &status, 1);
    return status;
}

void mx25l51_write_enable(struct rt_qspi_device *device)
{
    char instruction = 0x06;
    rt_qspi_send(device, &instruction, 1);
}

void mx25l51_enter_qspi_mode(struct rt_qspi_device *device)
{
    char status = 0;
    /* 0x38 enter qspi mode */
    char instruction = 0x38;
    char write_status2_buf[2] = {0};

    /* 0x31 write status register2 */
    write_status2_buf[0] = 0x31;

    status = mx25l51_read_status_register2(device);
    if (!(status & 0x02))
    {
        status |= 1 << 1;
        mx25l51_write_enable(device);
        write_status2_buf[1] = status;
        rt_qspi_send(device, &write_status2_buf, 2);
        rt_qspi_send(device, &instruction, 1);
        rt_kprintf("flash already enter qspi mode\n");
        rt_thread_mdelay(10);
    }
}
static int rt_hw_qspi_flash_with_sfud_init(void)
{
    stm32_qspi_bus_attach_device("qspi1", "qspi10", RT_NULL, 4, mx25l51_enter_qspi_mode, RT_NULL);
    
    /* init MX25L51 */
    if (RT_NULL == rt_sfud_flash_probe("MX25L51", "qspi10"))
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_qspi_flash_with_sfud_init);

#if defined(RT_USING_DFS_ELMFAT) && !defined(BSP_USING_SDCARD)
#include <dfs_fs.h>

#define BLK_DEV_NAME  "MX25L51"

int mnt_init(void)
{
    rt_thread_delay(RT_TICK_PER_SECOND);

    if (dfs_mount(BLK_DEV_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    }
    else
    {
        if(dfs_mkfs("elm", BLK_DEV_NAME) == 0)
        {
            if (dfs_mount(BLK_DEV_NAME, "/", "elm", 0, 0) == 0)
            {
                rt_kprintf("file system initialization done!\n");
            }
            else
            {
                rt_kprintf("file system initialization failed!\n");
            }
        }
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);

#endif /* defined(RT_USING_DFS_ELMFAT) && !defined(BSP_USING_SDCARD) */
#endif /* BSP_USING_QSPI_FLASH */
