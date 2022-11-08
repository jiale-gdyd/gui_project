#ifndef UNITTEST_HOST_BOOST_PRIVATE_HPP
#define UNITTEST_HOST_BOOST_PRIVATE_HPP

#ifndef unittest_print
#define unittest_print(msg, ...)        fprintf(stderr, msg, ##__VA_ARGS__);
#endif

#ifndef unittest_error
#define unittest_error(msg, ...)        unittest_print("\033[1;31m[TEST][E]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_warn
#define unittest_warn(msg, ...)         unittest_print("\033[1;33m[TEST][W]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_info
#define unittest_info(msg, ...)         unittest_print("\033[1;32m[TEST][I]: " msg "\033[0m\n", ##__VA_ARGS__)
#endif

#ifndef unittest_debug
#define unittest_debug(msg, ...)        {}
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int host_boost_signal_slots_demo_exit(void);
int host_boost_signal_slots_demo_init(int argc, char *argv[]);

#if defined(__cplusplus)
}
#endif

#endif
