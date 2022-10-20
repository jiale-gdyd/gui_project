/*
 * Copyright 2015 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define MODULE_TAG "mpp_info"

#include <stdlib.h>
#include <string.h>

#include "rockchip/rkmpp/mpp_log.h"
#include "../osal/inc/mpp_env.h"
#include "inc/mpp_info.h"

static const char *mpp_version = "rockchip-rkmpp - " __DATE__ " " __TIME__;

void show_mpp_version(void)
{

}

const char *get_mpp_version(void)
{
    return mpp_version;
}
