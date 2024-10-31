#pragma once
#include "Common.h"
#include "TcpDmtpClient.h"

class TcpDmtpActor : public DmtpActor
{
public:
	TcpDmtpClient* Client;

	void OutputSend(DmtpActor* sender, std::vector<std::vector<uint8_t>> datas) override {
		std::vector<uint8_t> transferBytes;
		for (const auto& innerVec : datas) {
			transferBytes.insert(transferBytes.end(), innerVec.begin(), innerVec.end());
		}
		Client->DmtpActorSend(transferBytes);
	}


	void OnHandshaking(DmtpActor* sender, DmtpVerifyEventArgs args) override {

	}

	void OnHandshaked(DmtpActor* sender, DmtpVerifyEventArgs args) override {

	}


	TcpDmtpActor() {
		Client->Actor = this;
	};
};
