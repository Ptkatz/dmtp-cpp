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

template <class T>
struct is_vector {
    using type = T;
    constexpr static bool value = false;
};

template <class T>
struct is_vector<std::vector<T>> {
    using type = std::vector<T>;
    constexpr  static bool value = true;
};

template <typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
using is_vector_t = typename is_vector<T>::type;


class FastBinaryFormatter {
private:
    template <typename T>
    static int SerializeClass(ByteBlock& byteBlock, const T& obj) {
        int len = 0;
        if constexpr (std::is_base_of_v<SerializeObjectBase, T>) {
            const SerializeObjectBase& _Obj = obj;  // 通过引用来处理派生类对象
            auto jsonObj = _Obj.operator json11::Json();  // 调用多态的转换操作符
            json11::Json::object map = jsonObj.object_items();
            for (const auto& pair : map) {
                std::vector<uint8_t> propertyBytes = std::vector<uint8_t>(pair.first.begin(), pair.first.end());
                char propertyLen = static_cast<char>(propertyBytes.size());
                byteBlock.Write(propertyLen);
                byteBlock.Write(propertyBytes);
                len += propertyBytes.size() + 1;
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
    static int SerializeVector(ByteBlock& byteBlock, const std::vector<T>& param) {
        int len = 0;
        int64_t oldPosition = byteBlock.Position();
        byteBlock.Position(oldPosition + 4);
        len += 4;
        uint32_t paramLen = 0;
        for (const auto& item : param)
        {
            paramLen++;
            len += SerializeObject(byteBlock, item);
        }
        int64_t newPosition = byteBlock.Position();
        byteBlock.Position(oldPosition);
        byteBlock.Write(paramLen);
        byteBlock.Position(newPosition);
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
                byteBlock.Write(static_cast<int32_t>(1));
                byteBlock.Write(static_cast<uint8_t>(graph));
                return 1 + 4;
            }
            else if constexpr (std::is_same_v<T, int8_t>) {  // sbyte
                byteBlock.Write(static_cast<int32_t>(2));
                byteBlock.Write(static_cast<int8_t>(graph));
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, bool>) {  // bool
                byteBlock.Write(static_cast<int32_t>(1));
                byteBlock.Write(static_cast<uint8_t>(graph ? 1 : 0));
                return 1 + 4;
            }
            else if constexpr (std::is_same_v<T, int16_t>) {  // short
                byteBlock.Write(static_cast<int32_t>(2));
                byteBlock.Write(static_cast<int16_t>(2));
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, uint16_t>) {  // ushort
                byteBlock.Write(static_cast<int32_t>(2));
                byteBlock.Write(static_cast<uint16_t>(graph));
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, int32_t>) {  // int
                byteBlock.Write(static_cast<int32_t>(4));
                byteBlock.Write(static_cast<int32_t>(graph));
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, uint32_t>) {  // uint
                byteBlock.Write(static_cast<int32_t>(4));
                byteBlock.Write(static_cast<uint32_t>(graph));
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, int64_t>) {  // long
                byteBlock.Write(static_cast<int32_t>(8));
                byteBlock.Write(static_cast<int64_t>(graph));
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {  // ulong
                byteBlock.Write(static_cast<int32_t>(8));
                byteBlock.Write(static_cast<uint64_t>(graph));
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, float>) {  // float
                byteBlock.Write(static_cast<int32_t>(4));
                byteBlock.Write(static_cast<float>(graph));
                return 4 + 4;
            }
            else if constexpr (std::is_same_v<T, double>) {  // double
                byteBlock.Write(static_cast<int32_t>(8));
                byteBlock.Write(static_cast<double>(graph));
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, char>) {  // char
                byteBlock.Write(static_cast<int32_t>(2));
                byteBlock.Write(static_cast<char>(graph));
                return 2 + 4;
            }
            else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {  // DateTime
                byteBlock.Write(static_cast<int32_t>(8));
                auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(graph.time_since_epoch()).count();
                byteBlock.Write(static_cast<int64_t>(ticks));
                return 8 + 4;
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {  // byte[]
                data = graph;
                len = static_cast<int>(data.size());
                endPosition = startPosition + len + 4;
            }
            else if constexpr (std::is_enum_v<T>) {  // 枚举类型
                using enum_underlying_t = typename std::underlying_type<T>::type;
                SerializeObject(byteBlock, static_cast<enum_underlying_t>(graph));
            }
            else 
            {
                // 处理复杂类型
                byteBlock.Position(startPosition + 4);
                if constexpr (std::is_same_v<T, std::string>) { // string
                    int pos = byteBlock.Pos();
                    byteBlock.Write(static_cast<std::string>(graph));
                    len += byteBlock.Pos() - pos;
                }
                else if constexpr (is_vector_v<T>) {
                    len += SerializeVector(byteBlock, graph);
                }
                else if constexpr (std::is_class_v<T>) {
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

    template <typename T, typename V = void>
    static T* DeserializeClass(std::vector<uint8_t> datas, int offset, int length) {
        T* instance = new T();

        if constexpr (std::is_base_of_v<SerializeObjectBase, T>) {
            const SerializeObjectBase* obj = instance;
            auto jsonObj = obj->operator json11::Json();
            json11::Json::object map = jsonObj.object_items();

            int index = offset;
            while (offset - index < length && (length >= 4))
            {
                int len = datas[offset];
                std::string propertyName = std::string(datas.begin() + offset + 1, datas.begin() + offset + 1 + len);
                offset += len + 1;
                auto property = map.at(propertyName);
                json11::Json jsonValue;
                if (property.is_number())
                {
                    auto objValue = *DeserializeObject<int>(datas, offset);
                    jsonValue = json11::Json(objValue);
                }
                else if (property.is_string())
                {
                    auto objValue = *DeserializeObject<std::string>(datas, offset);
                    jsonValue = json11::Json(objValue);
                }
                else if (property.is_bool())
                {
                    auto objValue = *DeserializeObject<bool>(datas, offset);
                    jsonValue = json11::Json(objValue);
                }
                map[propertyName] = jsonValue;
            }
            jsonObj = json11::Json(map);
            instance = new T(jsonObj);
        }

        return instance;
    }

    template <typename V>
    static std::vector<V>* DeserializeVector(std::vector<uint8_t> datas, int offset, int length) {
        std::vector<V>* instance = new std::vector<V>();
        uint32_t paramLen = TouchSocketBitConverter::ToUInt32(datas, offset);
        offset += 4;
        for (uint32_t i = 0; i < paramLen; i++)
        {
            V* obj = DeserializeObject<V>(datas, offset);
            instance->push_back(*obj);
        }
        return instance;
    }

    template <typename T, typename V = void>
    static T* DeserializeObject(std::vector<uint8_t> datas, int& offset) {
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
                    else if constexpr (is_vector_v<T>) {
                        obj = DeserializeVector<V>(datas, offset, len);
                    }
                    else if constexpr (std::is_class_v<T>) {
                        obj = DeserializeClass<T>(datas, offset, len);
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
    template <typename T, typename V = void>
    static T Deserialize(std::vector<uint8_t> datas) {
        int offset = 0;
        if (datas[offset] != 1)
        {
            throw std::out_of_range("Deserialization data stream parsing error.");
        }
        offset += 1;
        return *DeserializeObject<T, V>(datas, offset);
    }

};