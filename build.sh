#!/bin/bash

export TOPSHELL=`pwd`
NUILDSHELLNAME=build.sh
MAKE=${TOPSHELL}/shell/make.sh

function help()
{
    echo "Usage: ./${NUILDSHELLNAME} [OPTION]"
    echo "[OPTION]:"
    echo "============================================================================================================="
    echo "    -    help                  显示帮助信息"
    echo "    -    clean                 清理项目工程"
    echo "============================================================================================================="
    echo "    -    host_unittest         构建使用构建主机的单元测试项目"
    echo "    -    host_lvgl_gui         构建使用构建主机的lvgl gui项目"
    echo "    -    host_azure_gui        构建使用构建主机的azure gui项目"
    echo "============================================================================================================="
    echo "    -    rv1126_unittest       构建使用rv1126 soc板卡的单元测试项目"
    echo "    -    rv1126_lvgl_gui       构建使用rv1126 soc板卡的lvgl gui项目"
    echo "    -    rv1126_azure_gui      构建使用rv1126 soc板卡的azure gui项目"
    echo "============================================================================================================="
}

if [ -z $1 ]; then
    help
else
    ${MAKE} $1
fi
