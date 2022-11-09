#include <iostream>
#include "tcp_network.hpp"
#include <string_view>

enum class message_type : uint32_t
{
	REGISTRATION,
	LOGIN
};

int main()
{
	net::message<message_type> msg;
	msg.header_.id_ = message_type::REGISTRATION;

	int a = 1;
	bool b = true;
	float c = 3.14159f;

	//std::string d = "ahoj vole";
	std::string_view d = "ahoj vole";

	msg << a << b << c << d;

	a = 99;
	b = false;
	c = 99.0f;
	d = "ahoj mrdko";

	msg >> d >> c >> b >> a;

	return 0;
}
