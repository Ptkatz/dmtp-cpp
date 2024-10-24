#pragma once
#include "Common.h"
#include "IWaitHandle.h"
#include "MsgRouterPackage.h"
#include <iostream>

class WaitRouterPackage : public MsgRouterPackage, public IWaitResult
{
public:
	bool IncludedRouter = false;

    void SetMessage(std::string message) {
        MsgRouterPackage::Message = message;
        IWaitResult::Message = message;
    }

    std::string _GetMessage() {
        return MsgRouterPackage::Message;
    }

	void PackageBody(ByteBlock& byteBlock) override {
        MsgRouterPackage::PackageBody(byteBlock);
        if (!IncludedRouter)
        {
            byteBlock.Write(Sign);
            byteBlock.Write(Status);
        }
	}

    void UnpackageBody(ByteBlock& byteBlock) override {
        MsgRouterPackage::UnpackageBody(byteBlock);
        if (!IncludedRouter)
        {
            Sign = byteBlock.ReadInt64();
            Status = static_cast<uint8_t>(byteBlock.ReadByte());
        }
    }

    void PackageRouter(ByteBlock& byteBlock) override {
        MsgRouterPackage::PackageRouter(byteBlock);
        if (IncludedRouter)
        {
            byteBlock.Write(Sign);
            byteBlock.Write(Status);
        }
    }

    void UnpackageRouter(ByteBlock& byteBlock) override {
        MsgRouterPackage::UnpackageRouter(byteBlock);
        if (IncludedRouter)
        {
            Sign = byteBlock.ReadInt64();
            Status = static_cast<uint8_t>(byteBlock.ReadByte());
        }
    }

    WaitRouterPackage() = default;
    ~WaitRouterPackage() = default;
};
