#pragma once
#include <chrono>
#include "Common.h"
#include "ByteBlock.h"
#include "DmtpVerifyEventArgs.h"
#include "DmtpMessage.h"
#include "WaitVerify.h"

class IDmtpActor
{
public:
	virtual bool Ping(int millisecondsTimeout) = 0;

	virtual void Send(uint16_t protocol, std::vector<uint8_t> buffer, int offset, int length) = 0;

	virtual void Send(uint16_t protocol, ByteBlock byteBlock) = 0;

	virtual void SendString(uint16_t protocol, std::string value) = 0;

	virtual bool SendClose(std::string msg) = 0;
};

class DmtpActor : public IDmtpActor
{
public:
#pragma region 属性
	std::string Id;
	bool IsHandshaked = false;
	std::chrono::steady_clock::time_point LastActiveTime;
#pragma endregion


#pragma region 委托
	virtual void OnHandshaking(DmtpActor* sender, DmtpVerifyEventArgs args) = 0;

	virtual void OnHandshaked(DmtpActor* sender, DmtpVerifyEventArgs args) = 0;

	virtual void OutputSend(DmtpActor* sender, std::vector<std::vector<uint8_t>> datas) = 0;
#pragma endregion

#pragma region 方法
	void Handshake(std::string verifyToken, std::string id, Metadata metadata) {
		if (IsHandshaked)
		{
			return;
		}

		DmtpVerifyEventArgs args = DmtpVerifyEventArgs(verifyToken, id, metadata);
		OnHandshaking(this, args);

		WaitVerify waitVerify = WaitVerify(args.Token, args.Id, args._Metadata);
		SendJsonObject(P1_Handshake_Request, waitVerify);
	}


	bool Ping(int millisecondsTimeout = 5000) override {
		return false;
	}

	void Send(uint16_t protocol, std::vector<uint8_t> buffer, int offset, int length) override {
		std::vector<std::vector<uint8_t>> transferBytes;
		transferBytes.push_back(DmtpMessage::Head);
		transferBytes.push_back(TouchSocketBitConverter::GetBigEndianBytes(protocol));
		transferBytes.push_back(TouchSocketBitConverter::GetBigEndianBytes(length));
		transferBytes.push_back(std::vector<unsigned char>(buffer.begin() + offset, buffer.begin() + offset + length));
		OutputSend(this, transferBytes);

		LastActiveTime = std::chrono::steady_clock::now();
	}

	void Send(uint16_t protocol, ByteBlock byteBlock) override {

	}

	void SendString(uint16_t protocol, std::string value) override {

	}

	bool SendClose(std::string msg) override {
		return false;
	}
#pragma endregion

#pragma region const
	const static uint16_t P0_Close = 0;
	const static uint16_t P1_Handshake_Request = 1;
	const static uint16_t P2_Handshake_Response = 2;
	const static uint16_t P3_ResetId_Request = 3;
	const static uint16_t P4_ResetId_Response = 4;
	const static uint16_t P5_Ping_Request = 5;
	const static uint16_t P6_Ping_Response = 6;
	const static uint16_t P7_CreateChannel_Request = 7;
	const static uint16_t P8_CreateChannel_Response = 8;
	const static uint16_t P9_ChannelPackage = 9;
#pragma endregion


private:
	template <typename T>
	void SendJsonObject(uint16_t protocol, const T& obj) {
		const json11::Json& jsonValue = static_cast<json11::Json>(obj);
		std::string jsonString = jsonValue.dump();
		std::vector<uint8_t> bytes(jsonString.begin(), jsonString.end());
		Send(protocol, bytes, 0, bytes.size());
	}

};