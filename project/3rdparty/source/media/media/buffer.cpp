#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <media/utils/utils.h>
#include <media/utils/buffer.h>
#include <media/utils/key_string.h>

namespace libdrm {
MediaBuffer::MemType StringToMemType(const char *s)
{
    if (s) {
        if (!strcmp(s, DRM_KEY_MEM_DRM) || !strcmp(s, DRM_KEY_MEM_HARDWARE)) {
            return MediaBuffer::MemType::MEM_HARD_WARE;
        }

        DRM_MEDIA_LOGW("%s is not supported or not integrated, fallback to common", s);
    }

    return MediaBuffer::MemType::MEM_COMMON;
}

static int free_common_memory(void *buffer)
{
    if (buffer) {
        free(buffer);
    }

    return 0;
}

static MediaBuffer alloc_common_memory(size_t size)
{
    void *buffer = malloc(size);
    if (!buffer) {
        return MediaBuffer();
    }

    DRM_MEDIA_LOGD("func:[%s], ptr:[%p], fd:[%d], handle:[%u], dev_fd:[%d], size:[%u]", __func__, buffer, -1, 0, -1, (unsigned)size);
    return MediaBuffer(buffer, size, -1, 0, -1, buffer, free_common_memory);
}

static MediaGroupBuffer *alloc_common_memory_group(size_t size)
{
    void *buffer = malloc(size);
    if (!buffer) {
        return nullptr;
    }

    MediaGroupBuffer *mgb = new MediaGroupBuffer(buffer, size, -1, 0, -1, buffer, free_common_memory);
    return mgb;
}

#include <libdrm/xf86drm.h>
#include <libdrm/drm/drm_fourcc.h>

#define STATIC_ASSERT(COND)                                                         \
    do {                                                                            \
        /*(void)sizeof(char[1 - 2 * !(COND)]);*/                                        \
    } while (0)

#define drm_mmap(addr, length, prot, flags, fd, offset)                             \
    mmap(addr, length, prot, flags, fd, offset)

static inline int drm_munmap(void *addr, size_t length)
{
#define LARGE_OFF_T ((((off_t)1 << 31) << 31) - 1 + (((off_t)1 << 31) << 31))
    STATIC_ASSERT(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1);
#undef LARGE_OFF_T

    return munmap(addr, length);
}

static int card_index = 0;
static int drm_device_open(const char *device = nullptr)
{
    uint64_t has_dumb;
    drmVersionPtr version;
    char drm_dev[] = "/dev/dri/card0000";

    if (!device) {
        snprintf(drm_dev, sizeof(drm_dev), DRM_DEV_NAME, DRM_DIR_NAME, card_index);
        device = drm_dev;
    }

    int fd = open(device, O_RDWR);
    if (fd < 0) {
        DRM_MEDIA_LOGE("Failed to open %s,please enable display_subsystem in the dts", device);
        return fd;
    }

    version = drmGetVersion(fd);
    if (!version) {
        DRM_MEDIA_LOGI("Failed to get version information from %s: probably not a DRM device?", device);
        close(fd);
        return -1;
    }

    DRM_MEDIA_LOGI("Opened DRM device:[%s], driver:[%s], version:[%d.%d.%d]", device, version->name, version->version_major, version->version_minor, version->version_patchlevel);
    drmFreeVersion(version);

    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        DRM_MEDIA_LOGE("drm device '%s' does not support dumb buffers", device);
        close(fd);
        return -1;
    }

    return fd;
}

class DrmDevice {
public:
    DrmDevice() {
        fd = drm_device_open();
    }

    ~DrmDevice() {
        if (fd >= 0) {
            close(fd);
        }
    }

    bool Valid() {
        return fd >= 0;
    }

    const static std::shared_ptr<DrmDevice> &GetInstance() {
        const static std::shared_ptr<DrmDevice> mDrmDevice = std::make_shared<DrmDevice>();
        return mDrmDevice;
    }

    int fd;
};

class DrmBuffer {
public:
    DrmBuffer(std::shared_ptr<DrmDevice> dev, size_t s, __u32 flags = 0)
        : device(dev), handle(0), len(DRM_UPALIGNTO(s, PAGE_SIZE)), fd(-1), map_ptr(nullptr)
    {
        struct drm_mode_create_dumb dmcb;

        memset(&dmcb, 0x00, sizeof(struct drm_mode_create_dumb));
        dmcb.bpp = 8;
        dmcb.width = len;
        dmcb.height = 1;
        dmcb.flags = flags;

        int ret = drmIoctl(dev->fd, DRM_IOCTL_MODE_CREATE_DUMB, &dmcb);
        if (ret < 0) {
            DRM_MEDIA_LOGE("Failed to create dumb<w,h,bpp: %d,%d,%d>: %m", dmcb.width, dmcb.height, dmcb.bpp);
            return;
        }

        assert(dmcb.handle > 0);
        assert(dmcb.size >= dmcb.width * dmcb.height * dmcb.bpp / 8);

        handle = dmcb.handle;
        len = dmcb.size;
        ret = drmPrimeHandleToFD(dev->fd, dmcb.handle, DRM_CLOEXEC, &fd);
        if (ret) {
            DRM_MEDIA_LOGE("Failed to convert drm handle to fd: %m");
            return;
        }

        assert(fd >= 0);
        dev_fd = dev->fd;
    }

    ~DrmBuffer() {
        if (map_ptr) {
            drm_munmap(map_ptr, len);
        }

        int ret;
        if (handle > 0) {
            struct drm_mode_destroy_dumb data = {
                .handle = handle,
            };
    
            ret = drmIoctl(device->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &data);
            if (ret) {
                DRM_MEDIA_LOGE("Failed to free drm handle <%d>: %m", handle);
            }
        }

        if (fd >= 0) {
            ret = close(fd);
            if (ret) {
                DRM_MEDIA_LOGE("Failed to close drm buffer fd <%d>: %m", fd);
            }
        }
    }

    bool MapToVirtual() {
        struct drm_mode_map_dumb dmmd;

        memset(&dmmd, 0x00, sizeof(dmmd));
        dmmd.handle = handle;

        int ret = drmIoctl(device->fd, DRM_IOCTL_MODE_MAP_DUMB, &dmmd);
        if (ret) {
            DRM_MEDIA_LOGE("Failed to map dumb: %m");
            return false;
        }

        void *ptr = drm_mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, device->fd, dmmd.offset);
        if (ptr == MAP_FAILED) {
            DRM_MEDIA_LOGE("Failed to drm_mmap: %m");
            return false;
        }

        assert(ptr);
        map_ptr = ptr;

        return true;
    }

    bool Valid() {
        return fd >= 0;
    }

    std::shared_ptr<DrmDevice> device;
    __u32                      handle;
    size_t                     len;
    int                        fd;
    void                       *map_ptr;
    int                        dev_fd;
};

static int free_drm_memory(void *buffer)
{
    assert(buffer);
    delete static_cast<DrmBuffer *>(buffer);
    return 0;
}

static MediaBuffer alloc_drm_memory(size_t size, unsigned int flag, bool map = true)
{
    DrmBuffer *db = nullptr;
    const static std::shared_ptr<DrmDevice> &drm_dev = DrmDevice::GetInstance();

    do {
        if (!drm_dev || !drm_dev->Valid()) {
            break;
        }

        db = new DrmBuffer(drm_dev, size, flag);
        if (!db || !db->Valid()) {
            break;
        }

        if (map && !db->MapToVirtual()) {
            break;
        }

        DRM_MEDIA_LOGD("func:[%s], ptr:[%p], fd:[%d], handle:[%u], dev_fd:[%d], size:[%u]", __func__, db->map_ptr, db->fd, db->handle, db->dev_fd, (unsigned)size);
        return MediaBuffer(db->map_ptr, db->len, db->fd, db->handle, db->dev_fd, db, free_drm_memory);
    } while (false);

    if (db) {
        delete db;
    }

    return MediaBuffer();
}

static MediaGroupBuffer *alloc_drm_memory_group(size_t size, unsigned int flag, bool map = true)
{
    DrmBuffer *db = nullptr;
    const static std::shared_ptr<DrmDevice> &drm_dev = DrmDevice::GetInstance();

    do {
        if (!drm_dev || !drm_dev->Valid()) {
            break;
        }

        db = new DrmBuffer(drm_dev, size, flag);
        if (!db || !db->Valid()) {
            break;
        }

        if (map && !db->MapToVirtual()) {
            break;
        }

        MediaGroupBuffer *mgb = nullptr;
        mgb = new MediaGroupBuffer(db->map_ptr, db->len, db->fd, db->handle, db->dev_fd, db, free_drm_memory);
        return mgb;
    } while (false);

    if (db) {
        delete db;
    }

    return nullptr;
}

std::shared_ptr<MediaBuffer> MediaBuffer::Alloc(size_t size, MemType type, unsigned int flag)
{
    MediaBuffer &&mb = Alloc2(size, type, flag);
    if (mb.GetSize() == 0) {
        return nullptr;
    }

    return std::make_shared<MediaBuffer>(mb);
}

MediaBuffer MediaBuffer::Alloc2(size_t size, MemType type, unsigned int flag)
{
    switch (type) {
        case MemType::MEM_COMMON:
            return alloc_common_memory(size);

        case MemType::MEM_HARD_WARE:
            return alloc_drm_memory(size, flag);

        default:
            DRM_MEDIA_LOGE("unknown memtype");
            return MediaBuffer();
    }
}

std::shared_ptr<MediaBuffer> MediaBuffer::Clone(MediaBuffer &src, MemType dst_type)
{
    size_t size = src.GetValidSize();
    if (!size) {
        return nullptr;
    }

    auto new_buffer = Alloc(size, dst_type);
    if (!new_buffer) {
        DRM_LOG_MEMORY();
        return nullptr;
    }

    if (src.IsHwBuffer() && new_buffer->IsHwBuffer()) {
        DRM_TODO();
    }

    memcpy(new_buffer->GetPtr(), src.GetPtr(), size);
    new_buffer->SetValidSize(size);
    new_buffer->CopyAttribute(src);

    return new_buffer;
}

std::shared_ptr<MediaBuffer> MediaBuffer::Clone2(MediaBuffer &src, MemType dst_type)
{
    size_t size = src.GetValidSize();
    if (!size) {
        return nullptr;
    }

    auto new_buffer = Alloc(size, dst_type);
    if (!new_buffer) {
        DRM_LOG_MEMORY();
        return nullptr;
    }

    new_buffer->SetValidSize(size);
    new_buffer->CopyAttribute(src);

    return new_buffer;
}

void MediaBuffer::CopyAttribute(MediaBuffer &src_attr)
{
    type = src_attr.GetType();
    user_flag = src_attr.GetUserFlag();
    ustimestamp = src_attr.GetUSTimeStamp();
    eof = src_attr.IsEOF();
    tsvc_level = src_attr.GetTsvcLevel();
}

struct dma_buf_sync {
    __u64 flags;
};

#define DMA_BUF_SYNC_READ               (1 << 0)
#define DMA_BUF_SYNC_WRITE              (2 << 0)
#define DMA_BUF_SYNC_RW                 (DMA_BUF_SYNC_READ | DMA_BUF_SYNC_WRITE)
#define DMA_BUF_SYNC_START              (0 << 2)
#define DMA_BUF_SYNC_END                (1 << 2)
#define DMA_BUF_SYNC_VALID_FLAGS_MASK   (DMA_BUF_SYNC_RW | DMA_BUF_SYNC_END)
#define DMA_BUF_BASE                    'b'
#define DMA_BUF_IOCTL_SYNC              _IOW(DMA_BUF_BASE, 0, struct dma_buf_sync)

void MediaBuffer::BeginCPUAccess(bool readonly)
{
    struct dma_buf_sync sync = {0};

    if (fd < 0) {
        return;
    }

    if (readonly) {
        sync.flags = DMA_BUF_SYNC_READ | DMA_BUF_SYNC_START;
    } else {
        sync.flags = DMA_BUF_SYNC_RW | DMA_BUF_SYNC_START;
    }

    int ret = ioctl(fd, DMA_BUF_IOCTL_SYNC, &sync);
    if (ret < 0) {
        DRM_MEDIA_LOGE("%s: %s", __func__, strerror(errno));
    }
}

void MediaBuffer::EndCPUAccess(bool readonly)
{
    struct dma_buf_sync sync = {0};

    if (fd < 0) {
        return;
    }

    if (readonly) {
        sync.flags = DMA_BUF_SYNC_READ | DMA_BUF_SYNC_END;
    } else {
        sync.flags = DMA_BUF_SYNC_RW | DMA_BUF_SYNC_END;
    }

    int ret = ioctl(fd, DMA_BUF_IOCTL_SYNC, &sync);
    if (ret < 0) {
        DRM_MEDIA_LOGE("%s: %s", __func__, strerror(errno));
    }
}

MediaGroupBuffer *MediaGroupBuffer::Alloc(size_t size, MediaBuffer::MemType type, unsigned int flag)
{
    switch (type) {
        case MediaBuffer::MemType::MEM_COMMON:
            return alloc_common_memory_group(size);

        case MediaBuffer::MemType::MEM_HARD_WARE:
            return alloc_drm_memory_group(size, flag);

        default:
            DRM_MEDIA_LOGE("unknown memtype");
            return nullptr;
    }
}

BufferPool::BufferPool(int cnt, int size, MediaBuffer::MemType type)
{
    bool sucess = true;

    if (cnt <= 0) {
        DRM_MEDIA_LOGE("BufferPool: cnt:%d is invalid", cnt);
        return;
    }

    for (int i = 0; i < cnt; i++) {
        auto mgb = MediaGroupBuffer::Alloc(size, type);
        if (!mgb) {
            sucess = false;
            break;
        }

        mgb->SetBufferPool(this);
        DRM_MEDIA_LOGD("Create pool:[%p], mgb:[%p], ptr:[%p], fd:[%d], handle:[%u], dev_fd:[%d], size:[%zu]", this, mgb, mgb->GetPtr(), mgb->GetFD(), mgb->GetHandle(), mgb->GetDevFD(), mgb->GetSize());
        ready_buffers.push_back(mgb);
    }

    if (!sucess) {
        while (ready_buffers.size() > 0) {
            ready_buffers.pop_front();
        }

        DRM_MEDIA_LOGE("Create buffer pool failed! Please check space is enough");
        return;
    }

    buf_cnt = cnt;
    buf_size = size;
    DRM_MEDIA_LOGD("Create buffer pool:[%p], size:[%d], cnt:[%d]", this, size, cnt);
}

BufferPool::BufferPool(int cnt, int size, MediaBuffer::MemType type, unsigned int flag)
{
    bool sucess = true;

    if (cnt <= 0) {
        DRM_MEDIA_LOGE("BufferPool: cnt:%d is invalid", cnt);
        return;
    }

    for (int i = 0; i < cnt; i++) {
        auto mgb = MediaGroupBuffer::Alloc(size, type, flag);
        if (!mgb) {
            sucess = false;
            break;
        }

        mgb->SetBufferPool(this);
        DRM_MEDIA_LOGD("Create pool:[%p], mgb:[%p], ptr:[%p], fd:[%d], handle:[%u], dev_fd:[%d], size:[%zu]", this, mgb, mgb->GetPtr(), mgb->GetFD(), mgb->GetHandle(), mgb->GetDevFD(), mgb->GetSize());
        ready_buffers.push_back(mgb);
    }

    if (!sucess) {
        while (ready_buffers.size() > 0) {
            ready_buffers.pop_front();
        }

        DRM_MEDIA_LOGE("BufferPool: Create buffer pool failed! Please check space is enough");
        return;
    }

    buf_cnt = cnt;
    buf_size = size;
    DRM_MEDIA_LOGD("Create buffer pool:[%p], size:[%d], cnt:[%d]", this, size, cnt);
}

BufferPool::~BufferPool()
{
    int cnt = 0;
    int wait_times = 30;

    while (busy_buffers.size() > 0) {
        if (wait_times-- <= 0) {
            DRM_MEDIA_LOGE("BufferPool: waiting bufferpool free for 900ms, TimeOut");
            break;
        }

        libdrm::usleep(30000);
    }

    MediaGroupBuffer *mgb = NULL;
    while (ready_buffers.size() > 0) {
        mgb = ready_buffers.front();
        ready_buffers.pop_front();
        DRM_MEDIA_LOGD("BufferPool: #%02d Destroy buffer pool(ready):[%p,%p]", cnt, this, mgb);
        delete mgb;
        cnt++;
    }

    while (busy_buffers.size() > 0) {
        mgb = busy_buffers.front();
        busy_buffers.pop_front();
        DRM_MEDIA_LOGW("BufferPool: #%02d Destroy buffer pool(busy):[%p,%p]", cnt, this, mgb);
        delete mgb;
        cnt++;
    }
}

static int __groupe_buffer_free(void *data)
{
    assert(data);

    if (data == NULL) {
        DRM_MEDIA_LOGE("BufferPool: free ptr is null");
        return 0;
    }

    MediaGroupBuffer *mgb = (MediaGroupBuffer *)data;
    if (mgb->pool == NULL) {
        DRM_MEDIA_LOGE("BufferPool: free pool ptr is null");
        return 0;
    }

    BufferPool *bp = (BufferPool *)mgb->pool;
    return bp->PutBuffer(mgb);
}

std::shared_ptr<MediaBuffer> BufferPool::GetBuffer(bool block)
{
    AutoLockMutex _alm(mtx);

    while (1) {
        if (!ready_buffers.size()) {
            if (block) {
                mtx.wait();
            } else {
                return nullptr;
            }
        }

        if (ready_buffers.size() > 0) {
            break;
        }
    }

    auto mgb = ready_buffers.front();
    ready_buffers.pop_front();
    busy_buffers.push_back(mgb);

    auto &&mb = std::make_shared<MediaBuffer>(mgb->GetPtr(), mgb->GetSize(), mgb->GetFD(), mgb->GetHandle(), mgb->GetDevFD(), mgb, __groupe_buffer_free);
    return mb;
}

int BufferPool::PutBuffer(MediaGroupBuffer *mgb)
{
    bool sucess = false;
    std::list<MediaGroupBuffer *>::iterator it;

    AutoLockMutex _alm(mtx);

    for (it = busy_buffers.begin(); it != busy_buffers.end();) {
        if (*it == mgb) {
            sucess = true;
            it = busy_buffers.erase(it);
            ready_buffers.push_back(mgb);
            mtx.notify();
            break;
        }

        it++;
    }

    if (!sucess) {
        DRM_MEDIA_LOGE("BufferPool: Unknow media group buffer:%p", mgb);
    }

    return sucess ? 0 : -1;
}

void BufferPool::DumpInfo()
{
    int id = 0;

    DRM_MEDIA_LOGI("##BufferPool DumpInfo:%p", this);
    DRM_MEDIA_LOGI("\tcnt:%d", buf_cnt);
    DRM_MEDIA_LOGI("\tsize:%d", buf_size);
    DRM_MEDIA_LOGI("\tready buffers(%zu):", ready_buffers.size());
    for (auto dev : ready_buffers) {
        DRM_MEDIA_LOGI("\t  #%02d Pool:[%p], mgb:[%p], ptr:[%p]", id++, dev->pool, dev, dev->GetPtr());
    }
    DRM_MEDIA_LOGI("\tbusy buffers(%zu):", busy_buffers.size());

    id = 0;
    for (auto dev : busy_buffers) {
        DRM_MEDIA_LOGI("\t  #%02d Pool:[%p], mgb:[%p], ptr:[%p]", id++, dev->pool, dev, dev->GetPtr());
    }
}
}
