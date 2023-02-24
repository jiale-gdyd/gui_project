#ifndef RTSP_SIMPLE_RTSP_SERVER_H
#define RTSP_SIMPLE_RTSP_SERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum rtspCodecFlags {
    RTSP_CODEC_FLAGS_H264 = 0,
    RTSP_CODEC_FLAGS_H265
};

enum rtspCodecIdEnum {
    RTSP_CODEC_ID_NONE        = 0,
    RTSP_CODEC_ID_VIDEO_H264  = 0x0001,     /* codec_data为SPS + PPS帧 */
    RTSP_CODEC_ID_VIDEO_H265,               /* codec_data为VPS + SPS + PPS帧 */
    RTSP_CODEC_ID_VIDEO_MPEG4,              /* 现在不支持 */
    RTSP_CODEC_ID_AUDIO_G711A = 0x4001,     /* codec_data为NULL */
    RTSP_CODEC_ID_AUDIO_G711U,              /* codec_data为NULL */
    RTSP_CODEC_ID_AUDIO_G726,               /* codec_data是比特率(int) */
    RTSP_CODEC_ID_AUDIO_AAC,                /* codec_data是音频特定配置(2字节)。帧类型为ADTS */
};

typedef void* simple_rtsp_handle_t;
typedef void* simple_rtsp_session_t;

/**
 * 接口调用顺序:
 * simple_create_rtsp_server
 *   -> simple_create_rtsp_session
 *      -> simple_rtsp_set_video
 *         -> simple_rtsp_sync_video_timestamp
 *            -> simple_rtsp_send_video
 *               -> simple_rtsp_wait_frame_finished
 *                  -> simple_rtsp_delete_handle
 *                     -> simple_rtsp_delete_session
 */

/**
 * 函数名称: simple_create_rtsp_server
 * 功能描述: 创建一个轻量级的rtsp服务器
 * 输入参数: port --> RTSP服务器监听端口号
 * 输出参数: 无
 * 返回说明: 成功返回创建的RTSP服务器句柄，否则为NULL
 */
simple_rtsp_handle_t simple_create_rtsp_server(uint16_t port);

/**
 * 函数名称: simple_rtsp_wait_frame_finished
 * 功能描述: 等待帧数据发送完毕
 * 输入参数: handle --> RTSP服务器句柄
 * 输出参数: 无
 * 返回说明: 成功返回1，其他则失败
 */
int simple_rtsp_wait_frame_finished(simple_rtsp_handle_t handle);

/**
 * 函数名称: simple_create_rtsp_session
 * 功能描述: 根据创建的RTSP服务器句柄创建RTSP会话
 * 输入参数: handle        --> RTSP服务器句柄
 *          path          --> 服务流地址
 *          encoder_flags --> 编码标识(0: h264, 1: h265, 其他不支持)
 * 输出参数: 无
 * 返回说明: 成功返回RTSP会话句柄，否则返回NULL
 */
simple_rtsp_session_t simple_create_rtsp_session(simple_rtsp_handle_t handle, const char *path, enum rtspCodecFlags encoder_flags);

/**
 * 函数名称: simple_rtsp_set_video
 * 功能描述: 设置视频RTSP预参数
 * 输入参数: session    --> RTSP服务会话句柄
 *          codec_id   --> 编码ID
 *          codec_data --> 编码数据
 *          data_len   --> 编码数据字节数
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int simple_rtsp_set_video(simple_rtsp_session_t session, enum rtspCodecIdEnum codec_id, const uint8_t *codec_data, int data_len);

/**
 * 函数名称: simple_rtsp_set_audio
 * 功能描述: 设置音频RTSP预参数
 * 输入参数: session    --> RTSP服务会话句柄
 *          codec_id   --> 编码ID
 *          codec_data --> 编码数据
 *          data_len   --> 编码数据字节数
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int simple_rtsp_set_audio(simple_rtsp_session_t session, enum rtspCodecIdEnum codec_id, const uint8_t *codec_data, int data_len);

/**
 * 函数名称: simple_rtsp_send_video
 * 功能描述: 向RTSP服务中发送视频数据帧
 * 输入参数: session --> RTSP服务会话句柄
 *          frame   --> 待发送的已编码数据帧
 *          len     --> 待发送的已编码数据帧字节数
 *          ts      --> 当前时间戳
 * 输出参数: 无
 * 返回说明: 成功返回发送字节数，其他则失败
 */
int simple_rtsp_send_video(simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts);

/**
 * 函数名称: simple_rtsp_send_audio
 * 功能描述: 向RTSP服务中发送音频数据帧
 * 输入参数: session --> RTSP服务会话句柄
 *          frame   --> 待发送的已编码数据帧
 *          len     --> 待发送的已编码数据帧字节数
 *          ts      --> 当前时间戳
 * 输出参数: 无
 * 返回说明: 成功返回发送字节数，其他则失败
 */
int simple_rtsp_send_audio(simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts);

/**
 * 函数名称: simple_rtsp_server_send_video
 * 功能描述: 向RTSP服务中发送视频数据帧
 * 输入参数: handle  --> RTSP服务句柄
 *          session --> RTSP服务会话句柄
 *          frame   --> 待发送的已编码数据帧
 *          len     --> 待发送的已编码数据帧字节数
 *          ts      --> 当前时间戳
 * 输出参数: 无
 * 返回说明: 成功返回发送字节数，其他则失败
 */
int simple_rtsp_server_send_video(simple_rtsp_handle_t handle, simple_rtsp_session_t session, const uint8_t *frame, int len, uint64_t ts);

/**
 * 函数名称: simple_rtsp_delete_handle
 * 功能描述: 释放RTSP服务句柄
 * 输入参数: handle --> RTSP服务句柄
 * 输出参数: 无
 * 返回说明: 无
 */
void simple_rtsp_delete_handle(simple_rtsp_handle_t handle);

/**
 * 函数名称: simple_rtsp_delete_session
 * 功能描述: 释放RTSP服务会话句柄
 * 输入参数: session --> RTSP会话句柄
 * 输出参数: 无
 * 返回说明: 无
 */
void simple_rtsp_delete_session(simple_rtsp_session_t session);

/**
 * 函数名称: simple_rtsp_get_reltime
 * 功能描述: 获取相对时间戳
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 相对时间戳
 */
uint64_t simple_rtsp_get_reltime(void);

/**
 * 函数名称: simple_rtsp_get_ntptime
 * 功能描述: 获取NTP时间戳
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: NTP时间戳
 */
uint64_t simple_rtsp_get_ntptime(void);

/**
 * 函数名称: simple_rtsp_sync_video_timestamp
 * 功能描述: 同步RTSP视频时间戳
 * 输入参数: session --> RTSP服务会话句柄
 *          ts      --> 时间戳
 *          ntptime --> NTP时间戳
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int simple_rtsp_sync_video_timestamp(simple_rtsp_session_t session, uint64_t ts, uint64_t ntptime);

/**
 * 函数名称: simple_rtsp_sync_audio_timestamp
 * 功能描述: 同步RTSP音频时间戳
 * 输入参数: session --> RTSP服务会话句柄
 *          ts      --> 时间戳
 *          ntptime --> NTP时间戳
 * 输出参数: 无
 * 返回说明: 成功返回0，其他则失败
 */
int simple_rtsp_sync_audio_timestamp(simple_rtsp_session_t session, uint64_t ts, uint64_t ntptime);

#ifdef __cplusplus
}
#endif

#endif
