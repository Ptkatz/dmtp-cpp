#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <type_traits>
#include <cstdint>
#include <chrono>
#include <cstring>
#include "ByteBlock.h"
#include "Common.h"

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T>;

class FastBinaryFormatter {
private:
    template <typename T>
    static int SerializeClass(ByteBlock& byteBlock, const T& obj) {
        int len = 0;
        if constexpr (std::is_base_of_v<ISerializeObject, T>) {
            const ISerializeObject& _Obj = obj;  // 通过引用来处理派生类对象
            auto jsonObj = _Obj.operator json11::Json();  // 调用多态的转换操作符
            json11::Json::object map = jsonObj.object_items();
            for (const auto& pair : map) {
                byteBlock.Write(pair.first);
                if (pair.second.is_string()) {
                    len += SerializeObject(byteBlock, pair.second.string_value());
                }
                else if (pair.second.is_number()) {
                    len += SerializeObject(byteBlock, pair.second.int_value());
                }
                else if (pair.second.is_bool()) {
                    len += SerializeObject(byteBlock, pair.second.bool_value());
                }
                else {
                    len += SerializeObject(byteBlock, pair.second.object_items());
                }
            }
        }
        return len;
    }

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
                if constexpr (std::is_same_v<T, std::string>) { // string
                    int pos = byteBlock.Pos();
                    byteBlock.Write(static_cast<std::string>(graph));
                    len += byteBlock.Pos() - pos;
                }
                else { // string
                    len += SerializeClass(byteBlock, graph);
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

    template <typename T>
    static T* DeserializeObject(std::vector<uint8_t> datas, int offset) {
        int len = TouchSocketBitConverter::ToInt32(datas, offset);
        offset += 4;
        T* obj = new T();

        if (len > 0)
        {
            if constexpr (!std::is_null_pointer_v<T>) {
                if constexpr (std::is_same_v<T, uint8_t>) {  // byte
                    obj = new uint8_t(datas[offset]);
                }
                else if constexpr (std::is_same_v<T, int8_t>) {  // sbyte
                    obj = new int8_t(TouchSocketBitConverter::ToInt16(datas, offset));
                }
                else if constexpr (std::is_same_v<T, bool>) {  // bool
                    obj = new bool(TouchSocketBitConverter::ToBoolean(datas, offset));
                }
                else if constexpr (std::is_same_v<T, int16_t>) {  // short
                    obj = new int16_t(TouchSocketBitConverter::ToInt16(datas, offset));
                }
                else if constexpr (std::is_same_v<T, uint16_t>) {  // ushort
                    obj = new uint16_t(TouchSocketBitConverter::ToInt16(datas, offset));
                }
                else if constexpr (std::is_same_v<T, int32_t>) {  // int
                    obj = new int32_t(TouchSocketBitConverter::ToInt32(datas, offset));
                }
                else if constexpr (std::is_same_v<T, uint32_t>) {  // uint
                    obj = new uint32_t(TouchSocketBitConverter::ToUInt32(datas, offset));
                }
                else if constexpr (std::is_same_v<T, int64_t>) {  // long
                    obj = new int64_t(TouchSocketBitConverter::ToInt64(datas, offset));
                }
                else if constexpr (std::is_same_v<T, uint64_t>) {  // ulong
                    obj = new uint64_t(TouchSocketBitConverter::ToUInt64(datas, offset));
                }
                else if constexpr (std::is_same_v<T, float>) {  // float
                    obj = new float(TouchSocketBitConverter::ToSingle(datas, offset));
                }
                else if constexpr (std::is_same_v<T, double>) {  // double
                    obj = new double(TouchSocketBitConverter::ToDouble(datas, offset));
                }
                else if constexpr (std::is_same_v<T, char>) {  // char
                    obj = new char(TouchSocketBitConverter::ToChar(datas, offset));
                }
                else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {  // DateTime
                    int64_t ms_since_epoch = new int64_t(TouchSocketBitConverter::ToInt64(datas, offset));
                    std::chrono::milliseconds duration_since_epoch(ms_since_epoch);
                    obj = new std::chrono::system_clock::time_point(duration_since_epoch);
                }
                else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {  // byte[]
                    std::vector<uint8_t> data(len);
                    std::copy(datas.begin() + offset, datas.begin() + offset + len, data.begin());
                    obj = data;
                }
                else {
                    if constexpr (std::is_same_v<T, std::string>) { // string
                        ByteBlock* byteBlock = new ByteBlock(datas);
                        byteBlock->Pos(offset);
                        obj = new std::string(byteBlock->ReadString());
                    }
                    else
                    {
                    }
                    // len += SerializeClass(byteBlock, graph, serializerContext);
                }
            }
        }
        offset += len;
        return obj;
    }

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

    // 反序列化对象
    template <typename T>
    static T Deserialize(std::vector<uint8_t> datas) {
        int offset = 0;
        if (datas[offset] != 1)
        {
            throw std::out_of_range("Deserialization data stream parsing error.");
        }
        offset += 1;
        return *DeserializeObject<T>(datas, offset);
    }

};