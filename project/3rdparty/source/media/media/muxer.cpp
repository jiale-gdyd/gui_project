#include <media/utils/muxer.h>

namespace libdrm {
DEFINE_REFLECTOR(Muxer)

DEFINE_FACTORY_COMMON_PARSE(Muxer)

Muxer::Muxer(const char *param _DRM_UNUSED) : io_output(nullptr), m_handler(nullptr), m_write_callback_func(nullptr)
{

}

DEFINE_PART_FINAL_EXPOSE_PRODUCT(Muxer, Muxer)
}
