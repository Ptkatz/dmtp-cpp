#pragma once
#include "Common.h"
#include "IWaitHandle.h"
#include "MsgRouterPackage.h"
#include <iostream>

class WaitRouterPackage : public MsgRouterPackage, public IWaitResult
{
public:
	bool IncludedRouter = false;
    int64_t Sign;


	void PackageBody(ByteBlock& byteBlock) override {
        MsgRouterPackage::PackageBody(byteBlock);
        if (!IncludedRouter)
        {
            byteBlock.Write(Sign);
            byteBlock.Write(Status);
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

    void UnpackageBody(ByteBlock& byteBlock) override {
        MsgRouterPackage::UnpackageBody(byteBlock);
        if (!IncludedRouter)
        {
            Sign = byteBlock.ReadInt64();
            Status = static_cast<uint8_t>(byteBlock.ReadByte());
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
};
