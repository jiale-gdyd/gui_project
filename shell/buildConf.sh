#!/bin/bash

export STAGING_DIR=""
export AUTHOR_NAME=ljlaych@163.com
export CPU_CORES=`grep -c processor /proc/cpuinfo`
export GIT_COMMIT_SHA1=`git log --oneline --date=short --format='%h author: %cn' -1`

SHELL_NAME=make.sh
PROJECT_NAME=jialelu
GIT_REPO_DIR=${TOPSHELL}/.git
GITCOMMITFILE=include/linux/git_commit.h

# 生成git commit信息到版本显示
function generate_gitcommit()
{
    if [ -d ${GIT_REPO_DIR} ]; then
        echo "#define GIT_SHORT_COMMIT \"${GIT_COMMIT_SHA1}\"" > ${GITCOMMITFILE}
    else
        echo "#define GIT_SHORT_COMMIT \"Not a git repository\"" > ${GITCOMMITFILE}
    fi
}

# 移除git commit信息
function remove_gitcommit()
{
    rm -rf ${GITCOMMITFILE}
}
