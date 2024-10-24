﻿#include "ByteBlock.h"
#include "DmtpMessage.h"
#include "FastBinaryFormatter.h"
#include "ISerializeObject.h"
#include "DmtpRpcPackage.h"
#include <iostream>

class MyObject : public SerializeObjectBase {
public:
    int id;
    std::string name;
    bool isok;

    MyObject() : id(0), name(""), isok(false) {}
    MyObject(int id, std::string name) : id(id), name(name), isok(false) {}
    MyObject(int id, std::string name, bool isok) : id(id), name(name), isok(isok) {}

    // 从json11::Json构造MyObject
    MyObject(const json11::Json& json) {
        id = json["id"].int_value();
        name = json["name"].string_value();
        isok = json["isok"].bool_value();
    }

    // 将MyObject序列化为json11::Json
   operator json11::Json() const override {
        return json11::Json::object{
            {"id", id},
            {"name", name},
            {"isok", isok}
        };
    }
};

void TestDmtpMessage() {
    ByteBlock byteBlock;
    MyObject obj(1, "example");
    byteBlock.Write(10);
    byteBlock.Write((const std::string&)"Hello");
    byteBlock.WriteObject(static_cast<json11::Json>(obj), SerializationType::Json);
    DmtpMessage dmtpMessage(1000);
    dmtpMessage.BodyByteBlock = &byteBlock;
    ByteBlock dmtpBlock;
    dmtpMessage.Build(dmtpBlock);
    DmtpMessage dmtpMsg = DmtpMessage::CreateFrom(dmtpBlock.Buffer());
}

void TestMetadata() {
    Metadata metadata;
    ByteBlock byteBlock;
    metadata.Add("Key1", "Value1").Add("Key2", "Value2");

    // 打包数据
    metadata.Package(byteBlock);

    // 重置位置以进行解包
    byteBlock.Pos(0);

    // 解包数据
    Metadata unpackedMetadata;
    unpackedMetadata.Unpackage(byteBlock);

    //输出解包后的数据
    std::cout << "Key1: " << unpackedMetadata["Key1"] << std::endl;
    std::cout << "Key2: " << unpackedMetadata["Key2"] << std::endl;
}

int TestFastSerialize() {
    MyObject obj1(1, "TestA", true);
    MyObject obj2(2, "TestB", false);

    std::vector<MyObject> objs{ obj1, obj2 };
    auto bytesVector = FastBinaryFormatter::Serialize<std::vector<MyObject>>(objs);
    auto deVector = FastBinaryFormatter::Deserialize<std::vector<MyObject>, MyObject>(bytesVector);

    return 0;
}


int TestDmtpRpcPackage() {
    DmtpRpcPackage package;
    Metadata metadata;
    std::vector<std::vector<uint8_t>> params;
    auto bytesStr = FastBinaryFormatter::Serialize<std::string>("123");
    params.push_back(bytesStr);
    metadata.Add("Key1", "Value1").Add("Key2", "Value2");
    package._Metadata = metadata;
    package._SerializationType = SerializationType::FastBinary;
    package.Feedback = FeedbackType::WaitInvoke;
    package.ParametersBytes = params;
    package.MethodName = "Req";
    package.SetMessage("okok");
    ByteBlock byteBlock;
    package.Package(byteBlock);

    for (size_t i = 0; i < byteBlock.Len(); i++)
    {
        uint8_t b = byteBlock.Buffer()[i];

        printf("%d,", b);
    }
    byteBlock.Pos(0);
    DmtpRpcPackage unpackage;
    unpackage.Unpackage(byteBlock);


    return 0;
}

int main()
{
    TestDmtpRpcPackage();

    printf("");
}
