#ifndef TK_COMMON_COORD_H
#define TK_COMMON_COORD_H

#include "tkc/types_def.h"
#include "tkc/rect.h"

BEGIN_C_DECLS

ret_t common_coord_init(void);
ret_t common_coord_deinit(void);

ret_t common_coord_set(point_t coord);
ret_t common_coord_get(point_t *p_coord);

END_C_DECLS

#endif
