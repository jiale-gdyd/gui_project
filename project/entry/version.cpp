#include <string>
#include <iomanip>
#include <iostream>

#include "process_init.h"
#include <linux/git_commit.h>

/**
 * 函数名称: app_version_header
 * 功能描述: 应用程序版本头
 * 输入参数: 无
 * 输出参数: 无
 * 返回说明: 成功返回0，失败返回<0
 */
int app_version_header(void)
{
    std::string appName = std::string(APP_NAME);
    std::string appAuthor = std::string(MODIFY_AUTHOR);
    std::string appVersion = std::string(BUILD_VERSION);
    std::string buildDateTime = std::string(BUILD_DATE) + " " + std::string(BUILD_TIME);
    std::string buildPlatform = std::string(BUILD_OS) + " " + std::string(OS_CODENAME) + " " + std::string(OS_VERSION);
    std::string gitCommitMesg = std::string(GIT_SHORT_COMMIT);

    size_t maxSize = appName.length();
    maxSize = maxSize > appVersion.length() ? maxSize : appVersion.length();
    maxSize = maxSize > appAuthor.length() ? maxSize : appAuthor.length();
    maxSize = maxSize > buildPlatform.length() ? maxSize : buildPlatform.length();
    maxSize = maxSize > buildDateTime.length() ? maxSize : buildDateTime.length();
    maxSize = maxSize > gitCommitMesg.length() ? maxSize : gitCommitMesg.length();

    std::cout << "\t\t\t***********************"                 << std::setfill('*') << std::setw(maxSize + 1 + 2) << "**\n";
    std::cout << "\t\t\t***********************"                 << std::setfill('*') << std::setw(maxSize + 1 + 2) << "**\n";
    std::cout << "\t\t\t** Application Name : " << appName       << std::setfill(' ') << std::setw(maxSize + 1 - appName.length() + 3)       << " **\n";
    std::cout << "\t\t\t** Build Version    : " << appVersion    << std::setfill(' ') << std::setw(maxSize + 1 - appVersion.length() + 3)    << " **\n";
    std::cout << "\t\t\t** Build Author     : " << appAuthor     << std::setfill(' ') << std::setw(maxSize + 1 - appAuthor.length() + 3)     << " **\n";
    std::cout << "\t\t\t** Build Platform   : " << buildPlatform << std::setfill(' ') << std::setw(maxSize + 1 - buildPlatform.length() + 3) << " **\n";
    std::cout << "\t\t\t** Build Date Time  : " << buildDateTime << std::setfill(' ') << std::setw(maxSize + 1 - buildDateTime.length() + 3) << " **\n";
    std::cout << "\t\t\t** Git Commit Mesg  : " << gitCommitMesg << std::setfill(' ') << std::setw(maxSize + 1 - gitCommitMesg.length() + 3) << " **\n";
    std::cout << "\t\t\t***********************"                 << std::setfill('*') << std::setw(maxSize + 1 + 2) << "**\n";
    std::cout << "\t\t\t***********************"                 << std::setfill('*') << std::setw(maxSize + 1 + 2) << "**\n";

    return 0;
}
