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

    // ���캯��
    DmtpMessage() = default;

    explicit DmtpMessage(uint16_t protocolFlags) : ProtocolFlags(protocolFlags) {}

    // ������Ϣ
    void Build(ByteBlock& byteBlock) const {
        byteBlock.Write(Head); // д��ͷ��
        uint16_t flag = htons(ProtocolFlags); // ��Э���־�ֽ�˳��ת��Ϊ�����ֽ�˳��
        byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&flag), reinterpret_cast<uint8_t*>(&flag) + 2), 0, sizeof(flag)); // д��Э���־
        if (BodyByteBlock == nullptr) {
            int32_t bodyLen = 0;
            bodyLen = htonl(bodyLen); // ת��Ϊ�����ֽ�˳��
            byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&bodyLen), reinterpret_cast<uint8_t*>(&bodyLen) + 4), 0, sizeof(bodyLen)); // д����峤
        }
        else {
            int32_t bodyLen = htonl(BodyByteBlock->Len()); // �����峤��ת��Ϊ�����ֽ�˳��
            byteBlock.Write(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&bodyLen), reinterpret_cast<uint8_t*>(&bodyLen) + 4), 0, sizeof(bodyLen)); // д����峤
            byteBlock.Write(*BodyByteBlock); // д����������
        }
    }

    // ���ֽڶδ���DmtpMessage
    static DmtpMessage CreateFrom(const std::vector<uint8_t>& buffer, int offset = 0) {
        if (buffer[offset++] != Head[0] || buffer[offset++] != Head[1]) {
            throw std::runtime_error("����ܲ���DmtpЭ������");
        }

        uint16_t protocolFlags;
        std::memcpy(&protocolFlags, &buffer[offset], sizeof(protocolFlags));
        protocolFlags = ntohs(protocolFlags); // ת��Ϊ�����ֽ�˳��
        offset += sizeof(protocolFlags);

        int32_t bodyLength;
        std::memcpy(&bodyLength, &buffer[offset], sizeof(bodyLength));
        bodyLength = ntohl(bodyLength); // ת��Ϊ�����ֽ�˳��
        offset += sizeof(bodyLength);

        DmtpMessage message(protocolFlags);
        if (bodyLength > 0) {
            message.BodyByteBlock = new ByteBlock(bodyLength);  // ��̬�����ڴ�
            message.BodyByteBlock->Write(buffer, offset, bodyLength); // д����������
        }

        return message;
    }
};

// DmtpMessage��ľ�̬��Ա��ʼ��
const std::vector<uint8_t> DmtpMessage::Head = { 100, 109 };