#pragma once
#include <WinSock2.h>
#include <memory>
#include "ByteBlock.h"
#include "Common.h"
#pragma comment(lib, "Ws2_32.lib")

class DmtpMessage {
private:
    int m_bodyLength = 0;
    static const std::vector<uint8_t> Head;

public:
    ByteBlock* BodyByteBlock = nullptr;
    uint16_t ProtocolFlags = 0;

    int MaxLength() const {
        return BodyByteBlock == nullptr ? 6 : BodyByteBlock->Len() + 6;
    }

    // 构造函数
    DmtpMessage() = default;

    explicit DmtpMessage(uint16_t protocolFlags) : ProtocolFlags(protocolFlags) {}

    // 构建消息
    void Build(ByteBlock& byteBlock) const {
        byteBlock.Write(Head); // 写入头部
        uint16_t flag = htons(ProtocolFlags); // 将协议标志字节顺序转换为网络字节顺序
        byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&flag), reinterpret_cast<uint8_t*>(&flag) + 2), 0, sizeof(flag)); // 写入协议标志
        if (BodyByteBlock == nullptr) {
            int32_t bodyLen = 0;
            bodyLen = htonl(bodyLen); // 转换为网络字节顺序
            byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&bodyLen), reinterpret_cast<uint8_t*>(&bodyLen) + 4), 0, sizeof(bodyLen)); // 写入空体长
        }
        else {
            int32_t bodyLen = htonl(BodyByteBlock->Len()); // 将主体长度转换为网络字节顺序
            byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&bodyLen), reinterpret_cast<uint8_t*>(&bodyLen) + 4), 0, sizeof(bodyLen)); // 写入空体长
            byteBlock.Write(*BodyByteBlock); // 写入主体数据
        }
    }

    // 从字节段创建DmtpMessage
    static DmtpMessage CreateFrom(const std::vector<uint8_t>& buffer, int offset = 0) {
        if (buffer[offset++] != Head[0] || buffer[offset++] != Head[1]) {
            throw std::runtime_error("这可能不是Dmtp协议数据");
        }

        uint16_t protocolFlags;
        std::memcpy(&protocolFlags, &buffer[offset], sizeof(protocolFlags));
        protocolFlags = ntohs(protocolFlags); // 转换为主机字节顺序
        offset += sizeof(protocolFlags);

        int32_t bodyLength;
        std::memcpy(&bodyLength, &buffer[offset], sizeof(bodyLength));
        bodyLength = ntohl(bodyLength); // 转换为主机字节顺序
        offset += sizeof(bodyLength);

        DmtpMessage message(protocolFlags);
        if (bodyLength > 0) {
            message.BodyByteBlock = new ByteBlock(bodyLength);  // 动态分配内存
            message.BodyByteBlock->Write(buffer, offset, bodyLength); // 写入主体内容
        }

        return message;
    }
};

// DmtpMessage类的静态成员初始化
const std::vector<uint8_t> DmtpMessage::Head = { 100, 109 };