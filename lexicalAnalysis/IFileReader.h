#pragma once
#include <string>

class IFileReader {
public:
    virtual ~IFileReader() = default;
    virtual bool exists(const std::string& path) const = 0;

    virtual std::string read(const std::string& path) = 0;
};