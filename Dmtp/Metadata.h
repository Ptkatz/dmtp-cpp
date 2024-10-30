#pragma once
#include <unordered_map>
#include <string>
#include "Common.h"
#include "ISerializeObject.h"

class Metadata : public IPackage, public SerializeObjectBase {
public:
    // 使用unordered_map模拟Dictionary<string, string>
    std::unordered_map<std::string, std::string> data;

    //获取元素，如果不存在则返回空字符串
    std::string operator[](const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return "";  // 返回空字符串代替null
    }

    // 添加或覆盖元素
    Metadata& Add(const std::string& name, const std::string& value) {
        data[name] = value;
        return *this;
    }

    // 打包方法
    void Package(ByteBlock& byteBlock) override {
        byteBlock.Write(static_cast<int32_t>(data.size()));  // 写入键值对的数量
        for (const auto& item : data) {
            byteBlock.Write(item.first);  // 写入键
            byteBlock.Write(item.second); // 写入值
        }
    }

    // 解包方法
    void Unpackage(ByteBlock& byteBlock) override {
        int32_t count = byteBlock.ReadInt32();  // 读取键值对的数量
        for (int32_t i = 0; i < count; ++i) {
            std::string key = byteBlock.ReadString();  // 读取键
            std::string value = byteBlock.ReadString();  // 读取值
            data[key] = value;  // 添加到map中
        }
    }

    Metadata() = default;

    Metadata(const json11::Json& json) {
        auto items = json.object_items();
        for (auto& pair : items) {
            data[pair.first] = pair.second.string_value();
        }
    }

    operator json11::Json() const override {
        json11::Json::object obj;
        for (auto &pair : data) {
            obj[pair.first] = pair.second;
        }
        return obj;
    }
};