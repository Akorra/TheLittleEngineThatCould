#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Assert.h"
#include <vector>
#include <string>

namespace TLETC
{

class BinaryWriter
{
public:
    void Write(const void* data, size_t size)
    {
        const uint8* bytes = static_cast<const uint8*>(data);
        buffer_.insert(buffer_.end(), bytes, bytes + size);
    }

    template<typename T>
    void Write(const T& value)
    {
        Write(&value, sizeof(T));
    }

    void WriteString(const std::string& str)
    {
        uint32 length = static_cast<uint32>(str.size());
        Write(length);
        Write(str.data(), str.size());
    }

    const std::vector<uint8>& GetBuffer() const { return buffer_; }
    size_t GetSize() const { return buffer_.size(); }

private:
    std::vector<uint8> buffer_;
};

class BinaryReader
{
public:
    BinaryReader(const std::vector<uint8>& data) : buffer_(data), offset_(0) {}

    void Read(void* dest, size_t size)
    {
        TLETC_ASSERT(offset_ + size <= buffer_.size(), "Read overflow!");
        std::memcpy(dest, buffer_.data() + offset_, size);
        offset_ += size;
    }

    template<typename T>
    T Read()
    {
        T value;
        Read(&value, sizeof(T));
        return value;
    }

    std::string ReadString()
    {
        uint32 length = Read<uint32>();
        std::string str(length, '\0');
        Read(str.data(), length);
        return str;
    }

private:
    const std::vector<uint8>& buffer_;
    size_t offset_;
};

} // namespace TLETC