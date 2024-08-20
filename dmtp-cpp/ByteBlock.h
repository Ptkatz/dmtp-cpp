#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include "json11.hpp"

class ByteBlock {
private:
    int m_dis = 1;
    int64_t m_length = 0;
    int64_t m_position = 0;
    std::vector<uint8_t> m_buffer;

public:
    // 构造函数
    ByteBlock(int byteSize = 1024 * 64) : m_buffer(byteSize) {}

    ByteBlock(const std::vector<uint8_t>& bytes, int length) : m_buffer(bytes), m_length(length) {}

    ByteBlock(const std::vector<uint8_t>& bytes) : ByteBlock(bytes, bytes.size()) {}

    // 访问器
    const std::vector<uint8_t>& Buffer() const {
        return m_buffer;
    }

    int CanReadLen() const {
        return Len() - Pos();
    }

    int Len() const {
        return static_cast<int>(m_length);
    }

    int Pos() const {
        return static_cast<int>(m_position);
    }

    void Pos(int pos) {
        m_position = pos;
    }

    int64_t Position() const {
        return m_position;
    }

    void Position(int64_t position) {
        m_position = position;
    }

    // 读取方法
    int Read(std::vector<uint8_t>& buffer, int offset, int length) {
        int len = std::min(static_cast<int>(m_length - m_position), length);
        std::copy(m_buffer.begin() + m_position, m_buffer.begin() + m_position + len, buffer.begin() + offset);
        m_position += len;
        return len;
    }

    int Read(std::vector<uint8_t>& buffer) {
        return Read(buffer, 0, buffer.size());
    }

    int ReadByte() {
        int value = m_buffer[m_position];
        m_position++;
        return value;
    }

    // 重置方法
    void Reset() {
        m_position = 0;
        m_length = 0;
    }

    // 设置容量方法
    void SetCapacity(int size, bool retainedData = false) {
        std::vector<uint8_t> bytes(size);
        if (retainedData) {
            std::copy(m_buffer.begin(), m_buffer.end(), bytes.begin());
        }
        m_buffer = std::move(bytes);
    }

    // 写入方法
    void Write(const std::vector<uint8_t>& buffer, int offset, int count) {
        if (count == 0) {
            return;
        }
        if (m_position >= INT_MAX) {
            throw std::out_of_range("Position cannot be greater than INT_MAX.");
        }
        if (m_buffer.size() - m_position < count) {
            int need = m_buffer.size() + count - (m_buffer.size() - m_position);
            int64_t lend = m_buffer.size();
            while (need > lend) {
                lend *= 2;
            }
            if (lend > INT_MAX) {
                lend = std::min(static_cast<int64_t>(need + 1024 * 1024 * 100), static_cast<int64_t>(INT_MAX));
            }
            SetCapacity(static_cast<int>(lend), true);
        }
        std::copy(buffer.begin() + offset, buffer.begin() + offset + count, m_buffer.begin() + m_position);
        m_position += count;
        m_length = std::max(m_position, m_length);
    }

    void Write(const ByteBlock& byteBlock) {
        Write(byteBlock.m_buffer, 0, byteBlock.Len());
    }

    void Write(const std::vector<uint8_t>& buffer) {
        Write(buffer, 0, buffer.size());
    }

    // 读取整数方法
    int32_t ReadInt32() {
        int32_t value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(int32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取短整型方法
    int16_t ReadInt16() {
        int16_t value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(int16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // 读取长整型方法
    int64_t ReadInt64() {
        int64_t value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(int64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取字符方法
    char ReadChar() {
        char value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(char value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // 读取布尔值方法
    bool ReadBoolean() {
        bool value;
        std::memcpy(&value, &m_buffer[m_position], 1);
        m_position += 1;
        return value;
    }

    void Write(bool value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 1));
    }


    // 读取字符串方法
    std::string ReadString() {
        int len = ReadInt32();
        if (len < 0) {
            return "";
        }
        else {
            std::string str(m_buffer.begin() + m_position, m_buffer.begin() + m_position + len);
            m_position += len;
            return str;
        }
    }

    void Write(const std::string& value) {
        if (value.empty()) {
            Write(-1);
        }
        else {
            Write(static_cast<int32_t>(value.size()));
            Write(std::vector<uint8_t>(value.begin(), value.end()));
        }
    }


    // 读取双精度浮点数方法
    double ReadDouble() {
        double value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(double value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取单精度浮点数方法
    float ReadFloat() {
        float value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(float value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取无符号短整型方法
    uint16_t ReadUInt16() {
        uint16_t value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(uint16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // 读取无符号整数方法
    uint32_t ReadUInt32() {
        uint32_t value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(uint32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取无符号长整型方法
    uint64_t ReadUInt64() {
        uint64_t value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(uint64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取对象方法
    template <typename T>
    T ReadObject() {
        int32_t length = ReadInt32();  // 读取对象的长度

        if (length == 0) {
            return T();  // 如果长度为0，返回类型的默认值
        }

        std::string jsonString(m_buffer.begin() + Pos(), m_buffer.begin() + Pos() + length);  // 提取JSON字符串
        std::string err;
        auto json = json11::Json::parse(jsonString, err);  // 解析JSON字符串

        if (!err.empty()) {
            throw std::runtime_error("JSON解析错误: " + err);
        }

        T obj = json;  // 将json对象转换为T类型的对象，需要确保T类型有从json11::Json类型构造的构造函数
        m_position += length;  // 更新读取位置
        return obj;
    }

    // 写入对象方法
    void WriteObject(const json11::Json& value) {
        if (value.is_null()) {
            Write((int32_t)0);  // 如果对象为空，写入0
        }
        else {
            std::string jsonString = value.dump();  // 序列化为JSON字符串
            std::vector<uint8_t> data(jsonString.begin(), jsonString.end());  // 转换为字节数组
            Write(static_cast<int32_t>(data.size()));  // 写入数据长度
            Write(data);  // 写入数据
        }
    }
    
};