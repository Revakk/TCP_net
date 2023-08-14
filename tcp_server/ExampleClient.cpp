#include <iostream>
#include "tcp_network.hpp"
#include <string>

enum class message_type : uint32_t
{
	REGISTRATION,
	LOGIN
};

int main()
{
	net::message<message_type> msg;
	msg.header_.id_ = message_type::REGISTRATION;

	//std::string d = "ahoj vole";
	std::vector<char> ss;
	ss.emplace_back('p');
	ss.emplace_back('i');
	ss.emplace_back('c');
	ss.emplace_back('a');


	std::cout << net::is_iterable_v<decltype(ss)> << '\n';

	auto it = std::begin(ss);

	msg << ss;

	std::cout << msg << '\n';

	net::message<message_type> msg_l;
	msg_l.header_.id_ = message_type::LOGIN;

	for (const auto& c : ss)
	{
		msg_l << c;
	}

	std::cout << msg_l << '\n';

	net::message<message_type> msg_c;
	msg_l.header_.id_ = message_type::LOGIN;

	msg_c << "ahoj picusko";
	std::cout << msg_c << '\n';



	std::this_thread::sleep_for(std::chrono::duration<int>(10));

	//msg >> d >> c >> b >> a;

	return 0;
}
