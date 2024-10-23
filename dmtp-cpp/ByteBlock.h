#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include "json11.hpp"
#include "SerializationType.h"
#include "TouchSocketBitConverter.h"
#include "Common.h"
#include "IPackage.h"

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

    ~ByteBlock() {
        m_length = 0;
        m_position = 0;
        m_dis = 0;
        m_buffer.clear();
    }

    // 访问器
    const std::vector<uint8_t>& Buffer() const {
        return m_buffer;
    }

    void SetBuffer(int pos, uint8_t value) {
        m_buffer[pos] = value;
    }

    int CanReadLen() const {
        return Len() - Pos();
    }

    int Len() const {
        return static_cast<int>(m_length);
    }

    void SetLen(int len) {
        m_length = len;
        m_buffer.resize(len);
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
        int32_t value = TouchSocketBitConverter::ToInt32(m_buffer, m_position);
        m_position += 4;
        return value;
    }

    void Write(int32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取短整型方法
    int16_t ReadInt16() {
        int16_t value = TouchSocketBitConverter::ToInt16(m_buffer, m_position);
        m_position += 2;
        return value;
    }

    void Write(int16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // 读取长整型方法
    int64_t ReadInt64() {
        int64_t value = TouchSocketBitConverter::ToInt64(m_buffer, m_position);
        m_position += 8;
        return value;
    }

    void Write(int64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取字符方法
    char ReadChar() {
        char value = TouchSocketBitConverter::ToChar(m_buffer, m_position);
        m_position += 2;
        return value;
    }


    void Write(uint8_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 1));
    }


    void Write(char value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 1));
    }

    // 读取布尔值方法
    bool ReadBoolean() {
        bool value = TouchSocketBitConverter::ToBoolean(m_buffer, m_position);
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
        double value = TouchSocketBitConverter::ToDouble(m_buffer, m_position);
        m_position += 8;
        return value;
    }

    void Write(double value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取单精度浮点数方法
    float ReadFloat() {
        float value = TouchSocketBitConverter::ToSingle(m_buffer, m_position);
        m_position += 4;
        return value;
    }

    void Write(float value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取无符号短整型方法
    uint16_t ReadUInt16() {
        uint16_t value = TouchSocketBitConverter::ToUInt16(m_buffer, m_position);
        m_position += 2;
        return value;
    }

    void Write(uint16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // 读取无符号整数方法
    uint32_t ReadUInt32() {
        uint32_t value = TouchSocketBitConverter::ToUInt32(m_buffer, m_position);
        m_position += 4;
        return value;
    }

    void Write(uint32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // 读取无符号长整型方法
    uint64_t ReadUInt64() {
        uint64_t value = TouchSocketBitConverter::ToUInt64(m_buffer, m_position);
        m_position += 8;
        return value;
    }

    void Write(uint64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // 读取对象方法
    template <typename T>
    T ReadObject(SerializationType serializationType = SerializationType::FastBinary) {
        int32_t length = ReadInt32();  // 读取对象的长度

        if (length == 0) {
            return T();  // 如果长度为0，返回类型的默认值
        }

        switch (serializationType)
        {
        case SerializationType::FastBinary:
            break;
        case SerializationType::Json: {
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
        case SerializationType::Xml:
            break;
        case SerializationType::SystemBinary:
            break;
        default:
            break;
        }
        return nullptr;
    }

    // 写入对象方法
    template <typename T>
    void WriteObject(const T& value, SerializationType serializationType = SerializationType::FastBinary) {
        if (value == nullptr) {
            Write((int32_t)0);  // 如果对象为空，写入0
        }
        else {
            std::vector<uint8_t> data{};
            switch (serializationType)
            {
            case SerializationType::FastBinary: {
                data = FastBinaryFormatter::Serialize(value);
                break;
            }
            case SerializationType::Json: {
                    const json11::Json& jsonValue = static_cast<json11::Json>(value);
                    if (jsonValue.is_null())
                    {
                        Write((int32_t)0);  // 如果对象为空，写入0
                        return;
                    }
                    std::string jsonString = jsonValue.dump();  // 序列化为JSON字符串
                    data = std::vector<uint8_t>(jsonString.begin(), jsonString.end());  // 转换为字节数组
                    break;
            }
            case SerializationType::Xml:
                break;
            case SerializationType::SystemBinary:
                break;
            default:
                break;
            }
            Write(static_cast<int32_t>(data.size()));  // 写入数据长度
            Write(data);  // 写入数据
        }
    }

    // 从当前流位置读取一个标识值，判断是否为null
    bool ReadIsNull() {
        uint8_t status = ReadByte();
        if (status == 0) {
            return true;
        }
        else if (status == 1) {
            return false;
        }
        else {
            throw std::runtime_error("标识既非Null，也非NotNull，可能是流位置发生了错误。");
        }
    }

    // 判断该值是否为Null，然后写入标识值 (适用于类类型)
    template <typename T>
    typename std::enable_if<std::is_class<T>::value>::type WriteIsNull(T* t) {
        if (t == nullptr) {
            WriteNull();
        }
        else {
            WriteNotNull();
        }
    }

    // 判断该值是否为Null，然后写入标识值 (适用于可空类型)
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value>::type WriteIsNull(T* t) {
        if (t == nullptr) {
            WriteNull();
        }
        else {
            WriteNotNull();
        }
    }

    // 写入一个标识非Null值
    void WriteNotNull() {
        Write(static_cast<uint8_t>(1));
    }

    // 写入一个标识Null值
    void WriteNull() {
        Write(static_cast<uint8_t>(0));
    }

    // 读取一个指定类型的包
    template <typename TPackage>
    std::unique_ptr<TPackage> ReadPackage() {
        static_assert(std::is_base_of<IPackage, TPackage>::value, "TPackage must implement IPackage");

        if (ReadIsNull()) {
            return nullptr;
        }
        else {
            auto package = std::make_unique<TPackage>();
            package->Unpackage(*this);
            return package;
        }
    }

    // 以包进行写入。允许null值。
    template <typename TPackage>
    void WritePackage(TPackage* package) {
        static_assert(std::is_base_of<IPackage, TPackage>::value, "TPackage must implement IPackage");

        WriteIsNull(package);
        if (package != nullptr) {
            package->Package(*this);
        }
    }
};