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
    // ���캯��
    ByteBlock(int byteSize = 1024 * 64) : m_buffer(byteSize) {}

    ByteBlock(const std::vector<uint8_t>& bytes, int length) : m_buffer(bytes), m_length(length) {}

    ByteBlock(const std::vector<uint8_t>& bytes) : ByteBlock(bytes, bytes.size()) {}

    // ������
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

    // ��ȡ����
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

    // ���÷���
    void Reset() {
        m_position = 0;
        m_length = 0;
    }

    // ������������
    void SetCapacity(int size, bool retainedData = false) {
        std::vector<uint8_t> bytes(size);
        if (retainedData) {
            std::copy(m_buffer.begin(), m_buffer.end(), bytes.begin());
        }
        m_buffer = std::move(bytes);
    }

    // д�뷽��
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

    // ��ȡ��������
    int32_t ReadInt32() {
        int32_t value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(int32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // ��ȡ�����ͷ���
    int16_t ReadInt16() {
        int16_t value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(int16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // ��ȡ�����ͷ���
    int64_t ReadInt64() {
        int64_t value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(int64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // ��ȡ�ַ�����
    char ReadChar() {
        char value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(char value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // ��ȡ����ֵ����
    bool ReadBoolean() {
        bool value;
        std::memcpy(&value, &m_buffer[m_position], 1);
        m_position += 1;
        return value;
    }

    void Write(bool value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 1));
    }


    // ��ȡ�ַ�������
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


    // ��ȡ˫���ȸ���������
    double ReadDouble() {
        double value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(double value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // ��ȡ�����ȸ���������
    float ReadFloat() {
        float value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(float value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // ��ȡ�޷��Ŷ����ͷ���
    uint16_t ReadUInt16() {
        uint16_t value;
        std::memcpy(&value, &m_buffer[m_position], 2);
        m_position += 2;
        return value;
    }

    void Write(uint16_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 2));
    }

    // ��ȡ�޷�����������
    uint32_t ReadUInt32() {
        uint32_t value;
        std::memcpy(&value, &m_buffer[m_position], 4);
        m_position += 4;
        return value;
    }

    void Write(uint32_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 4));
    }

    // ��ȡ�޷��ų����ͷ���
    uint64_t ReadUInt64() {
        uint64_t value;
        std::memcpy(&value, &m_buffer[m_position], 8);
        m_position += 8;
        return value;
    }

    void Write(uint64_t value) {
        Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + 8));
    }

    // ��ȡ���󷽷�
    template <typename T>
    T ReadObject() {
        int32_t length = ReadInt32();  // ��ȡ����ĳ���

        if (length == 0) {
            return T();  // �������Ϊ0���������͵�Ĭ��ֵ
        }

        std::string jsonString(m_buffer.begin() + Pos(), m_buffer.begin() + Pos() + length);  // ��ȡJSON�ַ���
        std::string err;
        auto json = json11::Json::parse(jsonString, err);  // ����JSON�ַ���

        if (!err.empty()) {
            throw std::runtime_error("JSON��������: " + err);
        }

        T obj = json;  // ��json����ת��ΪT���͵Ķ�����Ҫȷ��T�����д�json11::Json���͹���Ĺ��캯��
        m_position += length;  // ���¶�ȡλ��
        return obj;
    }

    // д����󷽷�
    void WriteObject(const json11::Json& value) {
        if (value.is_null()) {
            Write((int32_t)0);  // �������Ϊ�գ�д��0
        }
        else {
            std::string jsonString = value.dump();  // ���л�ΪJSON�ַ���
            std::vector<uint8_t> data(jsonString.begin(), jsonString.end());  // ת��Ϊ�ֽ�����
            Write(static_cast<int32_t>(data.size()));  // д�����ݳ���
            Write(data);  // д������
        }
    }
    
};