#include <gmock/gmock.h>
#include "IFileReader.h"

class MockFileReader : public IFileReader {
public:
    MOCK_METHOD(bool, exists, (const std::string& path), (const, override));
    MOCK_METHOD(std::string, read, (const std::string& path), (override));
};