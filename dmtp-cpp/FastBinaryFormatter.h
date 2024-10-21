#pragma once
#ifndef BYTEBLOCK_H
#define BYTEBLOCK_H
#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <type_traits>
#include <cstdint>
#include <chrono>
#include <cstring>
#include "ByteBlock.h"

#endif
template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T>;

static class FastBinaryFormatter {
private:
    template <typename T>
    static int SerializeObject(ByteBlock& byteBlock, const T& graph) {
        int len = 0;
        std::vector<uint8_t> data;
        size_t startPosition = byteBlock.Position();
        size_t endPosition;

        // 如果graph不是空，开始处理
        if constexpr (!std::is_null_pointer_v<T>) {
            if constexpr (std::is_same_v<T, uint8_t>) {  // byte
                byteBlock.Write(static_cast<uint8_t>(1));
                byteBlock.Write(graph);
                return 1 + 4;
            }
            else if constexpr (std::is_same_v<T, int8_t>) {  // sbyte
                byteBlock.Write(static_cast<uint8_t>(2));
                byteBlock.Write(graph);
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, bool>) {  // bool
                byteBlock.Write(static_cast<uint8_t>(1));
                byteBlock.Write(static_cast<uint8_t>(graph ? 1 : 0));
                return 1 + 4;
            }
            else if constexpr (std::is_same_v<T, int16_t>) {  // short
                byteBlock.Write(static_cast<uint8_t>(2));
                byteBlock.Write(graph);
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, uint16_t>) {  // ushort
                byteBlock.Write(static_cast<uint8_t>(2));
                byteBlock.Write(graph);
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, int32_t>) {  // int
                byteBlock.Write(static_cast<uint8_t>(4));
                byteBlock.Write(graph);
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, uint32_t>) {  // uint
                byteBlock.Write(static_cast<uint8_t>(4));
                byteBlock.Write(graph);
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, int64_t>) {  // long
                byteBlock.Write(static_cast<uint8_t>(8));
                byteBlock.Write(graph);
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {  // ulong
                byteBlock.Write(static_cast<uint8_t>(8));
                byteBlock.Write(graph);
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, float>) {  // float
                byteBlock.Write(static_cast<uint8_t>(4));
                byteBlock.Write(graph);
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, double>) {  // double
                byteBlock.Write(static_cast<uint8_t>(8));
                byteBlock.Write(graph);
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, char>) {  // char
                byteBlock.Write(static_cast<uint8_t>(2));
                byteBlock.Write(static_cast<uint16_t>(graph));
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {  // DateTime
                byteBlock.Write(static_cast<uint8_t>(8));
                auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(graph.time_since_epoch()).count();
                byteBlock.Write(static_cast<int64_t>(ticks));
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {  // byte[]
                data = graph;
                len = static_cast<int>(data.size());
                endPosition = startPosition + len + 4;
            }
            else if constexpr (is_enum_v<T>) {  // 枚举类型
                using enum_underlying_t = typename std::underlying_type<T>::type;
                SerializeObject(byteBlock, static_cast<enum_underlying_t>(graph));
            }
            else {
                // 处理复杂类型
                byteBlock.Position(startPosition + 4);
                // 自定义复杂对象序列化的逻辑
                // 根据需要实现复杂对象的序列化处理
                if constexpr (std::is_same_v<T, std::string>) { // string
                    int pos = byteBlock.Pos();
                    byteBlock.Write(static_cast<std::string>(graph));
                    len += byteBlock.Pos() - pos;
                }
                // len += SerializeClass(byteBlock, graph, serializerContext);
                endPosition = byteBlock.Position();
            }

            if (!data.empty()) {
                endPosition = startPosition + len + 4;
            }
        }
        else {
            endPosition = startPosition + 4;
        }

        // 写入长度信息
        byteBlock.Position(startPosition);
        byteBlock.Write(len);

        // 如果data有内容，则写入data
        if (!data.empty()) {
            byteBlock.Write(data.data());
        }

        byteBlock.Position(endPosition);
        return len + 4;
    }

    //template <typename T>
    //static T Deserialize(std::vector<uint8_t> datas, int offset) {

    //}

public:

    // 序列化对象
    template <typename T>
    static std::vector<uint8_t> Serialize(const T& graph) {
        ByteBlock byteBlock;
        byteBlock.Position(1);
        SerializeObject(byteBlock, graph);
        byteBlock.SetBuffer(0, 1);
        byteBlock.SetLen(byteBlock.Position());
        return byteBlock.Buffer();
    }
};