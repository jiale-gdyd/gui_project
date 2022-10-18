#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/time.h>

#include <media/utils/flow.h>
#include <media/utils/codec.h>
#include <media/utils/muxer.h>
#include <media/utils/utils.h>
#include <media/utils/buffer.h>

#include <sstream>

#include "muxer_flow.h"

namespace libdrm {
static int64_t sg_last_time = 0;
static unsigned sg_buffer_size = 0;
static unsigned sg_buffer_count = 0;

static int muxer_buffer_callback(void *handler, uint8_t *buf, int buf_size)
{
    MuxerFlow *f = (MuxerFlow *)handler;
    auto media_buffer = MediaBuffer::Alloc(buf_size);

    memcpy(media_buffer->GetPtr(), buf, buf_size);
    f->GetInputSize();
    media_buffer->SetValidSize(buf_size);
    media_buffer->SetUSTimeStamp(libdrm::gettimeofday());
    f->SetOutput(media_buffer, 0);

    int64_t cur_time = libdrm::gettimeofday();
    sg_buffer_size += buf_size;
    sg_buffer_count++;
    if ((cur_time - sg_last_time) / 1000 > 1000) {
        DRM_MEDIA_LOGI("MUXER:: one second output buffer size = %u, count = %u, last_size = %u", sg_buffer_size, sg_buffer_count, buf_size);

        sg_buffer_size = 0;
        sg_last_time = cur_time;
        sg_buffer_count = 0;
    }

    return buf_size;
}

MuxerFlow::MuxerFlow(const char *param)
    : video_recorder(nullptr), video_in(false), audio_in(false),
    file_duration(0), real_file_duration(-1), file_index(-1), last_ts(0),
    file_time_en(false), enable_streaming(true), request_stop_stream(false),
    file_name_cb(nullptr), file_name_handle(nullptr), muxer_id(0),
    manual_split(false), manual_split_record(false),
    manual_split_file_duration(0), is_first_file(true),
    pre_record_mode(MUXER_PRE_RECORD_NONE), pre_record_time(0),
    pre_record_cache_time(0), vid_prerecord_size(0), aud_prerecord_size(0),
    is_lapse_record(false), lapse_time_stamp(0), change_config_split(false),
    io_error(false), flush_thread(nullptr)
{
    std::list<std::string> separate_list;
    std::map<std::string, std::string> params;

    if (!ParseWrapFlowParams(param, params, separate_list)) {
        SetError(-EINVAL);
        return;
    }

    std::string &muxer_name = params[DRM_KEY_NAME];
    if (muxer_name.empty()) {
        DRM_MEDIA_LOGE("%s: missing muxer name", __func__);
        SetError(-EINVAL);
        return;
    }

    file_path = params[DRM_KEY_PATH];
    if (!file_path.empty()) {
        DRM_MEDIA_LOGI("Muxer will use internal path");
        is_use_customio = false;
    } else {
        is_use_customio = true;
        DRM_MEDIA_LOGI("Muxer:: file_path is null, will use CustomeIO");
    }

    file_prefix = params[DRM_KEY_FILE_PREFIX];
    if (file_prefix.empty()) {
        DRM_MEDIA_LOGI("Muxer will use default prefix\n");
    }

    std::string time_str = params[DRM_KEY_FILE_TIME];
    if (!time_str.empty()) {
        file_time_en = !!std::stoi(time_str);
        DRM_MEDIA_LOGI("Muxer will record video end with time");
    }

    std::string index_str = params[DRM_KEY_FILE_INDEX];
    if (!index_str.empty()) {
        file_index = std::stoi(index_str);
        DRM_MEDIA_LOGI("Muxer will record video start with index %" PRId64 "", file_index);
    }

    std::string &duration_str = params[DRM_KEY_FILE_DURATION];
    if (!duration_str.empty()) {
        file_duration = std::stoi(duration_str);
        DRM_MEDIA_LOGI("Muxer will save video file per %" PRId64 "sec", file_duration);
    }

    std::string pre_record_mode_str = params[DRM_KEY_PRE_RECORD_MODE];
    if (!pre_record_mode_str.empty()) {
        pre_record_mode = (PRE_RECORD_MODE_E)std::stoi(pre_record_mode_str);
        DRM_MEDIA_LOGI("Muxer: pre_record_mode %d", pre_record_mode);
    }

    std::string &pre_record_time_str = params[DRM_KEY_PRE_RECORD_TIME];
    if (!pre_record_time_str.empty()) {
        pre_record_time = std::stoi(pre_record_time_str);
        DRM_MEDIA_LOGI("Muxer: pre-record time %d(s)", pre_record_time);
    }

    std::string &pre_record_cache_time_str = params[DRM_KEY_PRE_RECORD_CACHE_TIME];
    if (!pre_record_cache_time_str.empty()) {
        pre_record_cache_time = std::stoi(pre_record_cache_time_str);
        if (pre_record_cache_time < pre_record_time) {
            pre_record_cache_time = pre_record_time;
        }

        DRM_MEDIA_LOGI("Muxer: pre-record cache time %d(s)", pre_record_cache_time);
    }

    std::string &muxer_id_str = params[DRM_KEY_MUXER_ID];
    if (!muxer_id_str.empty()) {
        muxer_id = std::stoi(muxer_id_str);
        DRM_MEDIA_LOGI("Muxer: muxer_id %d", muxer_id);
    }

    std::string lapse_record_str = params[DRM_KEY_LAPSE_RECORD];
    if (!lapse_record_str.empty()) {
        is_lapse_record = !!std::stoi(lapse_record_str);
        DRM_MEDIA_LOGI("Muxer: is_lapse_record %d", is_lapse_record);
    }

    output_format = params[DRM_KEY_OUTPUTDATATYPE];
    if (output_format.empty() && is_use_customio) {
        DRM_MEDIA_LOGI("Muxer: output_data_type is null, no use customio.\n");
        is_use_customio = false;
    }

    std::string enable_streaming_s = params[DRM_KEY_ENABLE_STREAMING];
    if (!enable_streaming_s.empty()) {
        if (!enable_streaming_s.compare("false"))
        enable_streaming = false;
        else
        enable_streaming = true;
    }
    DRM_MEDIA_LOGI("Muxer:: enable_streaming is %d", enable_streaming);

    rkaudio_avdictionary = params[DRM_KEY_MUXER_RKAUDIO_AVDICTIONARY];

    for (auto param_str : separate_list) {
        DrmMediaConfig enc_config;
        std::map<std::string, std::string> enc_params;
        if (!parse_media_param_map(param_str.c_str(), enc_params)) {
            continue;
        }

        if (!ParseMediaConfigFromMap(enc_params, enc_config)) {
            continue;
        }

        if (enc_config.type == DrmType::Video) {
            vid_enc_config = enc_config;
            video_in = true;
            DRM_MEDIA_LOGI("Found video encode config");
        } else if (enc_config.type == DrmType::Audio) {
            aud_enc_config = enc_config;
            audio_in = true;
            DRM_MEDIA_LOGI("Found audio encode config");
        }
    }

    std::string token;
    std::istringstream tokenStream(param);
    std::getline(tokenStream, token, FLOW_PARAM_SEPARATE_CHAR);
    muxer_param = token;

    SlotMap sm;
    sm.input_slots.push_back(0);
    sm.input_slots.push_back(1);
    if (is_use_customio) {
        sm.output_slots.push_back(0);
    }
    sm.thread_model = Model::ASYNCCOMMON;
    sm.mode_when_full = InputMode::BLOCKING;
    sm.input_maxcachenum.push_back(20);
    sm.input_maxcachenum.push_back(40);
    sm.fetch_block.push_back(false);
    sm.fetch_block.push_back(false);
    if (is_lapse_record) {
        sm.process = save_buffer_lapse;
    } else {
        sm.process = save_buffer;
    }

    if (!InstallSlotMap(sm, "MuxerFlow", 0)) {
        DRM_MEDIA_LOGI("Fail to InstallSlotMap for MuxerFlow");
        return;
    }

    if (!is_lapse_record) {
        flush_thread_quit = false;
        flush_thread = new std::thread(&MuxerFlow::FlushThread, this);
        if (!flush_thread) {
            DRM_MEDIA_LOGE("FATAL: Create FluashThread fail");
            request_stop_stream = true;
        }
    }

    SetFlowTag("MuxerFlow");
}

MuxerFlow::~MuxerFlow()
{
    if (flush_thread != nullptr) {
        cached_cond_mtx.lock();
        flush_thread_quit = true;
        DRM_MEDIA_LOGW("Ask flush thread to quit");

        cached_cond_mtx.notify();
        cached_cond_mtx.unlock();
        flush_thread->join();
        DRM_MEDIA_LOGW("join(quit) flush thread completely");

        delete flush_thread;
        flush_thread = nullptr;
    }

    StopAllThread();
    video_recorder = nullptr;
    video_extra = nullptr;
    vid_prerecord_buffers.clear();
    aud_prerecord_buffers.clear();
    vid_cached_buffers.clear();
    aud_cached_buffers.clear();

    DRM_MEDIA_LOGW("Destory MuxerFlow");
}

std::shared_ptr<VideoRecorder> MuxerFlow::NewRecorder(const char *path)
{
    std::string param = std::string(muxer_param);
    std::shared_ptr<VideoRecorder> vrecorder = nullptr;

    PARAM_STRING_APPEND(param, DRM_KEY_OUTPUTDATATYPE, output_format.c_str());
    PARAM_STRING_APPEND(param, DRM_KEY_PATH, path);
    PARAM_STRING_APPEND(param, DRM_KEY_MUXER_RKAUDIO_AVDICTIONARY, rkaudio_avdictionary);

    if (is_use_customio) {
        vrecorder = std::make_shared<VideoRecorder>(param.c_str(), this, path, 1);
        DRM_MEDIA_LOGI("use customio, output foramt is %s", output_format.c_str());
    } else {
        vrecorder = std::make_shared<VideoRecorder>(param.c_str(), this, path, 0);
    }

    if (!vrecorder) {
        DRM_MEDIA_LOGI("FATAL: Create video recoder failed, path:[%s]", path);
        assert(vrecorder != nullptr);
        return nullptr;
    } else {
        DRM_MEDIA_LOGI("Ready to recod new video file path:[%s]", path);
    }

    return vrecorder;
}

std::string MuxerFlow::GenFilePath()
{
    std::ostringstream ostr;

    if (file_name_cb) {
        char new_name[MUXER_FLOW_FILE_NAME_LEN] = {0};
        int ret = (*file_name_cb)(file_name_handle, new_name, muxer_id);
        if (!ret) {
            return std::string(new_name);
        } else {
            DRM_MEDIA_LOGE("%s: file name callback error", __func__);
        }
    }

    if (!file_path.empty() && file_prefix.empty()) {
        return file_path;
    }

    if (!file_path.empty()) {
        ostr << file_path;
        ostr << "/";
    }

    if (!file_prefix.empty()) {
        ostr << file_prefix;
    }

    if (file_time_en) {
        time_t t = time(NULL);
        char time_str[128] = {0};
        struct tm tm = *localtime(&t);

        snprintf(time_str, 128, "_%d%02d%02d%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        ostr << time_str;
    }

    if (file_index > 0) {
        ostr << "_" << file_index;
        file_index++;
    }

    ostr << ".mp4";
    return ostr.str();
}

int MuxerFlow::GetVideoExtradata(std::shared_ptr<MediaBuffer> vid_buffer)
{
    int extra_size = 0;
    void *extra_ptr = NULL;
    DrmCodecType c_type = vid_enc_config.vid_cfg.image_cfg.codec_type;

    if (c_type == DRM_CODEC_TYPE_H264) {
        extra_ptr = GetSpsPpsFromBuffer(vid_buffer, extra_size, c_type);
    } else if (c_type == DRM_CODEC_TYPE_H265) {
        extra_ptr = GetVpsSpsPpsFromBuffer(vid_buffer, extra_size, c_type);
    }

    if (extra_ptr && (extra_size > 0)) {
        video_extra = MediaBuffer::Alloc(extra_size);
        if (!video_extra) {
            DRM_LOG_MEMORY();
            return -1;
        }

        memcpy(video_extra->GetPtr(), extra_ptr, extra_size);
        video_extra->SetValidSize(extra_size);
    } else {
        DRM_MEDIA_LOGE("Muxer Flow: Intra Frame without sps pps");
    }

    return 0;
}

int MuxerFlow::Control(unsigned long int request, ...)
{
    va_list vl;
    int ret = 0;

    va_start(vl, request);

    switch (request) {
        case S_START_SRTEAM: {
            cached_cond_mtx.lock();
            StartStream();
            cached_cond_mtx.unlock();
        }
        break;

        case S_STOP_SRTEAM: {
            request_stop_stream = true;
            cached_cond_mtx.notify();
            DRM_MEDIA_LOGI("Muxer: request to stop stream");
        }
        break;

        case S_MANUAL_SPLIT_STREAM: {
            int duration = va_arg(vl, int);

            cached_cond_mtx.lock();
            if (duration > 0 && enable_streaming) {
                DRM_MEDIA_LOGI("Muxer: manual split file duration = %d", duration);
                manual_split_file_duration = duration;
                manual_split = true;
            }

            cached_cond_mtx.notify();
            cached_cond_mtx.unlock();
        }
        break;

        case G_MUXER_GET_STATUS: {
            int *value = va_arg(vl, int *);
            if (value) {
                *value = enable_streaming ? 1 : 0;
            }
        }
        break;

        case S_MUXER_FILE_DURATION: {
            int duration = va_arg(vl, int);
            DRM_MEDIA_LOGI("Muxer:: file_duration is %d", duration);
            if (duration > 0) {
                file_duration = duration;
            }
        }
        break;

        case S_MUXER_FILE_PATH: {
            std::string path = va_arg(vl, std::string);
            DRM_MEDIA_LOGI("Muxer:: file_path is %s", path.c_str());
            if (!path.empty()) {
                file_path = path;
            }
        }
        break;

        case S_MUXER_FILE_PREFIX: {
            std::string prefix = va_arg(vl, std::string);
            DRM_MEDIA_LOGI("Muxer:: file_prefix is %s", prefix.c_str());
            if (!prefix.empty())
            file_prefix = prefix;
        }
        break;

        case S_MUXER_FILE_NAME_CB: {
            file_name_cb = va_arg(vl, GET_FILE_NAMES_CB);
            file_name_handle = va_arg(vl, void *);
            DRM_MEDIA_LOGI("Muxer:: file_name_cb is %p, file_name_handle is %p", file_name_cb, file_name_handle);
        }
        break;

        case S_MUXER_SET_FPS: {
            int fps = va_arg(vl, int);
            int split = va_arg(vl, int);
            if (fps > 0 && vid_enc_config.vid_cfg.frame_rate != fps) {
                vid_enc_config.vid_cfg.frame_rate = fps;
                if (split) {
                    change_config_split = true;
                }

                DRM_MEDIA_LOGI("Muxer:: change fps: %d, change_config_split: %d", fps, change_config_split);
            }
        }
        break;

        default:
            ret = -1;
            break;
    }

    va_end(vl);
    return ret;
}

void MuxerFlow::StartStream()
{
    if (event_callback_ && !enable_streaming) {
        DrmMuxerEvent muxer_event;
        memset(&muxer_event, 0, sizeof(DrmMuxerEvent));
        muxer_event.type = MUX_EVENT_STREAM_START;
        event_callback_(event_handler2_, (void *)&muxer_event);
        is_first_file = true;
    }

    enable_streaming = true;
}

void MuxerFlow::StopStream()
{
    if (video_recorder) {
        video_recorder.reset();
        video_recorder = nullptr;
        video_extra = nullptr;
    }

    manual_split = false;
    manual_split_record = false;
    change_config_split = false;
    vid_prerecord_buffers.clear();
    aud_prerecord_buffers.clear();
    vid_cached_buffers.clear();
    aud_cached_buffers.clear();

    if (enable_streaming) {
        if (event_callback_) {
            DrmMuxerEvent muxer_event;
            memset(&muxer_event, 0, sizeof(DrmMuxerEvent));
            muxer_event.type = MUX_EVENT_STREAM_STOP;
            event_callback_(event_handler2_, (void *)&muxer_event);
        }
    }
}

void MuxerFlow::Reset()
{
    video_recorder.reset();
    video_recorder = nullptr;
    video_extra = nullptr;
    is_first_file = false;
    manual_split = false;
    manual_split_record = false;
    change_config_split = false;
    io_error = false;
}

void MuxerFlow::CheckRecordEnd(std::shared_ptr<MediaBuffer> vid_buffer)
{
    int duration_s;

    if (manual_split_record) {
        duration_s = manual_split_file_duration;
    } else {
        duration_s = file_duration;
    }

    if (!video_in || video_recorder == nullptr || vid_buffer == nullptr) {
        return;
    }

    if (!(vid_buffer->GetUserFlag() & MediaBuffer::kIntra)) {
        return;
    }

    if (manual_split) {
        DRM_MEDIA_LOGW("%d: manual_split: record end", muxer_id);
        Reset();
        manual_split_record = true;
    } else if (io_error) {
        DRM_MEDIA_LOGW("%d: io_error: record end", muxer_id);
        Reset();
    } else if (last_ts != 0) {
        bool is_end;
        int frame_interval = 1000000 / vid_enc_config.vid_cfg.frame_rate;
        int64_t total_time = vid_buffer->GetUSTimeStamp() - last_ts;

        is_end = total_time > duration_s * 1000000 - frame_interval;

        if (is_end || change_config_split) {
            DRM_MEDIA_LOGW("%u: duration: %lld, total_time: %lld, change_config_split: %d", muxer_id, (long long)real_file_duration, (long long)total_time, change_config_split);
            Reset();
        }
    }
}

void MuxerFlow::DequePushBack(std::deque<std::shared_ptr<MediaBuffer>> *deque, std::shared_ptr<MediaBuffer> buffer)
{
    unsigned int max_size;

    max_size = vid_enc_config.vid_cfg.frame_rate * pre_record_cache_time + 1;
    if (buffer == nullptr) {
        DRM_MEDIA_LOGE("%s: clone mb failed", __func__);
        return;
    }

    deque->push_back(buffer);
    while (1) {
        auto front = deque->front();
        auto back = deque->back();
        int64_t cache_duration = back->GetUSTimeStamp() - front->GetUSTimeStamp();

        if (deque->size() > max_size * 2 || cache_duration > pre_record_cache_time * 1000000) {
            deque->pop_front();
        } else {
            break;
        }
    }
}

int MuxerFlow::PreRecordWrite()
{
    unsigned int size = 0;
    bool is_pre_record = false;
    std::shared_ptr<MediaBuffer> aud_buffer = nullptr;
    std::shared_ptr<MediaBuffer> vid_buffer = nullptr;

    if (video_recorder == nullptr) {
        return 0;
    }

    switch (pre_record_mode) {
        case MUXER_PRE_RECORD_MANUAL_SPLIT:
            is_pre_record = manual_split_record && (pre_record_time > 0);
            break;

        case MUXER_PRE_RECORD_SINGLE:
            is_pre_record = is_first_file && (pre_record_time > 0);
            break;

        case MUXER_PRE_RECORD_NORMAL:
            is_pre_record = pre_record_time > 0 ? true : false;
            break;

        default:
            return 0;
    }

    if (!is_pre_record) {
        return 0;
    }

    size = vid_prerecord_buffers.size();

    DRM_MEDIA_LOGW("%d: to pre_record by: mode: %d, manual_split: %d, pre_record_time: %d, first_file: %d", muxer_id, pre_record_mode, manual_split_record, pre_record_time, is_first_file);
    DRM_MEDIA_LOGI("video pre_record size: %d", size);

    vid_cached_buffers.clear();
    aud_cached_buffers.clear();

    while (vid_prerecord_buffers.size() > 0) {
        vid_buffer = vid_prerecord_buffers.front();
        vid_prerecord_buffers.pop_front();
        vid_cached_buffers.push_back(vid_buffer);
    }

    while (aud_prerecord_buffers.size() > 0) {
        aud_buffer = aud_prerecord_buffers.front();
        aud_cached_buffers.push_back(aud_buffer);
        aud_prerecord_buffers.pop_front();
    }

    return 1;
}

static bool save_buffer_lapse(Flow *f, MediaBufferVector &input_vector)
{
    MuxerFlow *flow = static_cast<MuxerFlow *>(f);
    auto &&recorder = flow->video_recorder;
    auto &vid_buffer = input_vector[0];
    auto &aud_buffer = input_vector[1];

    if (flow->last_ts != 0 && flow->video_in && vid_buffer) {
        int lapse_frame_interval = 1000000 / flow->vid_enc_config.vid_cfg.frame_rate;
        vid_buffer->SetUSTimeStamp(flow->lapse_time_stamp + lapse_frame_interval);
        flow->lapse_time_stamp += lapse_frame_interval;
    }

    if (flow->request_stop_stream) {
        flow->StopStream();
        flow->enable_streaming = false;
        flow->request_stop_stream = false;
        return true;
    }

    if (!flow->enable_streaming) {
        return true;
    }

    flow->CheckRecordEnd(vid_buffer);

    if (recorder == nullptr) {
        std::string path = flow->GenFilePath();
        if (path.empty()) {
            DRM_MEDIA_LOGW("lapse: empty file path, drop");
            return true;
        }

        recorder = flow->NewRecorder(path.c_str());
        flow->last_ts = 0;
        flow->real_file_duration = 0;
    }

    if (flow->io_error) {
        return true;
    }

    do {
        if (!flow->audio_in || aud_buffer == nullptr) {
            break;
        }

        if (!recorder->Write(flow, aud_buffer)) {
            DRM_MEDIA_LOGE("FATAL: Recorder write audio failed");
            flow->io_error = true;
            return true;
        }
    } while (0);

    do {
        if (!flow->video_in || vid_buffer == nullptr) {
            break;
        }

        if (!flow->video_extra) {
            if ((vid_buffer->GetUserFlag() & MediaBuffer::kIntra)) {
                if (flow->GetVideoExtradata(vid_buffer)) {
                    break;
                }
            } else {
                break;
            }
        }

        if (!recorder->Write(flow, vid_buffer)) {
            flow->io_error = true;
            DRM_MEDIA_LOGE("FATAL: Recorder write audio failed");
            return true;
        }

        if (flow->last_ts == 0 || vid_buffer->GetUSTimeStamp() < flow->last_ts) {
            flow->last_ts = vid_buffer->GetUSTimeStamp();
            flow->lapse_time_stamp = flow->last_ts;
        }

        flow->real_file_duration = (vid_buffer->GetUSTimeStamp() - flow->last_ts) / 1000;
    } while (0);

    return true;
}

bool save_buffer(Flow *f, MediaBufferVector &input_vector)
{
    MuxerFlow *flow = static_cast<MuxerFlow *>(f);
    bool overflow = false;
    auto &vid_buffer = input_vector[0];
    auto &aud_buffer = input_vector[1];

    flow->cached_cond_mtx.lock();
    if ((flow->pre_record_mode != MUXER_PRE_RECORD_NONE) && (flow->pre_record_time > 0)) {
        if (flow->audio_in && aud_buffer != nullptr) {
            flow->DequePushBack(&(flow->aud_prerecord_buffers), aud_buffer);
        }

        if (flow->video_in && vid_buffer != nullptr) {
            flow->DequePushBack(&(flow->vid_prerecord_buffers), vid_buffer);
        }
    }

    if (flow->request_stop_stream || !flow->enable_streaming) {
        flow->cached_cond_mtx.unlock();
        return true;
    }

    if (flow->audio_in && aud_buffer) {
        flow->aud_cached_buffers.push_back(aud_buffer);
        flow->cached_cond_mtx.notify();
    }

    if (flow->video_in && vid_buffer) {
        flow->vid_cached_buffers.push_back(vid_buffer);
        flow->cached_cond_mtx.notify();
    }

    while (!flow->vid_cached_buffers.empty()) {
        int64_t t1, t2;
        int cache_seconds = 10;
        int size = flow->vid_cached_buffers.size();

        t1 = flow->vid_cached_buffers.back()->GetUSTimeStamp();
        t2 = flow->vid_cached_buffers.front()->GetUSTimeStamp();
        if (t2 - t1 >= cache_seconds * 1000 * 1000 || size >= cache_seconds * flow->vid_enc_config.vid_cfg.frame_rate) {
            DRM_MEDIA_LOGE("%d: cached size more than %d seconds. Data writing too slow", flow->muxer_id, cache_seconds);

            overflow = true;
            flow->vid_cached_buffers.pop_front();
        } else {
            break;
        }
    }

    if (flow->video_recorder != nullptr && overflow) {
        flow->video_recorder->ProcessEvent(MUX_EVENT_ERR_WRITE_FILE_FAIL, 0);
    }
    flow->cached_cond_mtx.unlock();

    return true;
}

void MuxerFlow::FlushThread()
{
    int cache_level = 40;
    int cache_warning = 0;
    auto &&recorder = video_recorder;
    std::deque<std::shared_ptr<MediaBuffer>> *vid_deque, *aud_deque;

    aud_deque = &aud_cached_buffers;
    vid_deque = &vid_cached_buffers;
    DRM_MEDIA_LOGW("Flush Thread Running");

    pthread_yield();

    while (1) {
        int size;
        std::shared_ptr<MediaBuffer> aud_buffer = nullptr;
        std::shared_ptr<MediaBuffer> vid_buffer = nullptr;

        size = vid_deque->size();
        if (size >= cache_level && size % cache_level == 0) {
            DRM_MEDIA_LOGE("%u: cached size(aud:%lu, vid:%d)", muxer_id, (unsigned long)aud_deque->size(), size);

            if (size >= 2 * cache_level) {
                cache_warning++;
                recorder->ProcessEvent(MUX_EVENT_WARN_FILE_WRITING_SLOW, size);
            }
        } else if (size < cache_level / 2 && cache_warning > 0) {
            cache_warning = 0;
            recorder->ProcessEvent(MUX_EVENT_WARN_FILE_WRITING_NORMAL, size);
        }

        cached_cond_mtx.lock();

        if (flush_thread_quit) {
            DRM_MEDIA_LOGW("%d: Flush Thread quit now", muxer_id);
            cached_cond_mtx.unlock();
            break;
        }

        if (aud_deque->empty() && vid_deque->empty()) {
            if (request_stop_stream) {
                StopStream();
                enable_streaming = false;
                request_stop_stream = false;

                DRM_MEDIA_LOGW("%d: Flush Thread Stopped", muxer_id);
            } else {
                cached_cond_mtx.wait();
            }

            cached_cond_mtx.unlock();
            continue;
        }

        if (!aud_deque->empty()) {
            aud_buffer = aud_deque->front();
            aud_deque->pop_front();
        }

        if (!vid_deque->empty()) {
            vid_buffer = vid_deque->front();
            if (aud_buffer != nullptr && vid_buffer != nullptr && aud_buffer->GetUSTimeStamp() < vid_buffer->GetUSTimeStamp()) {
                vid_buffer = nullptr;
            } else {
                vid_deque->pop_front();
            }
        }

        CheckRecordEnd(vid_buffer);

        if (recorder == nullptr) {
            std::string path = GenFilePath();

            if (path.empty()) {
                cached_cond_mtx.unlock();
                continue;
            }

            recorder = NewRecorder(path.c_str());
            last_ts = 0;
            real_file_duration = 0;

            if (PreRecordWrite() == 1) {
                if (!aud_deque->empty()) {
                    aud_buffer = aud_deque->front();
                    aud_deque->pop_front();
                }

                if (!vid_deque->empty()) {
                    vid_buffer = vid_deque->front();
                    vid_deque->pop_front();
                }
            }
        }
        cached_cond_mtx.unlock();

        if (io_error) {
            continue;
        }

        if (audio_in && aud_buffer) {
            if (!recorder->Write(this, aud_buffer)) {
                DRM_MEDIA_LOGE("%d: FATAL: Recorder write audio failed", muxer_id);
                io_error = true;
                continue;
            }
        }

        if (video_in && vid_buffer) {
            if (!video_extra) {
                if ((vid_buffer->GetUserFlag() & MediaBuffer::kIntra)) {
                    if (GetVideoExtradata(vid_buffer)) {
                        continue;
                    }
                } else {
                    continue;
                }
            }

            if (!recorder->Write(this, vid_buffer)) {
                DRM_MEDIA_LOGE("%d: FATAL: Recorder write video failed\n", muxer_id);
                io_error = true;
                continue;
            }

            if (last_ts == 0 || vid_buffer->GetUSTimeStamp() < last_ts) {
                last_ts = vid_buffer->GetUSTimeStamp();
            }

            real_file_duration = (vid_buffer->GetUSTimeStamp() - last_ts) / 1000;
        }
    }
}

DEFINE_FLOW_FACTORY(MuxerFlow, Flow)

const char *FACTORY(MuxerFlow)::ExpectedInputDataType()
{
    return DRM_TYPE_NOTHING;
}

const char *FACTORY(MuxerFlow)::OutPutDataType()
{
    return DRM_TYPE_ANYTHING;
}

VideoRecorder::VideoRecorder(const char *param, Flow *f, const char *rpath, int customio)
    : vid_stream_id(-1), aud_stream_id(-1), muxer_flow(f), record_path(rpath)
{
    muxer = libdrm::REFLECTOR(Muxer)::Create<libdrm::Muxer>("rkaudio", param);
    if (!muxer) {
        DRM_MEDIA_LOGI("Create muxer rkaudio failed\n");
        exit(EXIT_FAILURE);
    }

    if (muxer_flow && customio) {
        muxer->SetWriteCallback(muxer_flow, &muxer_buffer_callback);
    }

    MuxerFlow *muxer_flow_ptr = static_cast<MuxerFlow *>(muxer_flow);
    if (muxer_flow_ptr->manual_split_record) {
        ProcessEvent(MUX_EVENT_FILE_BEGIN, (int)muxer_flow_ptr->manual_split_file_duration);
    } else {
        ProcessEvent(MUX_EVENT_FILE_BEGIN, (int)muxer_flow_ptr->file_duration);
    }
}

VideoRecorder::~VideoRecorder()
{
    if (vid_stream_id != -1) {
        auto buffer = libdrm::MediaBuffer::Alloc(1);
        buffer->SetEOF(true);
        buffer->SetValidSize(0);
        muxer->Write(buffer, vid_stream_id);
    }

    MuxerFlow *muxer_flow_ptr = static_cast<MuxerFlow *>(muxer_flow);
    if (muxer_flow_ptr->manual_split_record) {
        ProcessEvent(MUX_EVENT_MANUAL_SPLIT_END, (int)muxer_flow_ptr->real_file_duration);
    } else {
        ProcessEvent(MUX_EVENT_FILE_END, (int)muxer_flow_ptr->real_file_duration);
    }

    if (muxer) {
        muxer.reset();
    }
}

void VideoRecorder::ProcessEvent(DrmMuxerEventType event_type, int value)
{
    if (muxer_flow) {
        DrmMuxerEvent muxer_event;
        memset(&muxer_event, 0, sizeof(DrmMuxerEvent));

        muxer_event.type = event_type;
        if (strlen(record_path.c_str())) {
            memcpy(muxer_event.file_name, record_path.c_str(), strlen(record_path.c_str()));
            muxer_event.value = value;
        }

        if (muxer_flow->event_callback_) {
            muxer_flow->event_callback_(muxer_flow->event_handler2_, (void *)&muxer_event);
        }
    }
}

void VideoRecorder::ClearStream()
{
    vid_stream_id = -1;
    aud_stream_id = -1;
}

bool VideoRecorder::Write(MuxerFlow *f, std::shared_ptr<MediaBuffer> buffer)
{
    MuxerFlow *flow = static_cast<MuxerFlow *>(f);

    if (flow->video_in && flow->video_extra && vid_stream_id == -1) {
        if (!muxer->NewMuxerStream(flow->vid_enc_config, flow->video_extra, vid_stream_id)) {
            DRM_MEDIA_LOGE("NewMuxerStream failed for video");
            ProcessEvent(MUX_EVENT_ERR_CREATE_FILE_FAIL, -1);
        } else {
            DRM_MEDIA_LOGI("Video: create video stream finished");
        }

        if (flow->audio_in) {
            if (!muxer->NewMuxerStream(flow->aud_enc_config, nullptr, aud_stream_id)) {
                DRM_MEDIA_LOGE("NewMuxerStream failed for audio");
                ProcessEvent(MUX_EVENT_ERR_CREATE_FILE_FAIL, -2);
            } else {
                DRM_MEDIA_LOGI("Audio: create audio stream finished");
            }
        }

        auto header = muxer->WriteHeader(vid_stream_id);
        if (!header) {
            DRM_MEDIA_LOGI("WriteHeader on video stream return nullptr");
            ClearStream();
            ProcessEvent(MUX_EVENT_ERR_WRITE_FILE_FAIL, 0);
            return false;
        }
    }

    if (buffer->GetType() == DrmType::Video && vid_stream_id != -1) {
        if (nullptr == muxer->Write(buffer, vid_stream_id)) {
            DRM_MEDIA_LOGE("Write on video stream return nullptr");

            ClearStream();
            ProcessEvent(MUX_EVENT_ERR_WRITE_FILE_FAIL, -1);
            return false;
        }
    } else if (buffer->GetType() == DrmType::Audio && aud_stream_id != -1) {
        if (nullptr == muxer->Write(buffer, aud_stream_id)) {
            DRM_MEDIA_LOGE("Write on audio stream return nullptr");

            ClearStream();
            ProcessEvent(MUX_EVENT_ERR_WRITE_FILE_FAIL, -2);
            return false;
        }
    }

    return true;
}
}
