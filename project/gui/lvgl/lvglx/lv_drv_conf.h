#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include <linux/kconfig.h>
#include "lv_conf.h"

#define LV_DRV_DELAY_INCLUDE                    <stdint.h>
#define LV_DRV_DELAY_US(us)                     /*delay_us(us)*/        // 延迟给定的微秒数
#define LV_DRV_DELAY_MS(ms)                     /*delay_ms(ms)*/        // 延迟给定的毫秒数

#define LV_DRV_DISP_INCLUDE                     <stdint.h>
#define LV_DRV_DISP_CMD_DATA(val)               /*pin_x_set(val)*/      // 将命令/数据引脚设置为'val'
#define LV_DRV_DISP_RST(val)                    /*pin_x_set(val)*/      // 将复位引脚设置为'val'

#define LV_DRV_DISP_SPI_CS(val)                 /*spi_cs_set(val)*/     // 将SPI的片选设置为'val'
#define LV_DRV_DISP_SPI_WR_BYTE(data)           /*spi_wr(data)*/        // 向SPI总线写入一个字节
#define LV_DRV_DISP_SPI_WR_ARRAY(adr, n)        /*spi_wr_mem(adr, n)*/  // 从'adr'向SPI总线写入'n'个字节

#define LV_DRV_DISP_PAR_CS(val)                 /*par_cs_set(val)*/     // 将并行端口的芯片选择设置为'val'
#define LV_DRV_DISP_PAR_SLOW                    /*par_slow()*/          // 在并口上设置低速
#define LV_DRV_DISP_PAR_FAST                    /*par_fast()*/          // 在并口上设置高速
#define LV_DRV_DISP_PAR_WR_WORD(data)           /*par_wr(data)*/        // 写一个字到并口
#define LV_DRV_DISP_PAR_WR_ARRAY(adr, n)        /*par_wr_mem(adr,n)*/   // 将'n'个字节从'adr'写入并行端口

#define LV_DRV_INDEV_INCLUDE                    <stdint.h>
#define LV_DRV_INDEV_RST(val)                   /*pin_x_set(val)*/      // 将复位引脚设置为'val'
#define LV_DRV_INDEV_IRQ_READ                   0 /*pn_x_read()*/       // 读取IRQ引脚

#define LV_DRV_INDEV_SPI_CS(val)                /*spi_cs_set(val)*/     // 将SPI的片选设置为'val'
#define LV_DRV_INDEV_SPI_XCHG_BYTE(data)        0 /*spi_xchg(val)*/     // 将'val'写入SPI并给出读取值

#define LV_DRV_INDEV_I2C_START                  /*i2c_start()*/         // 启动I2C
#define LV_DRV_INDEV_I2C_STOP                   /*i2c_stop()*/          // 停止I2C
#define LV_DRV_INDEV_I2C_RESTART                /*i2c_restart()*/       // 重启I2C
#define LV_DRV_INDEV_I2C_WR(data)               /*i2c_wr(data)*/        // 向I2C总线写入一个字节
#define LV_DRV_INDEV_I2C_READ(last_read)        0 /*i2c_rd()*/          // 从I2C总线读取一个字节

// 硬件加速SDL驱动程序
#ifndef USE_SDL_GPU
#define USE_SDL_GPU                             0
#endif

#if USE_SDL || USE_SDL_GPU
#define SDL_HOR_RES                             480
#define SDL_VER_RES                             320

#define SDL_ZOOM                                1

#define SDL_DOUBLE_BUFFERED                     0

#define SDL_INCLUDE_PATH                        <SDL2/SDL.h>

#define SDL_DUAL_DISPLAY                        0
#endif

#ifndef USE_MONITOR
#define USE_MONITOR                             0
#endif

#if USE_MONITOR
#define MONITOR_HOR_RES                         480
#define MONITOR_VER_RES                         320

// 按此系数缩放窗口(在模拟小屏幕时很有用)
#define MONITOR_ZOOM                            1

// 用于测试仅更改地址的真正双缓冲。使用2个绘制缓冲区，均具有MONITOR HOR_RES x MONITOR_VER_RES大小
#define MONITOR_DOUBLE_BUFFERED                 0

#define MONITOR_SDL_INCLUDE_PATH                <SDL2/SDL.h>

// 打开两个窗口测试多显示器支持
#define MONITOR_DUAL                            0
#endif

#ifndef USE_WINDOWS
#define USE_WINDOWS                             0
#endif

#if USE_WINDOWS
#define WINDOW_HOR_RES                          480
#define WINDOW_VER_RES                          320
#endif

#ifndef USE_WIN32DRV
#define USE_WIN32DRV                            0
#endif

#if USE_WIN32DRV
// 按此系数缩放窗口(在模拟小屏幕时很有用)
#define WIN32DRV_MONITOR_ZOOM                   1
#endif

#ifndef USE_GTK
#define USE_GTK                                 0
#endif

#ifndef USE_SSD1963
#define USE_SSD1963                             0
#endif

#if USE_SSD1963
#define SSD1963_HOR_RES                         LV_HOR_RES
#define SSD1963_VER_RES                         LV_VER_RES
#define SSD1963_HT                              531
#define SSD1963_HPS                             43
#define SSD1963_LPS                             8
#define SSD1963_HPW                             10
#define SSD1963_VT                              288
#define SSD1963_VPS                             12
#define SSD1963_FPS                             4
#define SSD1963_VPW                             10
#define SSD1963_HS_NEG                          0           // 负水平同步
#define SSD1963_VS_NEG                          0           // 负垂直同步
#define SSD1963_ORI                             0           // 0, 90, 180, 270
#define SSD1963_COLOR_DEPTH                     16
#endif

#ifndef USE_R61581
#define USE_R61581                              0
#endif

#if USE_R61581
#define R61581_HOR_RES                          LV_HOR_RES
#define R61581_VER_RES                          LV_VER_RES
#define R61581_HSPL                             0           // HSYNC信号极性
#define R61581_HSL                              10          // HSYNC长度(未实现)
#define R61581_HFP                              10          // 水平前廊(未实现)
#define R61581_HBP                              10          // 水平后廊(未实现)
#define R61581_VSPL                             0           // VSYNC信号极性
#define R61581_VSL                              10          // VSYNC长度(未实现)
#define R61581_VFP                              8           // 垂直前廊
#define R61581_VBP                              8           // 垂直后廊
#define R61581_DPL                              0           // DCLK信号极性
#define R61581_EPL                              1           // ENABLE信号极性
#define R61581_ORI                              0           // 0, 180
#define R61581_LV_COLOR_DEPTH                   16          // Fix 16 bit
#endif

#ifndef USE_ST7565
#define USE_ST7565                              0
#endif

#if USE_ST7565

#endif

#ifndef USE_GC9A01
#define USE_GC9A01                              0
#endif

#if USE_GC9A01

#endif

#ifndef USE_UC1610
#define USE_UC1610                              0
#endif

#if USE_UC1610
#define UC1610_HOR_RES                          LV_HOR_RES
#define UC1610_VER_RES                          LV_VER_RES
#define UC1610_INIT_CONTRAST                    33          // 初始对比度，值[%]
#define UC1610_INIT_HARD_RST                    0           // 1: 初始化时硬件复位，0: 软件复位
#define UC1610_TOP_VIEW                         0           // 0: 底视图，1: 顶视图
#endif

#ifndef USE_SHARP_MIP
#define USE_SHARP_MIP                           0
#endif

#if USE_SHARP_MIP
#define SHARP_MIP_HOR_RES                       LV_HOR_RES
#define SHARP_MIP_VER_RES                       LV_VER_RES
#define SHARP_MIP_SOFT_COM_INVERSION            0
#define SHARP_MIP_REV_BYTE(b)                   /*((uint8_t) __REV(__RBIT(b)))*/  // 体系结构/编译器相关字节位顺序反转
#endif

#ifndef USE_ILI9341
#define USE_ILI9341                             0
#endif

#if USE_ILI9341
#define ILI9341_HOR_RES                         LV_HOR_RES
#define ILI9341_VER_RES                         LV_VER_RES
#define ILI9341_GAMMA                           1
#define ILI9341_TEARING                         0
#endif

#if defined(CONFIG_DRM_DISP_DRIVER)
#ifndef USE_DRM
#define USE_DRM                                 1
#endif

#ifndef USE_FBDEV
#define USE_FBDEV                               0
#endif

#ifndef USE_WAYLAND
#define USE_WAYLAND                             0
#endif
#elif defined(CONFIG_FBDEV_DISP_DRIVER)
#ifndef USE_FBDEV
#define USE_FBDEV                               1
#endif

#ifndef USE_DRM
#define USE_DRM                                 0
#endif

#ifndef USE_WAYLAND
#define USE_WAYLAND                             0
#endif
#elif defined(CONFIG_WAYLAND_DISP_DRIVER)
#ifndef USE_FBDEV
#define USE_FBDEV                               0
#endif

#ifndef USE_DRM
#define USE_DRM                                 0
#endif

#ifndef USE_WAYLAND
#define USE_WAYLAND                             1
#endif
#endif

#if USE_FBDEV
#define FBDEV_PATH                              "/dev/fb0"
#endif

#ifndef USE_BSD_FBDEV
#define USE_BSD_FBDEV                           0
#endif

#if USE_BSD_FBDEV
#define FBDEV_PATH                              "/dev/fb0"
#endif

#if USE_DRM
#define DRM_CARD                                "/dev/dri/card0"
#define DRM_CONNECTOR_ID                        -1                      // -1为第一个连接的
#endif

#if USE_WAYLAND

#ifndef LV_WAYLAND_CLIENT_SIDE_DECORATIONS
#define LV_WAYLAND_CLIENT_SIDE_DECORATIONS      1
#endif

#ifndef LV_WAYLAND_WL_SHELL
#define LV_WAYLAND_WL_SHELL                     1
#endif

#ifndef LV_WAYLAND_XDG_SHELL
#define LV_WAYLAND_XDG_SHELL                    1
#endif

#ifndef LV_WAYLAND_TIMER_HANDLER
#define LV_WAYLAND_TIMER_HANDLER                1
#endif
#endif

#ifndef USE_XPT2046
#define USE_XPT2046                             0
#endif

#if USE_XPT2046
#define XPT2046_HOR_RES                         480
#define XPT2046_VER_RES                         320
#define XPT2046_X_MIN                           200
#define XPT2046_Y_MIN                           200
#define XPT2046_X_MAX                           3800
#define XPT2046_Y_MAX                           3800
#define XPT2046_AVG                             4
#define XPT2046_X_INV                           0
#define XPT2046_Y_INV                           0
#define XPT2046_XY_SWAP                         0
#endif

#ifndef USE_FT5406EE8
#define USE_FT5406EE8                           0
#endif

#if USE_FT5406EE8
#define FT5406EE8_I2C_ADR                       0x38                    // 7位地址
#endif

#ifndef USE_AD_TOUCH
#define USE_AD_TOUCH                            0
#endif

#if USE_AD_TOUCH

#endif

#ifndef USE_MOUSE
#define USE_MOUSE                               0
#endif

#if USE_MOUSE

#endif

#ifndef USE_MOUSEWHEEL
#define USE_MOUSEWHEEL                          0
#endif

#if USE_MOUSEWHEEL

#endif

#ifndef USE_LIBINPUT
#define USE_LIBINPUT                            0
#endif

#ifndef USE_BSD_LIBINPUT
#define USE_BSD_LIBINPUT                        0
#endif

#if USE_LIBINPUT || USE_BSD_LIBINPUT
#define LIBINPUT_NAME                           "/dev/input/event0"         // 您可以使用"evtest" Linux 工具获取设备列表并对其进行测试
#endif

#ifndef USE_EVDEV
#define USE_EVDEV                               1
#endif

#ifndef USE_BSD_EVDEV
#define USE_BSD_EVDEV                           0
#endif

#if USE_EVDEV || USE_BSD_EVDEV
#define EVDEV_NAME                              "/dev/input/event0"         // 您可以使用"evtest" Linux 工具获取设备列表并对其进行测试
#define EVDEV_SWAP_AXES                         0                           // 交换触摸屏的x和y轴

#define EVDEV_CALIBRATE                         0                           // 通过使用每个轴的最大值和最小值来缩放和偏移触摸屏坐标

#if EVDEV_CALIBRATE
#define EVDEV_HOR_MIN                           0                           // 通过EVDEV_XXX_MAX反转轴交换EVDEV_XXX_MIN
#define EVDEV_HOR_MAX                           4096                        // "evtest" Linux工具可以帮助获得正确的校准值
#define EVDEV_VER_MIN                           0
#define EVDEV_VER_MAX                           4096
#endif
#endif

#ifndef USE_XKB
#define USE_XKB                                 0
#endif

#if USE_LIBINPUT || USE_BSD_LIBINPUT || USE_EVDEV || USE_BSD_EVDEV
#if USE_XKB
#define XKB_KEY_MAP                             { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
#endif
#endif

#ifndef USE_KEYBOARD
#define USE_KEYBOARD                            0
#endif

#if USE_KEYBOARD

#endif

#endif
