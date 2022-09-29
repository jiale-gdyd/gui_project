#ifndef SINGLEFILE_KCONFIG_H
#define SINGLEFILE_KCONFIG_H

#include <generated/autoconf.h>

#ifdef CONFIG_CPU_BIG_ENDIAN
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN                                    (4321)
#endif
#else
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN                                 (1234)
#endif
#endif

#define __ARG_PLACEHOLDER_1                             0,
#define __take_second_arg(__ignored, val, ...)          val

// "&&" / "||"在某些表达式中的使用受到限制。以下仅允许使用宏展开计算"and" / "or"
#define __and(x, y)                                     ___and(x, y)
#define ___and(x, y)                                    ____and(__ARG_PLACEHOLDER_##x, y)
#define ____and(arg1_or_junk, y)                        __take_second_arg(arg1_or_junk y, 0)

#define __or(x, y)                                      ___or(x, y)
#define ___or(x, y)                                     ____or(__ARG_PLACEHOLDER_##x, y)
#define ____or(arg1_or_junk, y)                         __take_second_arg(arg1_or_junk 1, y)

/**
 * 为可能定义或可能未定义的arg获取在C和CPP中有效的东西是很棘手的。在这里，如果我们有“#define CONFIG_BOOGER 1”，
 * 我们匹配占位符定义，为arg1插入“0”并生成三元组(0, 1, 0)。然后最后一步樱桃选择第二个参数（一个）。当CONFIG_BOOGER未定义时，
 * 我们生成一个 (... 1, 0) 对，当最后一步cherry 选择第二个参数时，我们得到一个零
 */
#define config_enabled(cfg)                             _config_enabled(cfg)
#define _config_enabled(value)                          __config_enabled(__ARG_PLACEHOLDER_##value)
#define __config_enabled(arg1_or_junk)                  ___config_enabled(arg1_or_junk 1, 0)
#define ___config_enabled(__ignored, val, ...)          val

/**
 * 而对于一个C/CPP来说，使用arg可能是不容易的。在这里，如果我们在占位符define上匹配了"#define CONFIG_BOOGER 1"，
 * 则为arg1插入"0,"，并生成三元组（0，1，0）。最后一步cherry选择第二个arg(一个)。
 * 如果没有定义CONFIG_BOOGER，我们将生成一个(... 1，0)对，当最后一步cherry选择第二个arg时，我们得到一个0
 */
#define __is_defined(x)                                 ___is_defined(x)
#define ___is_defined(val)                              ____is_defined(__ARG_PLACEHOLDER_##val)
#define ____is_defined(arg1_or_junk)                    __take_second_arg(arg1_or_junk 1, 0)

#if defined(CONFIG_TPL_BUILD)
#define _CONFIG_PREFIX TPL_
#elif defined(CONFIG_SPL_BUILD)
#define _CONFIG_PREFIX SPL_
#else
#define _CONFIG_PREFIX
#endif

#define config_val(cfg)                                 _config_val(_CONFIG_PREFIX, cfg)
#define _config_val(pfx, cfg)                           __config_val(pfx, cfg)
#define __config_val(pfx, cfg)                          CONFIG_ ## pfx ## cfg

/**
 * 如果CONFIG_SPL_BUILD未定义，则CONFIG_VAL(FOO)计算为CONFIG_FOO的值，如果CONFIG_SPL_BUILD已定义，则计算为CONFIG_SPL_FOO。
 * CONFIG_TPL_FOO如果定义了CONFIG_TPL_BUILD
 */
#define CONFIG_VAL(option)                              config_val(option)

#define __arg6(a1, a2, a3, a4, a5, a6, ...)             a6
#define __count_args(...)                               __arg6(dummy, ##__VA_ARGS__, 4, 3, 2, 1, 0)

#define __concat(a, b)                                  ___concat(a, b)
#define ___concat(a, b)                                 a ## b

#define __unwrap(...)                                   __VA_ARGS__
#define __unwrap1(case1, case0)                         __unwrap case1
#define __unwrap0(case1, case0)                         __unwrap case0

#define __CONFIG_IS_ENABLED_1(option)                   __CONFIG_IS_ENABLED_3(option, (1), (0))
#define __CONFIG_IS_ENABLED_2(option, case1)            __CONFIG_IS_ENABLED_3(option, case1, ())
#define __CONFIG_IS_ENABLED_3(option, case1, case0)     __concat(__unwrap, config_enabled(CONFIG_VAL(option))) (case1, case0)

/**
 * 如果CONFIG_FOO设置为'y'，则IS_BUILTIN(CONFIG_FOO)的计算结果为1，否则为0。对于布尔型选项，这相当于IS_ENABLED(CONFIG_FOO)
 */
#define IS_BUILTIN(option)                              __is_defined(option)

/**
 * 如果CONFIG_FOO设置为'm'，则IS_MODULE(CONFIG_FOO)的计算结果为1，否则为0
 */
#define IS_MODULE(option)                               __is_defined(option##_MODULE)

/**
 * 如果当前编译的代码可以调用在基于CONFIG_FOO编译的代码中定义的函数，则IS_REACHABLE(CONFIG_FOO)
 * 的计算结果为1。这类似于IS_ENABLED()，但当CONFIG_FOO设置为'm'时，从内置代码调用时返回false
 */
#define IS_REACHABLE(option)                            __or(IS_BUILTIN(option), __and(IS_MODULE(option), __is_defined(MODULE)))

/**
 * 如果CONFIG_FOO设置为'y'或'm'，则IS_ENABLED(CONFIG_FOO)的计算结果为1，否则为0
 */
#define IS_ENABLED(option)                              __or(IS_BUILTIN(option), IS_MODULE(option))
#define IS_ENABLEDEX(option)                            (config_enabled(option))

// 如果CONFIG_FOO设置为'y'或'm'，则IF_ENABLED(CONFIG_FOO, ptr)计算为(ptr)，否则为NULL
#define IF_ENABLED(option, ptr)                         (IS_ENABLED(option) ? (ptr) : NULL)

/**
 * CONFIG_IS_ENABLED(FOO)扩展为:
 *      1如果CONFIG_SPL_BUILD未定义且CONFIG_FOO设置为'y'，
 *      1如果定义了CONFIG_SPL_BUILD并且CONFIG_SPL_FOO设置为'y'，
 *      1如果定义了CONFIG_TPL_BUILD并且CONFIG_TPL_FOO设置为'y'，
 *      否则为0
 * 
 * CONFIG_IS_ENABLED(FOO, (abc))扩展为:
 *      abc如果CONFIG_SPL_BUILD未定义且CONFIG_FOO设置为'y'，
 *      abc如果定义了CONFIG_SPL_BUILD并且CONFIG_SPL_FOO设置为'y'，
 *      abc如果定义了CONFIG_TPL_BUILD并且CONFIG_TPL_FOO设置为'y'，
 *      啥也不是
 * 
 * CONFIG_IS_ENABLED(FOO, (abc), (def))扩展为:
 *      abc如果CONFIG_SPL_BUILD未定义且CONFIG_FOO设置为'y'，
 *      abc如果定义了CONFIG_SPL_BUILD并且CONFIG_SPL_FOO设置为'y'，
 *      abc如果定义了CONFIG_TPL_BUILD并且CONFIG_TPL_FOO设置为'y'，
 *      否则为def
 * 
 * 可选的第二个和第三个参数必须用括号括起来；允许包含一个尾随逗号，例如用于:
 *      CONFIG_IS_ENABLED(ACME, ({.compatible = "acme,frobnozzle"},))
 * 
 * 如果设置了CONFIG_ACME(或CONFIG_SPL_ACME/CONFIG_TPL_ACME，取决于构建上下文)，它将向正在定义的数组添加一个条目，否则没有
 */
#define CONFIG_IS_ENABLED(option, ...)					__concat(__CONFIG_IS_ENABLED_, __count_args(option, ##__VA_ARGS__)) (option, ##__VA_ARGS__)

#endif
