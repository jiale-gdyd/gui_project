#ifndef LIBDRM_DISPLAY_STREAM_H
#define LIBDRM_DISPLAY_STREAM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
typedef struct {
    int (*close)(void *stream);
    size_t (*read)(void *ptr, size_t size, size_t nmemb, void *stream);
    size_t (*write)(const void *ptr, size_t size, size_t nmemb, void *stream);
    int (*seek)(void *stream, int64_t offset, int whence);
    long (*tell)(void *stream);
} StreamOperation;
}
#endif

#include "control.h"
#include "image.h"
#include "media_reflector.h"
#include "utils.h"

namespace libdrm {
DECLARE_FACTORY(Stream)

DECLARE_REFLECTOR(Stream)

#define DEFINE_STREAM_FACTORY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT)                                                           \
    DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, REAL_PRODUCT::GetStreamName(), FINAL_EXPOSE_PRODUCT, Stream)                   \
    DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                                          \
    DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT, Open() < 0)

class MediaBuffer;

int local_close(void *stream);

class Stream {
public:
    static StreamOperation c_operations;

    Stream() : readable(false), writeable(false), seekable(false) {}
    virtual ~Stream() = default;

    virtual size_t Read(void *ptr, size_t size, size_t nmemb) = 0;
    virtual size_t Write(const void *ptr, size_t size, size_t nmemb) = 0;

    virtual int Seek(int64_t offset, int whence) = 0;
    virtual long Tell() = 0;

    virtual int NewStream(std::string new_path _DRM_UNUSED) {
        return -1;
    };

    virtual size_t WriteAndClose(const void *ptr _DRM_UNUSED, size_t size _DRM_UNUSED, size_t nmemb _DRM_UNUSED) {
        return -1;
    };

    virtual int ReName(std::string old_path _DRM_UNUSED, std::string new_path _DRM_UNUSED) {
        return -1;
    };

    virtual bool Readable() {
        return readable;
    }

    virtual bool Writeable() {
        return writeable;
    }

    virtual bool Seekable() {
        return seekable;
    }

    void SetReadable(bool able) {
        readable = able;
    }

    void SetWriteable(bool able) {
        writeable = able;
    }

    void SetSeekable(bool able) {
        seekable = able;
    }

    virtual bool Eof() {
        return false;
    }

    virtual std::shared_ptr<MediaBuffer> Read() {
        return nullptr;
    }

    virtual bool Write(std::shared_ptr<MediaBuffer>) {
        return false;
    }

    virtual int IoCtrl(unsigned long int request _DRM_UNUSED, ...) {
        return -1;
    }

    virtual int SubIoCtrl(unsigned long int request _DRM_UNUSED, void *arg, int size = 0) {
        SubRequest subreq = {request, size, arg};
        return IoCtrl(S_SUB_REQUEST, &subreq);
    }

    bool ReadImage(void *ptr, const DrmImageInfo &info);

protected:
    virtual int Open() = 0;
    virtual int Close() = 0;

    friend int local_close(void *stream);

private:
    bool readable;
    bool writeable;
    bool seekable;

    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Stream)
};
}

#endif
