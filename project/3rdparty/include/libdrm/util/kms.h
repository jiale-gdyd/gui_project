#ifndef LIBDRM_UTIL_KMS_H
#define LIBDRM_UTIL_KMS_H

#if defined(__cplusplus)
extern "C" {
#endif

int util_open(const char *device, const char *module);

const char *util_lookup_encoder_type_name(unsigned int type);
const char *util_lookup_connector_status_name(unsigned int type);

#if defined(__cplusplus)
}
#endif

#endif
