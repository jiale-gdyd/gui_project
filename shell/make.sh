#!/bin/bash

source ${TOPSHELL}/shell/make_host.sh
source ${TOPSHELL}/shell/make_rv11xx.sh
source ${TOPSHELL}/shell/make_ax620a.sh
source ${TOPSHELL}/shell/make_imx6ull.sh

function clean()
{
    clear

    begin=`get_timestamp`
    type=$(uname)
    distro=`get_linux_distro`
    version=$(get_general_version)
    echo "Platform type: "${type}" "${distro}" "${version}

    print_info "Starting distclean"
    print_info "distclean ${PROJECT_NAME} project start"

    host_clean
    ax620a_clean
    rv11xx_clean
    imx6ull_clean

    make distclean -j$[$(nproc)-1]
    if [ $? -ne 0 ]; then
        print_error "distclean project failed, maybe there nothing can be done."
    fi

    print_info "distclean ${PROJECT_NAME} project done."
    print_info "Finished distclean"

    end=`get_timestamp`
    second=`expr ${end} - ${begin}`
    min=`expr ${second} / 60`
    echo "It takes "${min}" minutes, and "${second} "seconds"

    echo "Platform type: "${type}" "${distro}" "${version}
}

$1
