/**
 * @file lv_fs_if.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_if.h"

#if LV_USE_FS_IF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_FS_IF_PC
void lv_fs_if_pc_init(void);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register driver(s) for the File system interface
 */
void lv_fs_if_init(void)
{

#if LV_FS_IF_PC != '\0'
	lv_fs_if_pc_init();
#endif

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
