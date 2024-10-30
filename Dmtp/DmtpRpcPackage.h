#pragma once
#include "WaitRouterPackage.h"
#include "FeedbackType.h"
#include "Metadata.h"

class DmtpRpcPackage : public WaitRouterPackage
{
public:
	FeedbackType Feedback;
	bool IsByRef = false;
	std::string MethodName;
	std::vector<std::vector<uint8_t>> ParametersBytes;
	std::vector<uint8_t> ReturnParameterBytes;
	SerializationType _SerializationType;
	Metadata _Metadata;

    void PackageBody(ByteBlock& byteBlock) override {
		WaitRouterPackage::PackageBody(byteBlock);
		byteBlock.Write(static_cast<uint8_t>(_SerializationType));
		byteBlock.Write(static_cast<uint8_t>(Feedback));
		byteBlock.Write(IsByRef);
		byteBlock.Write(MethodName);
		byteBlock.WriteBytesPackage(ReturnParameterBytes);

		if (!ParametersBytes.empty())
		{
			byteBlock.Write(static_cast<uint8_t>(ParametersBytes.size()));
			for (auto& item : ParametersBytes) {
				byteBlock.WriteBytesPackage(item);
			}
		}
		else
		{
			byteBlock.Write(static_cast<uint8_t>(0));
		}
		byteBlock.WritePackage(&_Metadata);
    }

    void UnpackageBody(ByteBlock& byteBlock) override {
		WaitRouterPackage::UnpackageBody(byteBlock);
		_SerializationType = static_cast<SerializationType>(byteBlock.ReadByte());
		Feedback = static_cast<FeedbackType>(byteBlock.ReadByte());
		IsByRef = byteBlock.ReadBoolean();
		MethodName = byteBlock.ReadString();
		ReturnParameterBytes = byteBlock.ReadBytesPackage();
		uint8_t countPar = static_cast<uint8_t>(byteBlock.ReadByte());
		ParametersBytes = std::vector<std::vector<uint8_t>>();
		for (auto i = 0; i < countPar; i++)
		{
			ParametersBytes.push_back(byteBlock.ReadBytesPackage());
		}
		if (!byteBlock.ReadIsNull())
		{
			Metadata package;
			package.Unpackage(byteBlock);
			_Metadata = package;
		}
    }

	~DmtpRpcPackage() = default;

	DmtpRpcPackage() 
	{
		IncludedRouter = true;
	}
};