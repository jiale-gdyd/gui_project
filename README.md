# gui_project
工程构建说明


<a href="https://996.icu"><img src="https://img.shields.io/badge/link-996.icu-red.svg" alt="996.icu" /></a>
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
## 一、构建应用，(默认master分支)可以根据configs/*_defconfig文件对功能进行裁剪

+ 构建主机的单元测试项目
   ```shell
   ./build.sh host_unittest             最终生成host_unittest可执行文件，可通过修改build.config.host中对应名字的版本号
   ```
+ 构建主机azure-rtos threadx+gui项目
   ```shell
   ./build.sh host_azure_gui            最终生成azure_gui可执行文件，可通过修改build.config.host中对应名字的版本号
   ```
+ 构建基于rv1126 soc的单元测试项目
   ```shell
   ./build.sh rv1126_unittest           最终生成rv1126_unittest可执行文件，可通过修改build.config.host中对应名字的版本号
   ```
+ 构建基于rv1126 soc的LVGL项目
   ```shell
   ./build.sh rv1126_lvgl_gui           最终生成lvgl_gui可执行文件，可通过修改build.config.rv11xx中对应名字的版本号
   ```
+ 构建基于rv1126 soc的azure-rtos threadx+gui项目
   ```shell
   ./build.sh rv1126_azure_gui          最终生成azure_gui可执行文件，可通过修改build.config.rv11xx中对应名字的版本号
   ```
+ 项目工程清理
   ```shell
   ./build.sh clean                     会将编译的信息全部清理干净
   ```
+ 项目构建帮助
   ```shell
   ./build.sh [help]                    显示项目构建帮助信息[help]表示可选，也可以直接执行./build.sh
   ```