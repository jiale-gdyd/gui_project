#!/bin/bash

source ${TOPSHELL}/shell/buildFunc.sh
source ${TOPSHELL}/shell/buildConf.sh

HOST_AZURE_GUI_CONFIG=host_azure_gui_defconfig

function host_clean()
{
    rm -rf host_lvgl_gui
    rm -rf host_azure_gui
}

function host_azure_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    if [ ${distro} == "Ubuntu" ]; then
        version=$(get_ubuntu_version)
    elif [ ${distro} == "LinuxMint" ]; then
        version=$(get_ubuntu_version)
    elif [ ${distro} == "CentOS" ]; then
        version=$(get_centos_version)
    fi
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=x86_64
    export Platform=host
    export APP_NAME=host_azure_gui
    export USE_STDCPP_VERSION=-std=c++11

    make ${HOST_AZURE_GUI_CONFIG} && make ARCH=x86_64 -j$[$(nproc)-1]
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