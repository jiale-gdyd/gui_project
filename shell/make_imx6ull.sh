#!/bin/bash

source ${TOPSHELL}/shell/buildFunc.sh
source ${TOPSHELL}/shell/buildConf.sh

IMX6ULL_LVGL_GUI_CONFIG=imx6ull_lvgl_gui_defconfig
IMX6ULL_AWTK_GUI_CONFIG=imx6ull_awtk_gui_defconfig
IMX6ULL_AZURE_GUI_CONFIG=imx6ull_azure_gui_defconfig

if [ -e /opt/toolchain/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-g++ ]; then
    IMX6ULL_CROSS_COMPILE=/opt/toolchain/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
else
    IMX6ULL_CROSS_COMPILE=arm-none-linux-gnueabihf-
fi

function imx6ull_clean()
{
    rm -rf lvgl_gui
    rm -rf awtk_gui
    rm -rf azure_gui
    remove_gitcommit
}

function imx6ull_lvgl_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=imx6ull
    export Platform=nxp
    export APP_NAME=lvgl_gui
    export USE_STDCPP_VERSION=-std=gnu++14
    generate_gitcommit

    make ${IMX6ULL_LVGL_GUI_CONFIG} && make ARCH=arm CROSS_COMPILE=${IMX6ULL_CROSS_COMPILE} -j$[$(nproc)-1]
    if [ $? -ne 0 ]; then
        error_exit "Unfortunately, build ${PROJECT_NAME} failed"
    fi

    print_info "build ${PROJECT_NAME} project done."
    print_info "Congratulations, the compilation is successful, Modify by [${AUTHOR_NAME}]"

    print_info "Finished '${SHELL_NAME}'"

    end=`get_timestamp`
    second=`expr ${end} - ${begin}`
    min=`expr ${second} / 60`
    echo "It takes "${min}" minutes, and "${second} "seconds"
}

function imx6ull_awtk_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=imx6ull
    export Platform=nxp
    export APP_NAME=awtk_gui
    export USE_STDCPP_VERSION=-std=gnu++11
    generate_gitcommit

    make ${IMX6ULL_AWTK_GUI_CONFIG} && make ARCH=arm CROSS_COMPILE=${IMX6ULL_CROSS_COMPILE} -j$[$(nproc)-1]
    if [ $? -ne 0 ]; then
        error_exit "Unfortunately, build ${PROJECT_NAME} failed"
    fi

    print_info "build ${PROJECT_NAME} project done."
    print_info "Congratulations, the compilation is successful, Modify by [${AUTHOR_NAME}]"

    print_info "Finished '${SHELL_NAME}'"

    end=`get_timestamp`
    second=`expr ${end} - ${begin}`
    min=`expr ${second} / 60`
    echo "It takes "${min}" minutes, and "${second} "seconds"
}

function imx6ull_azure_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=imx6ull
    export Platform=nxp
    export APP_NAME=azure_gui
    export USE_STDCPP_VERSION=-std=gnu++11
    generate_gitcommit

    make ${IMX6ULL_AZURE_GUI_CONFIG} && make ARCH=arm CROSS_COMPILE=${IMX6ULL_CROSS_COMPILE} -j$[$(nproc)-1]
    if [ $? -ne 0 ]; then
        error_exit "Unfortunately, build ${PROJECT_NAME} failed"
    fi

    print_info "build ${PROJECT_NAME} project done."
    print_info "Congratulations, the compilation is successful, Modify by [${AUTHOR_NAME}]"

    print_info "Finished '${SHELL_NAME}'"

    end=`get_timestamp`
    second=`expr ${end} - ${begin}`
    min=`expr ${second} / 60`
    echo "It takes "${min}" minutes, and "${second} "seconds"
}
