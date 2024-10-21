#include "ByteBlock.h"
#include "DmtpMessage.h"
#include "Metadata.h"
#include "FastBinaryFormatter.h"
#include <iostream>

class MyObject {
public:
    int id;
    std::string name;

    MyObject() : id(0), name("") {}
    MyObject(int id, std::string name) : id(id), name(name) {}

    // 从json11::Json构造MyObject
    MyObject(const json11::Json& json) {
        id = json["id"].int_value();
        name = json["name"].string_value();
    }

    // 将MyObject序列化为json11::Json
    operator json11::Json() const {
        return json11::Json::object{
            {"id", id},
            {"name", name}
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

    // 输出解包后的数据
    std::cout << "Key1: " << unpackedMetadata["Key1"] << std::endl;
    std::cout << "Key2: " << unpackedMetadata["Key2"] << std::endl;
}

int TestFastSerialize() {
    auto bytes = FastBinaryFormatter::Serialize<std::string>("test");
    return 0;
}

int main()
{
    TestFastSerialize();
}
