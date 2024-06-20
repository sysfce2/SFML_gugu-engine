////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/System/Platform.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/System/String.h"
#include "Gugu/System/Path.h"

#include <iostream>
#include <fstream>

#if defined(GUGU_OS_WINDOWS)
    #include <windows.h>
    #include <shellapi.h>
#endif

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

void WriteInConsole(const std::string& _strLine, bool printInIDE)
{
    std::cout << _strLine;

#if defined(GUGU_OS_WINDOWS) && defined(GUGU_ENV_VISUAL)
    if (printInIDE)
    {
        OutputDebugStringA(_strLine.c_str());
    }
#endif
}

void WriteInConsoleEndline(const std::string& _strLine, bool printInIDE)
{
    WriteInConsole(_strLine + "\n", printInIDE);
}

void WriteInFileEndline(const std::string& _strFileName, const std::string& _strLine)
{
    std::ofstream oFile;
    oFile.open(_strFileName.c_str(), std::ios::out | std::ios::app);

    if (oFile)
    {
        oFile << _strLine << std::endl;
        oFile.close();
    }
}

void OpenFileExplorer(const std::string& path)
{
#if defined(GUGU_OS_WINDOWS)
    std::string normalizedPath = path;
    StdStringReplaceSelf(normalizedPath, system::PathSeparator, '\\');

    ShellExecuteA(nullptr, "open", normalizedPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);   //ShellExecuteA uses normal strings, ShellExecuteW uses wide strings (which needs a L prefix : L"...")
#endif
}

void OpenWebBrowser(const std::string& _strURL)
{
#if defined(GUGU_OS_WINDOWS)
    ShellExecuteA(nullptr, "open", _strURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);   //ShellExecuteA uses normal strings, ShellExecuteW uses wide strings (which needs a L prefix : L"...")
#endif
}

void GetFiles(const std::string& rootPath_utf8, std::vector<FileInfo>& files, bool recursive)
{
    std::string normalizedPath = NormalizePath(rootPath_utf8);
    if (normalizedPath.empty())
    {
        normalizedPath = ".";
    }

    fs::path rootPath = fs::u8path(normalizedPath);
    if (!fs::is_directory(rootPath))
        return;

    if (recursive)
    {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath))
        {
            if (entry.is_regular_file())
            {
                FileInfo fileInfo = FileInfo::FromPath(entry.path());
                if (fileInfo.IsValid())
                {
                    files.push_back(fileInfo);
                }
            }
        }
    }
    else
    {
        for (const auto& entry : fs::directory_iterator(rootPath))
        {
            if (entry.is_regular_file())
            {
                FileInfo fileInfo = FileInfo::FromPath(entry.path());
                if (fileInfo.IsValid())
                {
                    files.push_back(fileInfo);
                }
            }
        }
    }
}

void GetDirectories(const std::string& rootPath_utf8, std::vector<std::string>& directories, bool recursive)
{
    std::string normalizedPath = NormalizePath(rootPath_utf8);
    if (normalizedPath.empty())
    {
        normalizedPath = ".";
    }

    fs::path rootPath = fs::u8path(normalizedPath);
    if (!fs::is_directory(rootPath))
        return;

    if (recursive)
    {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath))
        {
            if (entry.is_directory())
            {
                directories.push_back(NormalizePath(entry.path().generic_u8string()));
            }
        }
    }
    else
    {
        for (const auto& entry : fs::directory_iterator(rootPath))
        {
            if (entry.is_directory())
            {
                directories.push_back(NormalizePath(entry.path().generic_u8string()));
            }
        }
    }
}

bool DirectoryExists(std::string_view path_utf8)
{
    return fs::is_directory(fs::u8path(path_utf8));
}

bool FileExists(std::string_view path_utf8)
{
    return fs::is_regular_file(fs::u8path(path_utf8));
}

bool EnsureDirectoryExists(std::string_view path_utf8)
{
    if (path_utf8.empty())
        return true;

#if defined(GUGU_OS_WINDOWS)

    std::vector<std::string> vecDirectories;
    StdStringSplit(path_utf8, system::PathSeparator, vecDirectories);

    std::string strCombinedPath = "";
    for (auto strSubDirectory : vecDirectories)
    {
        strCombinedPath += strSubDirectory;

        if (!(CreateDirectoryA(strCombinedPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
        {
            return false;
        }

        strCombinedPath.push_back(system::PathSeparator);
    }

    return true;

#else

    return false;

#endif
}

bool RemoveFile(std::string_view path_utf8)
{
    fs::path convertedPath = fs::u8path(path_utf8);
    if (fs::is_regular_file(convertedPath))
    {
        std::error_code errorCode;
        return fs::remove(convertedPath, errorCode);
    }

    return false;
}

bool RemoveTargetDirectory(std::string_view path_utf8)
{
    fs::path convertedPath = fs::u8path(path_utf8);
    if (fs::is_directory(convertedPath))
    {
        std::error_code errorCode;
        return fs::remove(convertedPath, errorCode);
    }

    return false;
}

bool RemoveDirectoryTree(std::string_view path_utf8)
{
    fs::path convertedPath = fs::u8path(path_utf8);
    if (fs::is_directory(convertedPath))
    {
        std::error_code errorCode;
        return fs::remove_all(convertedPath, errorCode) > 0;
    }

    return false;
}

}   // namespace gugu
