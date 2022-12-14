#!/bin/bash

export TOPSHELL=`pwd`
BUILDSHELLNAME=build.sh
MAKE=${TOPSHELL}/shell/make.sh

function help()
{
    echo "Usage: ./${BUILDSHELLNAME} [OPTION]"
    echo "[OPTION]:"
    echo "============================================================================"
    echo "    -    help                  显示帮助信息"
    echo "    -    clean                 清理项目工程"
    echo "============================================================================"
    echo "    -    host_unittest         构建使用构建主机的单元测试项目"
    echo "    -    host_awtk_gui         构建使用构建主机的awtk gui项目"
    echo "    -    host_lvgl_gui         构建使用构建主机的lvgl gui项目"
    echo "    -    host_azure_gui        构建使用构建主机的azure gui项目"
    echo "============================================================================"
    echo "    -    rv11xx_unittest       构建使用rv1109/rv1126 soc板卡的单元测试项目"
    echo "    -    rv11xx_awtk_gui       构建使用rv1109/rv1126 soc板卡的awtk gui项目"
    echo "    -    rv11xx_lvgl_gui       构建使用rv1109/rv1126 soc板卡的lvgl gui项目"
    echo "    -    rv11xx_azure_gui      构建使用rv1109/rv1126 soc板卡的azure gui项目"
    echo "============================================================================"
}

if [ -z $1 ]; then
    help
else
    ${MAKE} $1
fi
