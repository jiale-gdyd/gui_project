#!/bin/bash

source ${TOPSHELL}/shell/buildFunc.sh
source ${TOPSHELL}/shell/buildConf.sh

HOST_UNITTEST_CONFIG=host_unittest_defconfig
HOST_LVGL_GUI_CONFIG=host_lvgl_gui_defconfig
HOST_AWTK_GUI_CONFIG=host_awtk_gui_defconfig
HOST_AZURE_GUI_CONFIG=host_azure_gui_defconfig

function host_clean()
{
    rm -rf lvgl_gui
    rm -rf awtk_gui
    rm -rf azure_gui
    rm -rf host_unittest
    remove_gitcommit
}

function host_lvgl_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=x86_64
    export Platform=host
    export APP_NAME=lvgl_gui
    export USE_STDCPP_VERSION=-std=gnu++14
    generate_gitcommit

    make ${HOST_LVGL_GUI_CONFIG} && make ARCH=x86_64 -j$[$(nproc)-1]
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

function host_awtk_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=x86_64
    export Platform=host
    export APP_NAME=awtk_gui
    export USE_STDCPP_VERSION=-std=gnu++11
    generate_gitcommit

    make ${HOST_AWTK_GUI_CONFIG} && make ARCH=x86_64 -j$[$(nproc)-1]
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

function host_azure_gui()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=x86_64
    export Platform=host
    export APP_NAME=azure_gui
    export USE_STDCPP_VERSION=-std=gnu++11
    generate_gitcommit

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

function host_unittest()
{
    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting '${SHELL_NAME}'"

    print_info "build ${PROJECT_NAME} project start"

    export SOC=x86_64
    export Platform=host
    export APP_NAME=host_unittest
    export USE_STDCPP_VERSION=-std=gnu++17
    generate_gitcommit

    make ${HOST_UNITTEST_CONFIG} && make ARCH=x86_64 -j$[$(nproc)-1]
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
