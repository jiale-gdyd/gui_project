#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#define ARGC_START              0
#define ARG_POSITION(x)         x

namespace utils {
/* 用于提取标志、值和字符串的命令行解析器 */
class commandLine {
public:
    commandLine(const int argc, char *argv[], const char *extraFlag = NULL) {
        mArgc = argc;
        mArgv = argv;
        AddFlag(extraFlag);
    }

    commandLine(const int argc, char *argv[], const char **extraArgs) {
        mArgc = argc;
        mArgv = argv;
        AddArgs(extraArgs);
    }

    /**
     * 函数名称: GetFlag
     * 功能描述: 检查指定的标志是否包含在命令行中。例如，如果argv包含'--foo'，那么'GetFlag("foo")'将返回true
     * 输入参数: argName              --> 待解析的参数名
     *          allowOtherDelimiters --> 如果为true(默认)，则argName将匹配包含'-'或'_'的项。例如，'--foo-bar'和'--foo_bar'是相同的
     * 输出参数: 无
     * 返回说明: 如果找到带有argName的标志为true，如果没有找到带有argName的标志为false
     */
    bool GetFlag(const char *argName, bool allowOtherDelimiters = true) const {
        if (mArgc < 1) {
            return false;
        }

        for (int i = ARGC_START; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start == 0) {
                continue;
            }

            const char *string_argv = &mArgv[i][string_start];
            const char *equal_pos = strchr(string_argv, '=');

            const int length = (int)strlen(argName);
            const int argv_length = (int)(equal_pos == 0 ? strlen(string_argv) : equal_pos - string_argv);

            if ((length == argv_length) && !strncasecmp(string_argv, argName, length)) {
                return true;
            }
        }

        if (!allowOtherDelimiters) {
            return false;
        }

        /* 尝试寻找交换了分隔符的参数 */
        char *swapped_ref = strSwapDelimiter(argName);
        if (!swapped_ref) {
            return false;
        }

        const bool value = GetFlag(swapped_ref, false);
        free(swapped_ref);

        return value;
    }

    /**
     * 函数名称: GetFloat
     * 功能描述: 获取浮动的参数。例如，如果argv包含'--foo=3.14159'，那么'GetInt("foo")'将返回'3.14159f'
     * 输入参数: argName              --> 待解析的参数名
     *          defaultValue         --> 默认值
     *          allowOtherDelimiters --> 如果为true(默认)，则argName将匹配包含'-'或'_'的项。例如，'--foo-bar'和'--foo_bar'是相同的
     * 输出参数: 无
     * 返回说明: 如果找不到参数则返回默认值(默认情况下为'0.0')。否则，返回实参的值
     */
    float GetFloat(const char *argName, float defaultValue = 0.0f, bool allowOtherDelimiters = true) const {
        if (mArgc < 1) {
            return defaultValue;
        }

        float value = -1;
        bool bFound = false;

        for (int i = ARGC_START; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start == 0) {
                continue;
            }

            const int length = (int)strlen(argName);
            const char *string_argv = &mArgv[i][string_start];

            if (!strncasecmp(string_argv, argName, length)) {
                if ((length + 1) <= (int)strlen(string_argv)) {
                    int auto_inc = (string_argv[length] == '=') ? 1 : 0;
                    value = (float)atof(&string_argv[length + auto_inc]);
                } else {
                    value = 0.f;
                }

                bFound = true;
                continue;
            }
        }

        if (bFound) {
            return value;
        }

        if (!allowOtherDelimiters) {
            return defaultValue;
        }

        /* 尝试寻找交换了分隔符的参数 */
        char *swapped_ref = strSwapDelimiter(argName);
        if (!swapped_ref) {
            return defaultValue;
        }

        value = GetFloat(swapped_ref, defaultValue, false);
        free(swapped_ref);

        return value;
    }

    /**
     * 函数名称: GetInt
     * 功能描述: 获取整数参数。例如，如果argv包含'--foo=100'，那么'GetInt("foo")'将返回'100'
     * 输入参数: argName              --> 待解析的参数名
     *          defaultValue         --> 默认值
     *          allowOtherDelimiters --> 如果为true(默认)，则argName将匹配包含'-'或'_'的项。例如，'--foo-bar'和'--foo_bar'是相同的
     * 输出参数: 无
     * 返回说明: 如果找不到参数则返回默认值(默认情况下为'0')。否则，返回实参的值
     */
    int GetInt(const char *argName, int defaultValue = 0, bool allowOtherDelimiters = true) const {
        if (mArgc < 1) {
            return defaultValue;
        }

        int value = -1;
        bool bFound = false;

        for (int i = ARGC_START; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start == 0) {
                continue;
            }

            const int length = (int)strlen(argName);
            const char *string_argv = &mArgv[i][string_start];

            if (!strncasecmp(string_argv, argName, length)) {
                if ((length + 1) <= (int)strlen(string_argv)) {
                    int auto_inc = (string_argv[length] == '=') ? 1 : 0;
                    value = atoi(&string_argv[length + auto_inc]);
                } else {
                    value = 0;
                }

                bFound = true;
                continue;
            }
        }

        if (bFound) {
            return value;
        }

        if (!allowOtherDelimiters) {
            return defaultValue;
        }

        /* 尝试寻找交换了分隔符的参数 */
        char *swapped_ref = strSwapDelimiter(argName);
        if (!swapped_ref) {
            return defaultValue;
        }

        value = GetInt(swapped_ref, defaultValue, false);
        free(swapped_ref);

        return value;
    }

    /**
     * 函数名称: GetUint
     * 功能描述: 获取整数参数。例如，如果argv包含'--foo=100'，那么'GetInt("foo")'将返回'100'
     * 输入参数: argName              --> 待解析的参数名
     *          defaultValue         --> 默认值
     *          allowOtherDelimiters --> 如果为true(默认)，则argName将匹配包含'-'或'_'的项。例如，'--foo-bar'和'--foo_bar'是相同的
     * 输出参数: 无
     * 返回说明: 如果找不到参数则返回默认值(默认情况下为'0')。否则，返回实参的值
     */
    uint32_t GetUint(const char *argName, uint32_t defaultValue = 0, bool allowOtherDelimiters = true) const {
        const int val = GetInt(argName, (int)defaultValue, allowOtherDelimiters);
        if (val < 0) {
            return defaultValue;
        }

        return val;
    }

    /**
     * 函数名称: GetString
     * 功能描述: 获取字符串参数。例如，如果argv包含'--foo=bar'，那么'GetString("foo")'将返回"bar"
     * 输入参数: argName              --> 待解析的参数名
     *          defaultValue         --> 默认值
     *          allowOtherDelimiters --> 如果为true(默认)，则argName将匹配包含'-'或'_'的项。例如，'--foo-bar'和'--foo_bar'是相同的
     * 输出参数: 无
     * 返回说明: 如果找不到参数，则返回默认值(默认为'NULL')。否则，返回一个指向'argv'数组中的参数值字符串的指针
     */
    const char *GetString(const char *argName, const char *defaultValue = NULL, bool allowOtherDelimiters = true) const {
        if (mArgc < 1) {
            return defaultValue;
        }

        for (int i = ARGC_START; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start == 0) {
                continue;
            }

            const int length = (int)strlen(argName);
            char *string_argv = (char *)&mArgv[i][string_start];

            if (!strncasecmp(string_argv, argName, length)) {
                return (string_argv + length + 1);
            }
        }

        if (!allowOtherDelimiters) {
            return defaultValue;
        }

        /* 尝试寻找交换了分隔符的参数 */
        char *swapped_ref = strSwapDelimiter(argName);
        if (!swapped_ref) {
            return defaultValue;
        }

        const char *value = GetString(swapped_ref, defaultValue, false);
        free(swapped_ref);

        return value;
    }

    /**
     * 函数名称: GetPosition
     * 功能描述: 获取位置字符串参数。位置参数不被命名，而是由它们在列表中的索引引用。例如，如果命令行包含'my-program --foo=bar /path/to/my_file.txt'，那么'GetString(0)'将返回"/path/to/my_file.txt"
     * 输入参数: position     --> 参数位置
     *          defaultValue --> 默认值
     * 输出参数: 无
     * 返回说明: 如果找不到参数，则返回默认值(默认为'NULL')。否则，返回一个指向'argv'数组中的参数值字符串的指针
     */
    const char *GetPosition(unsigned int position, const char *defaultValue = NULL) const {
        if ((mArgc < 1) || (position >= GetPositionArgs())) {
            return defaultValue;
        }

        unsigned int position_count = 0;
        for (int i = 1; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start != 0) {
                continue;
            }

            if (position == position_count) {
                return mArgv[i];
            }

            position_count++;
        }

        return defaultValue;
    }

    /**
     * 函数名称: GetPositionArgs
     * 功能描述: 获取命令行中位置参数的数量。位置参数是那些没有名称的参数
     * 输入参数: 无
     * 输出参数: 无
     * 返回说明: 命令行中位置参数的数量
     */
    unsigned int GetPositionArgs() const {
        unsigned int position_count = 0;

        for (int i = 1; i < mArgc; i++) {
            const int string_start = strFindDelimiter('-', mArgv[i]);
            if (string_start != 0) {
                continue;
            }

            position_count++;
        }

        return position_count;
    }

private:
    /**
     * 函数名称: AddArg
     * 功能描述: 向命令行添加一个参数
     * 输入参数: arg --> 添加的参数
     * 输出参数: 无
     * 返回说明: 无
     */
    void AddArg(const char *arg) {
        if (!arg) {
            return;
        }

        const size_t arg_length = strlen(arg);
        if (arg_length == 0) {
            return;
        }

        const int new_argc = mArgc + 1;
        char **new_argv = (char **)malloc(sizeof(char *) * new_argc);
        if (!new_argv) {
            return;
        }

        for (int n = 0; n < mArgc; n++) {
            new_argv[n] = mArgv[n];
        }

        new_argv[mArgc] = (char *)malloc(arg_length + 1);
        if (!new_argv[mArgc]) {
            return;
        }

        strcpy(new_argv[mArgc], arg);
        mArgc = new_argc;
        mArgv = new_argv;
    }

    /**
     * 函数名称: AddArgs
     * 功能描述: 向命令行添加参数
     * 输入参数: arg --> 添加的参数
     * 输出参数: 无
     * 返回说明: 无
     */
    void AddArgs(const char **args) {
        if (!args) {
            return;
        }

        int arg_count = 0;
        while (true) {
            if (!args[arg_count]) {
                return;
            }

            AddArg(args[arg_count]);
            arg_count++;
        }
    }

    /**
     * 函数名称: AddFlag
     * 功能描述: 在命令行中添加一个标志。
     * 输入参数: flag --> 添加的标志
     * 输出参数: 无
     * 返回说明: 无
     */
    void AddFlag(const char *flag) {
        if (!flag || (strlen(flag) == 0)) {
            return;
        }

        if (GetFlag(flag)) {
            return;
        }

        const std::string arg = std::string("--") + flag;
        AddArg(arg.c_str());
    }

public:
    /**
     * 函数名称: Print
     * 功能描述: 打印出命令行以供参考
     * 输入参数: 无
     * 输出参数: 无
     * 返回说明: 无
     */
    void Print() const {
        for (int n = 0; n < mArgc; n++) {
            printf("%s ", mArgv[n]);
        }
        printf("\n");
    }

private:
    /* 在一个字符串中搜索一个开头字符的结尾(例如: '--foo') */
    static int strFindDelimiter(char delimiter, const char *str) {
        int string_start = 0;

        while (str[string_start] == delimiter) {
            string_start++;
        }

        if (string_start >= (int)strlen(str)-1) {
            return 0;
        }

        return string_start;
    }

    /* 将连字符替换为下划线，反之亦然(如果没有变化则返回NULL) */
    static char *strSwapDelimiter(const char *str) {
        if (!str) {
            return NULL;
        }

        /* 确定原始字符是否在字符串中 */
        bool found = false;
        const int str_length = strlen(str);

        for (int n = 0; n < str_length; n++) {
            if ((str[n] == '-') || (str[n] == '_' )) {
                found = true;
                break;
            }
        }

        if (!found) {
            return NULL;
        }

        /* 分配要修改的新字符串 */
        char *new_str = (char *)malloc(str_length);
        if (!new_str) {
            return NULL;
        }

        strcpy(new_str, str);

        /* 替换旧字符的实例 */
        for (int n = 0; n < str_length; n++) {
            if (new_str[n] == '-') {
                new_str[n] = '_';
            } else if (new_str[n] == '_') {
                new_str[n] = '-';
            }
        }

        return new_str;
    }

private:
    int  mArgc;     /* 从main()中创建对象的参数计数 */
    char **mArgv;   /* 从main()中创建对象的参数字符串 */
};
}

#endif
