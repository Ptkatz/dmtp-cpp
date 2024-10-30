#include "stdafx.h"
#include <iostream>
#include "../HPSocket/Include/HPSocket/HPSocket.h"
#include "../Dmtp/ISerializeObject.h"
#include "../Dmtp/FastBinaryFormatter.h"
#include "../Dmtp/DmtpRpcPackage.h"
#include "../Dmtp/DmtpMessage.h"
#include "../Dmtp/WaitVerify.h"
#include "../Dmtp/DmtpActor.h"
#include "../HPSocket.Dmtp/TcpDmtpActor.h"

#pragma region Test Dmtp
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

int TestDmtpMessage() {
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
    return 0;
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

int TestWaitVerify() {
    WaitVerify waitVerify;
    waitVerify.Id = "0";
    waitVerify.Token = "Dmtp";
    auto jsonObj = json11::Json(waitVerify);
    std::string jsonString = jsonObj.dump();
    std::string err;
    auto json = json11::Json::parse(jsonString, err);
    if (err.empty()) {
        WaitVerify waitVerify2 = WaitVerify(json);
    }

    return 0;
}

int TestDmtpActor() {
    TcpDmtpAcotr dmtpClient;
    Metadata metadata;
    dmtpClient.Handshake("Dmtp", "hello", metadata);
    return 0;
}

#pragma endregion

#pragma region Test HPSocket
class CListenerImpl : public CTcpClientListener
{
public:
    EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
        printf("OnReceive\n");
        return EnHandleResult::HR_OK;
    }
    EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
    {
        printf("OnClose\n");
        return EnHandleResult::HR_OK;
    }

    EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID) override {
        printf("OnHandShake\n");
        return EnHandleResult::HR_OK;
    }

    EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override {
        printf("OnSend\n");
        return EnHandleResult::HR_OK;
    }
};

int TestHPSocketClient() {
    CListenerImpl s_listener;
    CTcpClientPtr s_pclient(&s_listener);
    bool status = s_pclient->Start((LPCTSTR)"127.0.0.1", 7789, false, nullptr);
    if (!status)
    {
        uint32_t err = s_pclient->GetLastError();
    }
    else
    {
        uint16_t protocol = htons(1);


        while (true)
        {
            Sleep(1000);
            //break;
        }
    }

    return 0;
}

#pragma endregion


int main(int argc, char* const argv[])
{
    return 0;
}