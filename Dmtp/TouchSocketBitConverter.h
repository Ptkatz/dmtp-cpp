#pragma once
#include <iostream>
#include <vector>

class TouchSocketBitConverter {
public:
    static int32_t ToInt32(const std::vector<uint8_t>& data, int offset) {
        int32_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static uint32_t ToUInt32(const std::vector<uint8_t>& data, int offset) {
        uint32_t  value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static int16_t ToInt16(const std::vector<uint8_t>& data, int offset) {
        int16_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static uint16_t ToUInt16(const std::vector<uint8_t>& data, int offset) {
        uint16_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static bool ToBoolean(const std::vector<uint8_t>& data, int offset) {
        return data[offset] != 0;
    }

    static float ToSingle(const std::vector<uint8_t>& data, int offset) {
        float value;
        std::memcpy(&value, data.data() + offset, 4);
        return value;
    }

    static double ToDouble(const std::vector<uint8_t>& data, int offset) {
        double value;
        std::memcpy(&value, data.data() + offset, 8);
        return value;
    }

    static char ToChar(const std::vector<uint8_t>& data, int offset) {
        char value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static int64_t ToInt64(const std::vector<uint8_t>& data, int offset) {
        int64_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static uint64_t ToUInt64(const std::vector<uint8_t>& data, int offset) {
        uint64_t value;
        std::memcpy(&value, data.data() + offset, sizeof(value));
        return value;
    }

    static std::vector<uint8_t> GetBigEndianBytes(int32_t value) {
        std::vector<uint8_t> bytes(sizeof(value));
        std::memcpy(bytes.data(), &value, sizeof(value));
        std::reverse(bytes.begin(), bytes.end());
        return bytes;
    }

    static std::vector<uint8_t> GetBigEndianBytes(uint16_t value) {
        std::vector<uint8_t> bytes(sizeof(value));
        std::memcpy(bytes.data(), &value, sizeof(value));
        std::reverse(bytes.begin(), bytes.end());
        return bytes;
    }
};
