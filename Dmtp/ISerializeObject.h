#pragma once
#include "json11.hpp"
class SerializeObjectBase {
public:
	virtual operator json11::Json() const = 0;
};