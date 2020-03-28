# RT-Thread-stm32f769-qspi

### Add qspi environment.
### Add MX25L51 in the flash chip information table.

1.首先在/borad中的Kconfig添加MX25L51 的相关配置,和相关依赖

```makefile
menu "Onboard Peripheral Drivers"
	config BSP_USING_QSPI_FLASH
        bool "Enable QSPI FLASH (MX25L51 qspi1)"
        select BSP_USING_QSPI
        select RT_USING_SFUD
        select RT_SFUD_USING_QSPI
        default n
endmenu

menuconfig BSP_USING_SPI
        bool "Enable SPI BUS"
        default n
        select RT_USING_SPI
        if BSP_USING_SPI
            config BSP_USING_SPI1
                bool "Enable SPI1 BUS"
                default n
		endif
		
config BSP_USING_QSPI
        bool "Enable QSPI BUS"
        select RT_USING_QSPI
        select RT_USING_SPI
        default n

```

2. 添加完成在初始化的时候 ，RT_thread系统会如下错误:

```
[D/drv.qspi] qspi init success!
[SFUD] Warning: Read SFDP parameter header information failed. The MX25L51 is not support JEDEC SFDP.
[SFUD] Warning: This flash device is not found or not support.
[SFUD] Error: MX25L51 flash device is initialize fail.
ERROR: SPI flash probe failed by SPI device qspi10.
```

这是因为SFUD_FLASH_EXT_INFO_TABLE并没有支持MX25L51这款flash，

```c
//可以在sfud_flash_def.h中SFUD_FLASH_EXT_INFO_TABLE下添加这一句
{"MX25L51",SFUD_MF_ID_MICRONIX, 0x20, 0x1A, 64L*1024L*1024L, SFUD_WM_PAGE_256B, 64L*1024L, 0xD8}, \
```

