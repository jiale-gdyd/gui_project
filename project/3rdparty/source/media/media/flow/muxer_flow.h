#ifndef LIBDRM_MEDIA_MUXER_FLOW_H
#define LIBDRM_MEDIA_MUXER_FLOW_H

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

#include <media/utils/flow.h>
#include <media/utils/muxer.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>

namespace libdrm {
#define PRE_RECORD_DEBUG            0
#define MUXER_FLOW_FILE_NAME_LEN    256

typedef int (*GET_FILE_NAMES_CB)(void *handle, char *file_name, unsigned int muxer_id);

typedef enum {
    MUXER_PRE_RECORD_NONE = 0,
    MUXER_PRE_RECORD_MANUAL_SPLIT,
    MUXER_PRE_RECORD_SINGLE,
    MUXER_PRE_RECORD_NORMAL
} PRE_RECORD_MODE_E;

class VideoRecorder;

static bool save_buffer(Flow *f, MediaBufferVector &input_vector);
static bool save_buffer_lapse(Flow *f, MediaBufferVector &input_vector);
static int muxer_buffer_callback(void *handler, uint8_t *buf, int buf_size);

class MuxerFlow : public Flow {
    friend VideoRecorder;

public:
    MuxerFlow(const char *param);
    virtual ~MuxerFlow();

    static const char *GetFlowName() {
        return "muxer_flow";
    }

    virtual int Control(unsigned long int request, ...) final;

private:
    void FlushThread();
    void StartStream();
    void StopStream();

    std::shared_ptr<VideoRecorder> NewRecorder(const char *path);
    std::string GenFilePath();

    int GetVideoExtradata(std::shared_ptr<MediaBuffer> vid_buffer);
    void CheckRecordEnd(std::shared_ptr<MediaBuffer> vid_buffer);
    void DequePushBack(std::deque<std::shared_ptr<MediaBuffer>> *deque, std::shared_ptr<MediaBuffer> buffer);
    int PreRecordWrite();
    void Reset();

    friend bool save_buffer(Flow *f, MediaBufferVector &input_vector);
    friend bool save_buffer_lapse(Flow *f, MediaBufferVector &input_vector);
    friend int muxer_buffer_callback(void *handler, uint8_t *buf, int buf_size);

private:
    std::shared_ptr<MediaBuffer>             video_extra;
    std::string                              muxer_param;
    std::string                              file_prefix;
    std::string                              file_path;
    std::string                              output_format;
    std::string                              rkaudio_avdictionary;
    std::shared_ptr<VideoRecorder>           video_recorder;
    DrmMediaConfig                           vid_enc_config;
    DrmMediaConfig                           aud_enc_config;
    bool                                     video_in;
    bool                                     audio_in;
    int64_t                                  file_duration;
    int64_t                                  real_file_duration;
    int64_t                                  file_index;
    int64_t                                  last_ts;
    bool                                     file_time_en;
    bool                                     is_use_customio;
    bool                                     enable_streaming;
    bool                                     request_stop_stream;

    GET_FILE_NAMES_CB                        file_name_cb;
    void                                     *file_name_handle;
    unsigned int                             muxer_id;
    bool                                     manual_split;
    bool                                     manual_split_record;
    int64_t                                  manual_split_file_duration;
    bool                                     is_first_file;
    PRE_RECORD_MODE_E                        pre_record_mode;
    unsigned int                             pre_record_time;
    unsigned int                             pre_record_cache_time;
    std::deque<std::shared_ptr<MediaBuffer>> vid_prerecord_buffers;
    std::deque<std::shared_ptr<MediaBuffer>> aud_prerecord_buffers;
    int                                      vid_prerecord_size;
    int                                      aud_prerecord_size;
    std::deque<std::shared_ptr<MediaBuffer>> vid_cached_buffers;
    std::deque<std::shared_ptr<MediaBuffer>> aud_cached_buffers;
    ConditionLockMutex                       cached_cond_mtx;
    ConditionLockMutex                       pre_record_cond_mtx;
    bool                                     pre_record_writing;
    bool                                     is_lapse_record;
    int64_t                                  lapse_time_stamp;
    bool                                     change_config_split;
    bool                                     io_error;
    std::thread                              *flush_thread;
    bool                                     flush_thread_quit;
};

class VideoRecorder {
public:
    VideoRecorder(const char *param, Flow *f, const char *rpath, int customio);
    ~VideoRecorder();

    bool Write(MuxerFlow *f, std::shared_ptr<MediaBuffer> buffer);
    void ProcessEvent(DrmMuxerEventType type, int value);

private:
    void ClearStream();

    std::shared_ptr<Muxer> muxer;
    int                    vid_stream_id;
    int                    aud_stream_id;
    Flow                   *muxer_flow;
    std::string            record_path;
};
}

#endif
