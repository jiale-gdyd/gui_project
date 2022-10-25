﻿#ifndef TK_INPUT_THREAD_H
#define TK_INPUT_THREAD_H

#include "tkc/thread.h"
#include "input_dispatcher.h"

BEGIN_C_DECLS

tk_thread_t *input_thread_run(const char *filename, input_dispatch_t dispatch, void *ctx, int32_t max_x, int32_t max_y);

END_C_DECLS

#endif
