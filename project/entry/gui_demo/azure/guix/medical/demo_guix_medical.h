#ifndef _DEMO_GUIX_MEDICAL_H_
#define _DEMO_GUIX_MEDICAL_H_

#include <gx_api.h>

#ifdef __cplusplus
extern "C" {
#endif

VOID ToggleScreen(GX_WIDGET *new_win, GX_WIDGET *old_win);
VOID GetPatientName(GX_STRING *string);
VOID GetPatientDOB(GX_STRING* string);

#ifdef __cplusplus
}
#endif

#endif
