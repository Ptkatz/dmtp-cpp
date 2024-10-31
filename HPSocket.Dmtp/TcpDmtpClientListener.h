#pragma once
#include <iostream>
#include "../HPSocket/Include/HPSocket/HPSocket.h"

class TcpDmtpClientListener : public CTcpClientListener
{
public:
    EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override
    {
        printf("OnReceive\n");
        return EnHandleResult::HR_OK;
    }
    EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode) override
    {
        printf("OnClose\n");
        return EnHandleResult::HR_OK;
    }

    EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID) override {
        printf("OnHandShake\n");
        return EnHandleResult::HR_OK;
    }

    EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength) override {
        printf("OnSend\n");
        return EnHandleResult::HR_OK;
    }

};
