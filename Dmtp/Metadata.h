#pragma once
#include <unordered_map>
#include <string>
#include "Common.h"
#include "ISerializeObject.h"

class Metadata : public IPackage, public SerializeObjectBase {
public:
    // ʹ��unordered_mapģ��Dictionary<string, string>
    std::unordered_map<std::string, std::string> data;

    //��ȡԪ�أ�����������򷵻ؿ��ַ���
    std::string operator[](const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return "";  // ���ؿ��ַ�������null
    }

    // ��ӻ򸲸�Ԫ��
    Metadata& Add(const std::string& name, const std::string& value) {
        data[name] = value;
        return *this;
    }

    // �������
    void Package(ByteBlock& byteBlock) override {
        byteBlock.Write(static_cast<int32_t>(data.size()));  // д���ֵ�Ե�����
        for (const auto& item : data) {
            byteBlock.Write(item.first);  // д���
            byteBlock.Write(item.second); // д��ֵ
        }
    }

    // �������
    void Unpackage(ByteBlock& byteBlock) override {
        int32_t count = byteBlock.ReadInt32();  // ��ȡ��ֵ�Ե�����
        for (int32_t i = 0; i < count; ++i) {
            std::string key = byteBlock.ReadString();  // ��ȡ��
            std::string value = byteBlock.ReadString();  // ��ȡֵ
            data[key] = value;  // ��ӵ�map��
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