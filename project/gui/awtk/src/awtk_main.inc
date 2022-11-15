#include "awtk_version.h"

#include "awtk_tkc.h"
#include "awtk_base.h"
#include "awtk_global.h"
#include "awtk_widgets.h"

#ifndef WITHOUT_EXT_WIDGETS
#include "awtk_ext_widgets.h"
#endif

BEGIN_C_DECLS
extern ret_t assets_init(void);

extern ret_t awtk_application_init(void);
extern ret_t awtk_application_exit(void);
END_C_DECLS

#ifndef APP_DEFAULT_FONT
#define APP_DEFAULT_FONT                "default"
#endif

#ifndef APP_NAME
#define APP_NAME                        "awtk_gui"
#endif

#ifndef APP_RES_ROOT
#define APP_RES_ROOT                    NULL
#endif

#ifndef APP_ENABLE_CONSOLE
#define APP_ENABLE_CONSOLE              TRUE
#endif

#include "base/custom_keys.inc"
#include "base/asset_loader_zip.h"

static int gui_app_start_ex(size_t disp_width, size_t disp_height, int app_type, const char *app_name, const char *res_root, const char *app_font);

static inline int awtk_application_enter(size_t disp_width, size_t disp_height, int app_type, const char *app_name, const char *res_root, const char *app_font)
{
    return gui_app_start_ex(disp_width, disp_height, app_type, app_name ? app_name : APP_NAME, res_root ? res_root : APP_RES_ROOT, app_font ? app_font : APP_DEFAULT_FONT);
}

int gui_app_start_ex(size_t disp_width, size_t disp_height, int app_type, const char *app_name, const char *res_root, const char *app_font)
{
    tk_init(disp_width, disp_height, (app_type_t)app_type, app_name, res_root);

#ifdef ASSETS_ZIP
    assets_manager_set_res_root(assets_manager(), "");
    log_debug("Load assets from zip:[%s]\n", ASSETS_ZIP);
    assets_manager_set_loader(assets_manager(), asset_loader_zip_create(ASSETS_ZIP));
#endif

#if defined(WITH_LCD_PORTRAIT)
    if (disp_width > disp_height) {
        tk_set_lcd_orientation(LCD_ORIENTATION_90);
    }
#endif

#ifdef WITH_LCD_LANDSCAPE
    if (disp_width < disp_height) {
        tk_set_lcd_orientation(LCD_ORIENTATION_90);
    }
#endif

#ifdef APP_LCD_ORIENTATION
    if (system_info()->app_type != APP_SIMULATOR) {
#if defined(APP_ENABLE_FAST_LCD_PORTRAIT)
        tk_enable_fast_lcd_portrait(TRUE);
#endif
        tk_set_lcd_orientation(APP_LCD_ORIENTATION);
    }
#endif

    system_info_set_default_font(system_info(), app_font);
    assets_init();

#ifndef WITH_FS_RES
    locale_info_reload(locale_info());
#endif

#ifndef WITHOUT_EXT_WIDGETS
    tk_ext_widgets_init();
#endif

#ifdef NDEBUG
    log_set_log_level(LOG_LEVEL_INFO);
#else
    log_set_log_level(LOG_LEVEL_DEBUG);
#endif
    log_info("Build at: %s %s\n", __DATE__, __TIME__);

#ifdef ENABLE_CURSOR
    window_manager_set_cursor(window_manager(), "cursor");
#endif

#if defined(APP_DEFAULT_LANGUAGE) && defined(APP_DEFAULT_COUNTRY)
    locale_info_change(locale_info(), APP_DEFAULT_LANGUAGE, APP_DEFAULT_COUNTRY);
#endif

    custom_keys_init(TRUE);
    awtk_application_init();

    tk_run();

    awtk_application_exit();
    custom_keys_deinit(TRUE);

    tk_exit();

    fflush(stdout);
    return 0;
}
