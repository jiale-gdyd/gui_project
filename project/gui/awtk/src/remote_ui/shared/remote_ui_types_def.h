﻿/**
 * File:   remote_ui_types_def.h
 * Author: AWTK Develop Team
 * Brief:  remote ui types def
 *
 * Copyright (c) 2018 - 2023  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2023-11-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_REMOTE_UI_TYPES_DEF_H
#define TK_REMOTE_UI_TYPES_DEF_H

#include "service/msg_header.h"

BEGIN_C_DECLS

/**
 * @enum remote_ui_msg_code_t
 * @prefix REMOTE_UI_
 * 消息类型。
 * 
 */
typedef enum _remote_ui_msg_code_t {
  /**
   * @const REMOTE_UI_GET_DEV_INFO
   * 获取设备信息。
   */
  REMOTE_UI_GET_DEV_INFO = MSG_USER_START,
  /**
   * @const REMOTE_UI_REBOOT
   * 重新加载请求。
   */
  REMOTE_UI_REBOOT,
  /**
   * @const REMOTE_UI_CREATE_DIR
   * 创建目录请求。
   */
  REMOTE_UI_CREATE_DIR,  
  /**
   * @const REMOTE_UI_REMOVE_DIR
   * 删除目录请求。
   */
  REMOTE_UI_REMOVE_DIR,
  /**
   * @const REMOTE_UI_REMOVE_FILE
   * 删除文件请求。
   */
  REMOTE_UI_REMOVE_FILE,
  /**
   * @const REMOTE_UI_ON_EVENT
   * 注册事件请求。
   */
  REMOTE_UI_ON_EVENT,
  /**
   * @const REMOTE_UI_OFF_EVENT
   * 注销事件请求。
   */
  REMOTE_UI_OFF_EVENT,
  /**
   * @const REMOTE_UI_SEND_EVENT
   * 发送事件请求。
   */
  REMOTE_UI_SEND_EVENT,
  /**
   * @const REMOTE_UI_OPEN_WINDOW
   * 打开窗口请求。
   */
  REMOTE_UI_OPEN_WINDOW,
  /**
   * @const REMOTE_UI_OPEN_DIALOG
   * 打开基本对话框请求。
   */
  REMOTE_UI_OPEN_DIALOG,
  /**
   * @const REMOTE_UI_CLOSE_WINDOW
   * 关闭窗口请求。
   */
  REMOTE_UI_CLOSE_WINDOW,
  /**
   * @const REMOTE_UI_BACK_TO_PREV
   * 返回请求。
   */
  REMOTE_UI_BACK_TO_PREV,
  /**
   * @const REMOTE_UI_BACK_TO_HOME
   * 返回主页请求。
   */
  REMOTE_UI_BACK_TO_HOME,
  /**
   * @const REMOTE_UI_SET_PROP
   * 设置属性请求。
   */
  REMOTE_UI_SET_PROP,
  /**
   * @const REMOTE_UI_GET_PROP
   * 获取属性请求。
   */
  REMOTE_UI_GET_PROP,
  /**
   * @const REMOTE_UI_GET_XML_SOURCE
   * 获取xml源码请求。
   */
  REMOTE_UI_GET_XML_SOURCE,
  /**
   * @const REMOTE_UI_EXEC_FSCRIPT
   * 执行脚本请求。
   */
  REMOTE_UI_EXEC_FSCRIPT,

  /**
   * @const REMOTE_UI_MOVE_WIDGET
   * 移动控件请求。
  */
  REMOTE_UI_MOVE_WIDGET,
  /**
   * @const REMOTE_UI_RESIZE_WIDGET
   * 调整控件大小请求。
  */
  REMOTE_UI_RESIZE_WIDGET,
  /**
   * @const REMOTE_UI_CREATE_WIDGET
   * 创建控件请求。
  */
  REMOTE_UI_CREATE_WIDGET,
  /**
   * @const REMOTE_UI_DESTROY_WIDGET
   * 销毁控件请求。
  */
  REMOTE_UI_DESTROY_WIDGET,

  /**
   * @const REMOTE_UI_NOTIFY
   * 事件通知。
   */
  REMOTE_UI_NOTIFY,
} remote_ui_msg_code_t;

/**
 * @class remote_ui_dev_info_t
 * 设备信息。 
*/
typedef struct _remote_ui_dev_info_t {
  /**
   * @property {char*} name
   * 设备名称。
   */
  char name[64];
  /**
   * @property {char*} version
   * 设备版本。
   */
  char version[64];
  /**
   * @property {char*} os
   * 操作系统。
   */
  char os[16];
  
  /**
   * @property {char*} arch
   * CPU架构。
   */
  char arch[16];

  /**
   * @property {uint32_t} screen_width
   * 屏幕宽度。
   */
  uint32_t screen_width;
  /**
   * @property {uint32_t} screen_height
   * 屏幕高度。
   */
  uint32_t screen_height;
  /**
   * @property {uint32_t} dpi
   * 屏幕DPI。
   */
  uint32_t dpi;
} remote_ui_dev_info_t;

/**
 * @enum remote_ui_reboot_type_t
 * @prefix REMOTE_UI_REBOOT_
 * 重启类型。
*/
typedef enum _remote_ui_reboot_type_t {
  /**
   * @const REMOTE_UI_REBOOT_DEFAULT
   * 重启。
   */
  REMOTE_UI_REBOOT_DEFAULT = 0,
  /**
   * @const REMOTE_UI_REBOOT_RELOAD
   * 重新加载。
   */
  REMOTE_UI_REBOOT_RELOAD,
  /**
   * @const REMOTE_UI_REBOOT_POWEROFF
   * 关机。
   */
  REMOTE_UI_REBOOT_POWEROFF
} remote_ui_reboot_type_t;

#define REMOTE_UI_KEY_USERNAME "username"
#define REMOTE_UI_KEY_PASSWORD "password"

#define REMOTE_UI_KEY_DEV_NAME "dev_name"
#define REMOTE_UI_KEY_DEV_OS "dev_os"
#define REMOTE_UI_KEY_DEV_ARCH "dev_arch"
#define REMOTE_UI_KEY_DEV_VERSION "dev_version"
#define REMOTE_UI_KEY_DEV_SCREEN_WIDTH "dev_screen_width"
#define REMOTE_UI_KEY_DEV_SCREEN_HEIGHT "dev_screen_height"
#define REMOTE_UI_KEY_DEV_DPI "dev_dpi"
#define REMOTE_UI_KEY_TARGET "target"
#define REMOTE_UI_KEY_NAME "name"
#define REMOTE_UI_KEY_VALUE "value"
#define REMOTE_UI_KEY_EVENT "event"
#define REMOTE_UI_KEY_XML "xml"
#define REMOTE_UI_KEY_INIT "init"
#define REMOTE_UI_KEY_X "x"
#define REMOTE_UI_KEY_Y "y"
#define REMOTE_UI_KEY_W "w"
#define REMOTE_UI_KEY_H "h"
#define REMOTE_UI_KEY_CODE "code"
#define REMOTE_UI_KEY_TYPE "type"
#define REMOTE_UI_KEY_TITLE "title"
#define REMOTE_UI_KEY_CONTENT "content"
#define REMOTE_UI_KEY_DURATION "duration"
#define REMOTE_UI_FILE_MANIFEST "__manifest__.txt"
#define REMOTE_UI_FILE_XML_SOURCE "__xml_source__.xml"
#define REMOTE_UI_FILE_SNAPSHOT "__snapshot__."

#define REMOTE_UI_FILE_LOADED_IMAGES_INFO "__images_info__"
#define REMOTE_UI_FILE_LOADED_ASSETS_INFO "__assets_info__"

#define REMOTE_UI_DIALOG_TYPE_CONFIRM "confirm"
#define REMOTE_UI_DIALOG_TYPE_WARN "warn"
#define REMOTE_UI_DIALOG_TYPE_INFO "info"
#define REMOTE_UI_DIALOG_TYPE_TOAST "toast"

#define REMOTE_UI_TARGET_GLOBAL "global"
#define REMOTE_UI_PROP_THEME "theme"
#define REMOTE_UI_PROP_LANGUAGE "language"

#ifndef REMOTE_UI_URL
#define REMOTE_UI_URL "tcp://localhost:2233"
#endif/*REMOTE_UI_URL*/

END_C_DECLS

#endif /*TK_REMOTE_UI_TYPES_DEF_H*/
