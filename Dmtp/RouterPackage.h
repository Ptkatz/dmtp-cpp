#pragma once
#include <iostream>
#include "Common.h"
#include "IPackage.h"
#include "ByteBlock.h"

class RouterPackage : public IPackage 
{
public:
	bool Route = false;
	std::string SourceId;
	std::string TargetId;

	virtual void PackageBody(ByteBlock& byteBlock) = 0;

	virtual void PackageRouter(ByteBlock& byteBlock) 
	{
		byteBlock.Write(Route);
		byteBlock.Write(SourceId);
		byteBlock.Write(TargetId);
	};

	virtual void UnpackageBody(ByteBlock& byteBlock) = 0;

	virtual void UnpackageRouter(ByteBlock& byteBlock) {
		Route = byteBlock.ReadBoolean();
		SourceId = byteBlock.ReadString();
		TargetId = byteBlock.ReadString();
	}

	void Package(ByteBlock& byteBlock) override {
		PackageRouter(byteBlock);
		PackageBody(byteBlock);
	}

	void Unpackage(ByteBlock& byteBlock) override {
		UnpackageRouter(byteBlock);
		UnpackageBody(byteBlock);
	}

	void SwitchId()
	{
		auto value = SourceId;
		SourceId = TargetId;
		TargetId = value;
	}
};
