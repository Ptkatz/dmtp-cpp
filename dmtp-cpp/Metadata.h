#pragma once
#include <unordered_map>
#include <string>
#include "ByteBlock.h"

class Metadata : public IPackage {
public:
    // 使用unordered_map模拟Dictionary<string, string>
    std::unordered_map<std::string, std::string> data;

    // 获取元素，如果不存在则返回空字符串
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
    void Package(ByteBlock& byteBlock) const override {
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
};