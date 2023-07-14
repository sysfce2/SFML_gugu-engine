#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include <filesystem>

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
class FileInfo
{
public:

    FileInfo();

    static FileInfo FromPath(const std::filesystem::path& path);
    static FileInfo FromString_utf8(const std::string& path_utf8);

    bool IsValid() const;

    // System path.
    const std::string& GetFileSystemPath() const;

    // Utf8 accessors.
    std::string_view GetFilePath_utf8() const;
    std::string_view GetDirectoryPath_utf8() const;
    std::string_view GetFileName_utf8() const;

    std::string_view GetPrettyName() const;
    std::string_view GetExtension() const;
    std::string_view GetAllExtensions() const;
    bool HasExtension(const std::string& extension) const;

    // Comparators (Used by std sorts).
    bool operator < (const FileInfo& other) const;
    bool operator == (const FileInfo& other) const;
    bool operator != (const FileInfo& other) const;

private:

    FileInfo(const std::filesystem::path& path);

private:

    std::string m_systemPath;       // Actual file system path.
    std::string m_filePath_utf8;    // Ut8 version of file path.
    size_t m_indexSeparator;        // Index of the last separator, between file directory and file name.
};

}   // namespace gugu
