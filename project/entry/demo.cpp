#include "process_init.h"

#if defined(CONFIG_UNITTEST)
#include <unittest/unittest.h>
#endif

#if defined(CONFIG_LIBDRM_TOOLS)
#include <libdrm/libdrm.h>
#endif

int main(int argc, char *argv[])
{
    app_version_header();

#if defined(CONFIG_LIBDRM_TOOLS)
#if defined(CONFIG_LIBDRM_MODETEST)
    return libdrm_modetest_main(argc, argv);
#elif defined(CONFIG_LIBDRM_MODEPRINT)
    return libdrm_modeprint_main(argc, argv);
#endif
#elif defined(CONFIG_UNITTEST)
    unittest_init(argc, argv);
    return unittest_exit();
#endif

    return -1;
}
