# gui_project
工程构建说明


<a href="https://996.icu"><img src="https://img.shields.io/badge/link-996.icu-red.svg" alt="996.icu" /></a>
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
## 一、构建应用，(默认master分支)可以根据configs/*_defconfig文件对功能进行裁剪

### 构建基于主机的应用
+ 构建主机的单元测试项目
   ```shell
   ./build.sh host_unittest             最终生成host_unittest可执行文件，可通过修改build.host.version中对应名字的版本号
   ```
+ 构建基于主机的LVGL项目
   ```shell
   ./build.sh host_lvgl_gui             最终生成lvgl_gui可执行文件，可通过修改build.host.version中对应名字的版本号
   ```
+ 构建基于主机的AWTK项目
   ```shell
   ./build.sh host_awtk_gui             最终生成awtk_gui可执行文件，可通过修改build.host.version中对应名字的版本号
   ```
+ 构建主机azure-rtos threadx+gui项目
   ```shell
   ./build.sh host_azure_gui            最终生成azure_gui可执行文件，可通过修改build.host.version中对应名字的版本号
   ```

### 构建基于瑞星微rv1109/rv1126的应用
+ 构建基于rv1109/rv1126 soc的单元测试项目
   ```shell
   ./build.sh rv11xx_unittest           最终生成rv11xx_unittest可执行文件，可通过修改build.rv11xx.version中对应名字的版本号
   ```
+ 构建基于rv1109/rv1126 soc的LVGL项目
   ```shell
   ./build.sh rv11xx_lvgl_gui           最终生成lvgl_gui可执行文件，可通过修改build.rv11xx.version中对应名字的版本号
   ```
+ 构建基于rv1109/rv1126 soc的AWTK项目
   ```shell
   ./build.sh rv11xx_awtk_gui           最终生成awtk_gui可执行文件，可通过修改build.rv11xx.version中对应名字的版本号
   ```
+ 构建基于rv1109/rv1126 soc的azure-rtos threadx+gui项目
   ```shell
   ./build.sh rv11xx_azure_gui          最终生成azure_gui可执行文件，可通过修改build.rv11xx.version中对应名字的版本号
   ```

### 构建基于恩智浦飞思卡尔imx6ull的应用
+ 构建基于freescale/imx6ull soc的LVGL项目
   ```shell
   ./build.sh imx6ull_lvgl_gui          最终生成lvgl_gui可执行文件，可通过修改build.imx6ull.version中对应名字的版本号
   ```
+ 构建基于freescale/imx6ull soc的AWTK项目
   ```shell
   ./build.sh imx6ull_awtk_gui          最终生成awtk_gui可执行文件，可通过修改build.imx6ull.version中对应名字的版本号
   ```
+ 构建基于freescale/imx6ull soc的azure-rtos threadx+gui项目
   ```shell
   ./build.sh imx6ull_azure_gui         最终生成azure_gui可执行文件，可通过修改build.imx6ull.version中对应名字的版本号
   ```

## 构建基于爱心元智AX620A的应用
+ 构建基于爱心元智AX620A soc的LVGL项目
   ```shell
   ./build.sh ax620a_lvgl_gui           最终生成lvgl_gui可执行文件，可通过修改build.ax620a.version中对应名字的版本号
   ```
+ 构建基于爱心元智AX620A soc的AWTK项目
   ```shell
   ./build.sh ax620a_awtk_gui           最终生成awtk_gui可执行文件，可通过修改build.ax620a.version中对应名字的版本号
   ```
+ 构建基于爱心元智AX620A soc的azure-rtos threadx+gui项目
   ```shell
   ./build.sh ax620a_azure_gui          最终生成azure_gui可执行文件，可通过修改build.ax620a.version中对应名字的版本号
   ```

### 工程清理与帮助
+ 项目工程清理
   ```shell
   ./build.sh clean                     会将编译的信息全部清理干净
   ```
+ 项目构建帮助
   ```shell
   ./build.sh [help]                    显示项目构建帮助信息[help]表示可选，也可以直接执行./build.sh
   ```

## 二、构建主机使用DRM/FB
+ 如果当前在图形桌面模式，执行下面的命令重启后进入字符模式，然后再执行GUI程序
   ```shell
   sudo systemctl set-default multi-user.target
   sudo reboot
   ```
+ 如果当前在字符模式，执行下面的命令重启后恢复桌面模式
   ```shell
   sudo systemctl set-default graphical.target
   sudo reboot
   ```
