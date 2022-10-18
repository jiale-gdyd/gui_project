#include <media/utils/filter.h>

namespace libdrm {
DEFINE_REFLECTOR(Filter)

DEFINE_FACTORY_COMMON_PARSE(Filter)

DEFINE_PART_FINAL_EXPOSE_PRODUCT(Filter, Filter)

Filter::~Filter()
{

}

int Filter::Process(std::shared_ptr<MediaBuffer> input _DRM_UNUSED, std::shared_ptr<MediaBuffer> &output _DRM_UNUSED)
{
    errno = ENOSYS;
    return -1;
}

int Filter::SendInput(std::shared_ptr<MediaBuffer> input _DRM_UNUSED)
{
    errno = ENOSYS;
    return -1;
}

std::shared_ptr<MediaBuffer> Filter::FetchOutput()
{
    errno = ENOSYS;
    return nullptr;
}
}