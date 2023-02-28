#!/bin/bash

export TOPSHELL=`pwd`
MAKE=${TOPSHELL}/shell/make.sh

function print_logo()
{
cat <<'EOF'
         ___       ___                                     ___           ___       ___     
        /  /\     /  /\           ___          ___        /  /\         /  /\     /  /\    
       /  /:/    /  /:/          /__/\        /__/\      /  /::\       /  /:/    /  /::\   
      /  /:/    /  /:/           \__\:\       \__\:\    /  /:/\:\     /  /:/    /  /:/\:\  
     /  /:/    /  /:/        ___ /  /::\      /  /::\  /  /::\ \:\   /  /:/    /  /::\ \:\ 
    /__/:/    /__/:/     /\ /__/\  /:/\/   __/  /:/\/ /__/:/\:\_\:\ /__/:/    /__/:/\:\ \:\
    \  \:\    \  \:\    /:/ \  \:\/:/     /__/\/:/    \__\/  \:\/:/ \  \:\    \  \:\ \:\_\/
     \  \:\    \  \:\  /:/   \  \::/      \  \::/          \__\::/   \  \:\    \  \:\ \:\  
      \  \:\    \  \:\/:/     \__\/        \  \:\          /  /:/     \  \:\    \  \:\_\/  
       \  \:\    \  \::/                    \__\/         /__/:/       \  \:\    \  \:\    
        \__\/     \__\/                                   \__\/         \__\/     \__\/    
EOF
}

function help()
{
    print_logo
    echo "Usage: $0 [OPTION]"
    echo "[OPTION]:"
    echo "================================================================================="
    echo "    -    help                  显示帮助信息"
    echo "    -    clean                 清理项目工程"
    echo "================================================================================="
    echo "    -    host_awtk_gui         构建使用构建主机的awtk gui项目"
    echo "    -    host_lvgl_gui         构建使用构建主机的lvgl gui项目"
    echo "    -    host_azure_gui        构建使用构建主机的azure gui项目"
    echo "================================================================================="
    echo "    -    rv11xx_awtk_gui       构建使用rv1109/rv1126 soc板卡的awtk gui项目"
    echo "    -    rv11xx_lvgl_gui       构建使用rv1109/rv1126 soc板卡的lvgl gui项目"
    echo "    -    rv11xx_azure_gui      构建使用rv1109/rv1126 soc板卡的azure gui项目"
    echo "================================================================================="
    echo "    -    imx6ull_awtk_gui      构建使用freescale/imx6ull soc板卡的awtk gui项目"
    echo "    -    imx6ull_lvgl_gui      构建使用freescale/imx6ull soc板卡的lvgl gui项目"
    echo "    -    imx6ull_azure_gui     构建使用freescale/imx6ull soc板卡的azure gui项目"
    echo "================================================================================="
    echo "    -    ax620a_awtk_gui       构建使用爱芯元智AX620a soc板卡的awtk gui项目"
    echo "    -    ax620a_lvgl_gui       构建使用爱芯元智AX620a soc板卡的lvgl gui项目"
    echo "    -    ax620a_azure_gui      构建使用爱芯元智AX620a soc板卡的azure gui项目"
    echo "================================================================================="
}

if [ -z $1 ]; then
    help
else
    ${MAKE} $1
fi
