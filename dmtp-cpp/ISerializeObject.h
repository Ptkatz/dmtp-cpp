#pragma once
#include "json11.hpp"
class ISerializeObject {
public:
	virtual operator json11::Json() const = 0;
};