#include <sys/prctl.h>
#include <libdrm/display/utils.hpp>
#include <libdrm/display/codec.hpp>
#include <libdrm/display/buffer.hpp>

namespace libdrm {
std::shared_ptr<MediaBuffer> Codec::GetExtraData(void **data, size_t *size)
{
    if (data && size && extra_data) {
        *data = extra_data->GetPtr();
        *size = extra_data->GetValidSize();
    }

    return extra_data;
}

bool Codec::SetExtraData(void *data, size_t size, bool realloc)
{
    if (!realloc) {
        if (!extra_data) {
            extra_data = std::make_shared<MediaBuffer>();
            if (!extra_data) {
                return false;
            }
        }

        extra_data->SetPtr(data);
        extra_data->SetValidSize(size);
        extra_data->SetSize(size);
        extra_data->SetUserData(nullptr);
        return true;
    }

    if (!data || size == 0) {
        return false;
    }

    auto extradata = MediaBuffer::Alloc(size);
    if (!extradata || extradata->GetSize() < size) {
        DRM_LOG_MEMORY();
        return false;
    }

    memcpy(extradata->GetPtr(), data, size);
    extradata->SetValidSize(size);
    extra_data = extradata;

    return true;
}

bool Codec::Init()
{
    return false;
}

static const uint8_t *find_startcode_internal(const uint8_t *p, const uint8_t *end)
{
    const uint8_t *a = p + 4 - ((intptr_t)p & 3);

    for (end -= 3; p < a && p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
            return p;
        }
    }

    for (end -= 3; p < end; p += 4) {
        uint32_t x = *(const uint32_t *)p;
        if ((x - 0x01010101) & (~x) & 0x80808080) {
            if (p[1] == 0) {
                if (p[0] == 0 && p[2] == 1)
                return p;
                if (p[2] == 0 && p[3] == 1)
                return p + 1;
            }

            if (p[3] == 0) {
                if (p[2] == 0 && p[4] == 1)
                return p + 2;
                if (p[4] == 0 && p[5] == 1)
                return p + 3;
            }
        }
    }

    for (end += 3; p < end; p++) {
        if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
            return p;
        }
    }

    return end + 3;
}

const uint8_t *find_nalu_startcode(const uint8_t *p, const uint8_t *end)
{
    const uint8_t *out = find_startcode_internal(p, end);
    if (p < out && out < end && !out[-1]) {
        out--;
    }

    return out;
}

std::list<std::shared_ptr<MediaBuffer>> split_h264_separate(const uint8_t *buffer, size_t length, int64_t timestamp)
{
    const uint8_t *p = buffer;
    const uint8_t *end = p + length;
    std::list<std::shared_ptr<MediaBuffer>> l;
    const uint8_t *nal_start = nullptr, *nal_end = nullptr;

    nal_start = find_nalu_startcode(p, end);

    // 00 00 01 or 00 00 00 01
    size_t start_len = (nal_start[2] == 1 ? 3 : 4);
    for (; ;) {
        if (nal_start == end) {
            break;
        }

        nal_start += start_len;
        nal_end = find_nalu_startcode(nal_start, end);

        uint32_t flag;
        size_t size = nal_end - nal_start + start_len;
        uint8_t nal_type = (*nal_start) & 0x1F;

        switch (nal_type) {
            case 7:
            case 8:
                flag = MediaBuffer::kExtraIntra;
                break;

            default:
                flag = 0;
        }

        if (!flag) {
            break;
        }

        auto sub_buffer = MediaBuffer::Alloc(size);
        if (!sub_buffer) {
            DRM_LOG_MEMORY();
            l.clear();
            return l;
        }

        memcpy(sub_buffer->GetPtr(), nal_start - start_len, size);
        sub_buffer->SetValidSize(size);
        sub_buffer->SetUserFlag(flag);
        sub_buffer->SetUSTimeStamp(timestamp);
        sub_buffer->SetType(DrmType::Video);
        l.push_back(sub_buffer);

        nal_start = nal_end;
    }

    return l;
}

std::list<std::shared_ptr<MediaBuffer>> split_h265_separate(const uint8_t *buffer, size_t length, int64_t timestamp)
{
    const uint8_t *p = buffer;
    const uint8_t *end = p + length;
    std::list<std::shared_ptr<MediaBuffer>> l;
    const uint8_t *nal_start = nullptr, *nal_end = nullptr;

    nal_start = find_nalu_startcode(p, end);

    // 00 00 01 or 00 00 00 01
    size_t start_len = (nal_start[2] == 1 ? 3 : 4);
    for (; ;) {
        if (nal_start == end) {
            break;
        }

        nal_start += start_len;
        nal_end = find_nalu_startcode(nal_start, end);

        uint32_t flag;
        size_t size = nal_end - nal_start + start_len;
        uint8_t nal_type = ((*nal_start) & 0x7E) >> 1;

        switch (nal_type) {
            case 32:
            case 33:
            case 34:
                flag = MediaBuffer::kExtraIntra;
                break;

            default:
                flag = 0;
        }

        if (!flag) {
            break;
        }

        auto sub_buffer = MediaBuffer::Alloc(size);
        if (!sub_buffer) {
            DRM_LOG_MEMORY();
            l.clear();
            return l;
        }

        memcpy(sub_buffer->GetPtr(), nal_start - start_len, size);
        sub_buffer->SetValidSize(size);
        sub_buffer->SetUserFlag(flag);
        sub_buffer->SetUSTimeStamp(timestamp);
        l.push_back(sub_buffer);

        nal_start = nal_end;
    }

    return l;
}

static void *FindNaluByType(std::shared_ptr<MediaBuffer> &mb, int nal_type, int &size, DrmCodecType c_type)
{
    if ((c_type != DRM_CODEC_TYPE_H264) && (c_type != DRM_CODEC_TYPE_H265)) {
        DRM_MEDIA_LOGE("%s failed! Invalid codec type", __func__);
        return NULL;
    }

    void *target_nalu = NULL;
    const uint8_t *start = (uint8_t *)mb->GetPtr();
    const uint8_t *end = start + mb->GetValidSize();
    const uint8_t *nal_start = nullptr, *nal_end = nullptr;
    nal_start = nal_end = find_nalu_startcode(start, end);

    // 00 00 01 or 00 00 00 01
    int start_len = (nal_start[2] == 1 ? 3 : 4);
    int nal_size = 0;
    uint8_t type = 0;

    for (; ;) {
        if (nal_start == end) {
            break;
        }

        nal_start = nal_end;
        nal_end = find_nalu_startcode(nal_start + start_len, end);
        nal_size = nal_end - nal_start;

        if (c_type == DRM_CODEC_TYPE_H264) {
            type = *(nal_start + start_len) & 0x1F;
        } else {
            type = (*(nal_start + start_len) & 0x7E) >> 1;
        }

        if (type == nal_type) {
            size = nal_size;
            target_nalu = (void *)nal_start;
            break;
        }
    }

    return target_nalu;
}

void *GetVpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    if (c_type != DRM_CODEC_TYPE_H265) {
        return NULL;
    }

    return FindNaluByType(mb, 32, size, c_type);
}

void *GetSpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int nalu_type = 0;

    if (c_type == DRM_CODEC_TYPE_H265) {
        nalu_type = 33;
    } else if (c_type == DRM_CODEC_TYPE_H264) {
        nalu_type = 7;
    } else {
        return NULL;
    }

    return FindNaluByType(mb, nalu_type, size, c_type);
}

void *GetPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int nalu_type = 0;

    if (c_type == DRM_CODEC_TYPE_H265) {
        nalu_type = 34;
    } else if (c_type == DRM_CODEC_TYPE_H264) {
        nalu_type = 8;
    } else {
        return NULL;
    }

    return FindNaluByType(mb, nalu_type, size, c_type);
}

void *GetSpsPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int sps_size = 0;
    int pps_size = 0;
    void *sps_ptr = NULL;
    void *pps_ptrt = NULL;

    sps_ptr = GetSpsFromBuffer(mb, sps_size, c_type);
    if (!sps_ptr) {
        return NULL;
    }

    pps_ptrt = GetPpsFromBuffer(mb, pps_size, c_type);
    if (!pps_ptrt) {
        return NULL;
    }

    size = sps_size + pps_size;
    return sps_ptr;
}

void *GetVpsSpsPpsFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int vps_size = 0;
    int sps_size = 0;
    int pps_size = 0;
    void *vps_ptr = NULL;
    void *sps_ptr = NULL;
    void *pps_ptrt = NULL;

    vps_ptr = GetVpsFromBuffer(mb, vps_size, c_type);
    if (!vps_ptr) {
        return NULL;
    }

    sps_ptr = GetSpsFromBuffer(mb, sps_size, c_type);
    if (!sps_ptr) {
        return NULL;
    }

    pps_ptrt = GetPpsFromBuffer(mb, pps_size, c_type);
    if (!pps_ptrt) {
        return NULL;
    }

    size = vps_size + sps_size + pps_size;
    return vps_ptr;
}

void *GetSeiFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int nalu_type = 0;

    if (c_type == DRM_CODEC_TYPE_H265) {
        nalu_type = 39;
    } else if (c_type == DRM_CODEC_TYPE_H264) {
        nalu_type = 6;
    } else {
        return NULL;
    }

    return FindNaluByType(mb, nalu_type, size, c_type);
}

void *GetIntraFromBuffer(std::shared_ptr<MediaBuffer> &mb, int &size, DrmCodecType c_type)
{
    int nalu_type = 0;
    void *idr_ptr = NULL;

    if (c_type == DRM_CODEC_TYPE_H265) {
        nalu_type = 19;
    } else if (c_type == DRM_CODEC_TYPE_H264) {
        nalu_type = 5;
    } else {
        return NULL;
    }

    idr_ptr = FindNaluByType(mb, nalu_type, size, c_type);
    size = mb->GetValidSize() - (int)((uint8_t *)idr_ptr - (uint8_t *)mb->GetPtr());

    return idr_ptr;
}
}
