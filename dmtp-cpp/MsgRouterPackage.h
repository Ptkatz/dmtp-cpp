#pragma once
#include <iostream>
#include "RouterPackage.h"
#include "Common.h"

class MsgRouterPackage : public RouterPackage
{
public:
	std::string Message;
	void PackageBody(ByteBlock& byteBlock) override {
		byteBlock.Write(Message);
	}

	// ͨ�� RouterPackage �̳�
	void UnpackageBody(ByteBlock& byteBlock) override {
		Message = byteBlock.ReadString();
	}

};
