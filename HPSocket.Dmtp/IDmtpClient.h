#pragma once
template <class TDmtpActor>
class IDmtpClient
{
public:
	bool IsOnline = false;

	TDmtpActor* Actor = new TDmtpActor;


};