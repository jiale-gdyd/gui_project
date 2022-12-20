#!/bin/bash

function gen_now_timestamp()
{
    echo $(date "+%Y-%m-%d %H:%M:%S")
}

function get_now_timestamp()
{
    time=`gen_now_timestamp`
    echo ${time}
}

function print_warn()
{
    printf "\033[33m[WARN ] `get_now_timestamp` --> $*\033[0m\n";
}

function print_info()
{
    printf "\033[32m[INFO ] `get_now_timestamp` --> $*\033[0m\n";
}

function print_error()
{
    printf "\033[31m[ERROR] `get_now_timestamp` --> $*\033[0m\n";
}

function error_exit() {
    printf "\033[31m[ERROR] `get_now_timestamp` --> $*\033[0m\n";
    exit 1;
}

# 获取linux发行版名称
function get_linux_distro()
{
    if grep -Eq "Ubuntu" /etc/*-release; then
        echo "Ubuntu"
    elif grep -Eq "Deepin" /etc/*-release; then
        echo "Deepin"
    elif grep -Eq "LinuxMint" /etc/*-release; then
        echo "LinuxMint"
    elif grep -Eq "elementary" /etc/*-release; then
        echo "elementaryOS"
    elif grep -Eq "Debian" /etc/*-release; then
        echo "Debian"
    elif grep -Eq "Kali" /etc/*-release; then
        echo "Kali"
    elif grep -Eq "CentOS" /etc/*-release; then
        echo "CentOS"
    elif grep -Eq "fedora" /etc/*-release; then
        echo "fedora"
    elif grep -Eq "openSUSE" /etc/*-release; then
        echo "openSUSE"
    elif grep -Eq "Arch Linux" /etc/*-release; then
        echo "ArchLinux"
    elif grep -Eq "ManjaroLinux" /etc/*-release; then
        echo "ManjaroLinux"
    else
        echo "Unknow"
    fi
}

# 获取日期
function get_datetime()
{
    time=$(date "+%Y%m%d%H%M%S")
    echo $time
}

# 获取当前时间戳
function get_timestamp()
{
    cur_sec_and_ns=`date '+%s-%N'`
    echo ${cur_sec_and_ns%-*}
}

# 判断文件是否存在
function is_exist_file()
{
    filename=$1
    if [ -f $filename ]; then
        echo 1
    else
        echo 0
    fi
}

# 判断目录是否存在
function is_exist_dir()
{
    dir=$1
    if [ -d $dir ]; then
        echo 1
    else
        echo 0
    fi
}

# 获取ubuntu版本
function get_ubuntu_version()
{
    line=$(cat /etc/lsb-release | grep "DISTRIB_RELEASE")
    arr=(${line//=/ })
    version=(${arr[1]//./ })

    echo ${version[0]}
}

# 获取通用版本
function get_general_version()
{
    version1=$(grep VERSION_ID /etc/os-release| sed 's/VERSION_ID=//')
    version=$(echo ${version1:1:-1})
    echo ${version}
}

# 获取Debian版本
function get_debian_version()
{
    version=$(cat /etc/debian_version)
    echo ${version}
}

# 获取centos版本
function get_centos_version()
{
    version=`cat /etc/redhat-release | awk '{print $4}' | awk -F . '{printf "%s",$1}'`
    echo $version
}

# 判断是否是macos10.14版本
function is_macos1014()
{
    product_version=$(sw_vers | grep ProductVersion)
    if [[ $product_version =~ "10.14" ]]; then
        echo 1
    else
        echo 0
    fi
}
