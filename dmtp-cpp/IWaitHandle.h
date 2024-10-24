#pragma once
#include <iostream>

class IWaitHandle {
public:
	int64_t Sign = 0;
};

class IWaitResult: public IWaitHandle{
public:
	uint8_t Status = 0;
protected:
	std::string Message;
};

