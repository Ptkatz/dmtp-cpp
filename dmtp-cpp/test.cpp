#include "ByteBlock.h"
#include "DmtpMessage.h"

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

int main()
{
    ByteBlock byteBlock;
    MyObject obj(1, "example");
    byteBlock.Write(10);
    byteBlock.Write((const std::string&)"Hello");
    byteBlock.WriteObject(static_cast<json11::Json>(obj));
    DmtpMessage dmtpMessage(1000);
    dmtpMessage.BodyByteBlock = &byteBlock;
    ByteBlock dmtpBlock;
    dmtpMessage.Build(dmtpBlock);
    DmtpMessage dmtpMsg = DmtpMessage::CreateFrom(dmtpBlock.Buffer());
}
