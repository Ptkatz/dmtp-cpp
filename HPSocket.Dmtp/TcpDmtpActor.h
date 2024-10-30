#pragma once
#include "../Dmtp/DmtpActor.h"

class TcpDmtpAcotr : public DmtpActor
{
public:
	void OnHandshaking(DmtpActor* sender, DmtpVerifyEventArgs args) override {

	}

	void OnHandshaked(DmtpActor* sender, DmtpVerifyEventArgs args) override {

	}

	void OutputSend(DmtpActor* sender, std::vector<std::vector<uint8_t>> datas) override {

	}

};
