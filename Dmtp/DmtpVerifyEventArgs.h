#pragma once
#include "Metadata.h"
class DmtpVerifyEventArgs
{
public:
	std::string Token;

	Metadata _Metadata;

	std::string Id;

	DmtpVerifyEventArgs(std::string token, std::string id, Metadata metadata)
		: Token(token), Id(id), _Metadata(metadata) {}
};