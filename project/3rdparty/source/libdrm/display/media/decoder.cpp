#include <libdrm/display/decoder.hpp>

namespace libdrm {
DEFINE_REFLECTOR(Decoder)
DEFINE_FACTORY_COMMON_PARSE(Decoder)

DEFINE_PART_FINAL_EXPOSE_PRODUCT(AudioDecoder, Decoder)
DEFINE_PART_FINAL_EXPOSE_PRODUCT(VideoDecoder, Decoder)
}
