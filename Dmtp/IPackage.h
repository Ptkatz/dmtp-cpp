#pragma once
#include "Common.h"
class IPackage
{
public:
    virtual void Package(ByteBlock& byteBlock) = 0;
    virtual void Unpackage(ByteBlock& byteBlock) = 0;
};
