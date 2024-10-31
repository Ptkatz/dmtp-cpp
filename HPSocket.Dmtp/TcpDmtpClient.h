#pragma once
#include "Common.h"
#include "TcpDmtpClientListener.h"
#include "../Dmtp/DmtpActor.h"

class TcpDmtpClient
{
public:
	bool IsOnline = false;

	TcpDmtpClientListener EventListener;

	CTcpClientPtr ClientObject = CTcpClientPtr(&EventListener);

	DmtpActor* Actor = new TcpDmtpActor();

	void Connect(std::string host, uint16_t port, std::string verifyToken, std::string id, Metadata metadata) {
		if (!IsOnline)
		{
			IsOnline = ClientObject->Start((LPCTSTR)host.c_str(), port, false, nullptr);
		}

		if (IsOnline) {
			if (Actor && Actor->IsHandshaked) {
				return;
			}
			if (Actor) {
				//std::vector<uint8_t> s = { 10, 20, 30 };
				//DmtpActorSend(s);

				Actor->Handshake(verifyToken, id, metadata);
			}
		}

		Sleep(2000);
	}

	void DmtpActorSend(std::vector<uint8_t> transferBytes) {
		bool status = ClientObject->Send(transferBytes.data(), transferBytes.size());
	}
};
