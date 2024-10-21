#pragma once
#include <iostream>

enum class SerializationType : uint8_t {
    FastBinary = 0,
    Json = 1,
    Xml = 2,
    SystemBinary = 3
};