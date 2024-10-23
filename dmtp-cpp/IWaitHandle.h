#pragma once
#include <iostream>

class IWaitHandle {
public:
	int64_t Sign;
};

class IWaitResult: public IWaitHandle{
public:
	std::string Message;
	uint8_t Status;
};

