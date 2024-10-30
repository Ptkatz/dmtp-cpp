#pragma once
#include "Common.h"
#include "IWaitHandle.h"
#include "Metadata.h"
#include "ISerializeObject.h"

class WaitVerify : public IWaitResult, public SerializeObjectBase {
public:
	std::string Id;

	std::string Token;

	Metadata _Metadata;

	WaitVerify() = default;

	WaitVerify(std::string token, std::string id, Metadata metadata)
		: Token(token), Id(id), _Metadata(metadata) {}

	WaitVerify(const json11::Json& json) {
		Token = json["Token"].string_value();
		Id = json["Id"].string_value();
		Message = json["Message"].string_value();
		_Metadata = Metadata(json["Metadata"].object_items());
		Sign = static_cast<int32_t>(json["Sign"].int_value());
		Status = json["Status"].int_value();
	}

	operator json11::Json() const override {
		return json11::Json::object{
			{"Token", Token},
			{"Metadata", json11::Json(_Metadata)},
			{"Id", Id},
			{"Message", Message},
			{"Sign", static_cast<int32_t>(Sign)},
			{"Status", Status}
		};
	}

};