#ifndef _GUIX_CALCULATOR_EVALUATE_H_
#define _GUIX_CALCULATOR_EVALUATE_H_

#ifdef __cplusplus
extern "C" {
#endif

extern GX_CHAR result[128];

void evaluate(GX_STRING *expression, GX_BOOL persent_flag);
VOID safe_string_copy(GX_CHAR *put, const GX_CHAR *get, INT max_len);
UINT string_length_get(GX_CONST GX_CHAR *input_string, UINT max_string_length);

#ifdef __cplusplus
}
#endif

#endif
