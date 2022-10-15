#ifndef LIBDRM_DISPLAY_FILTER_H
#define LIBDRM_DISPLAY_FILTER_H

#include <memory>
#include <stdarg.h>

#include "control.h"
#include "media_reflector.h"

namespace libdrm {
DECLARE_FACTORY(Filter)

DECLARE_REFLECTOR(Filter)

#define DEFINE_FILTER_FACTORY(REAL_PRODUCT, FINAL_EXPOSE_PRODUCT)                                           \
    DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, REAL_PRODUCT::GetFilterName(), FINAL_EXPOSE_PRODUCT, Filter)   \
    DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                          \
    DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, Filter, GetError() < 0)

#define DEFINE_COMMON_FILTER_FACTORY(REAL_PRODUCT)                                                          \
    DEFINE_FILTER_FACTORY(REAL_PRODUCT, Filter)

class MediaBuffer;

class Filter {
public:
    virtual ~Filter() = 0;

    static const char *GetFilterName() {
        return nullptr;
    }

    virtual int Process(std::shared_ptr<MediaBuffer> input, std::shared_ptr<MediaBuffer> &output);

    virtual int SendInput(std::shared_ptr<MediaBuffer> input);
    virtual std::shared_ptr<MediaBuffer> FetchOutput();

    virtual int IoCtrl(unsigned long int request _DRM_UNUSED, ...) {
        return -1;
    }

    DEFINE_ERR_GETSET()
    DECLARE_PART_FINAL_EXPOSE_PRODUCT(Filter)
};
}

#endif
