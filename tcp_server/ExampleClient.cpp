#include <iostream>
#include "tcp_network.hpp"
#include <nlohmann/json.hpp>
#include <string>

enum class message_type : uint32_t
{
	JSON,
};

class CustomClient : public net::client_interface<message_type>
{
public:

	void ping_server()
	{
		net::message<message_type> msg;
		msg.header_.id_ = message_type::JSON;

		nlohmann::json js;

		js["time"] = std::chrono::system_clock::now().time_since_epoch().count();
		//std::chrono::system_clock::time_point time_now = ;

		msg << js;
		send(msg);
	}

	void message_all()
	{
		net::message<message_type> msg;
		msg.header_.id_ = message_type::JSON;
		nlohmann::json js;
		js["time"] = std::chrono::system_clock::now().time_since_epoch().count();
		msg << js;

		send(msg);
	}
};


int main()
{
	//	net::message<message_type> msg;
	//msg.header_.id_ = message_type::JSON;

	//nlohmann::json js;
	//js["kokot"] = "pica";
	//js["pica"] = "kokot";

	////std::string d = "ahoj vole";
	//msg << js;

	//std::cout << msg << '\n';
	//
	//nlohmann::json js_o;

	//msg >> js_o;

	//std::cout << js_o.dump();


	//std::this_thread::sleep_for(std::chrono::duration<int>(10));

	////msg >> d >> c >> b >> a;

	//return 0;

	/*if (std::is_convertible_v<nlohmann::json, std::string>)
	{
		std::cout << "is convertible" << '\n';

		nlohmann::json js;
		js["ahoj"] = "debile";

		auto ss = to_string(js);

		std::cout << ss;
	}


	net::message<message_type> msg;

	msg.header_.id_ = message_type::JSON;
	nlohmann::json js;
	js["kokot"] = "pica";
	js["pica"] = "kokot";
	msg << js.dump();

	std::string js_o;

	msg >> js_o;

	std::cout << js_o << '\n';

	return 0;*/

	//std::array<CustomClient, 100> clients{};
	CustomClient c;

	//std::for_each(std::execution::par, clients.begin(), clients.end(), [](CustomClient& client)
	//	{
	//		client.connect("127.0.0.1", 60000);
	//	});

	//for (auto& client : clients) {
	//	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//	client.connect("127.0.0.1", 60000);
	//}

	//std::this_thread::sleep_for(std::chrono::seconds(60));

	//return 0;

	auto successful = c.connect("127.0.0.1", 6666);

	//return 0;

	if (successful)
	{
		std::cout << "jsem tam :)";
	}

	bool key[3] = { false, false, false };
	bool old_key[3] = { false,false,false };

	bool b_quit = false;

	while (!b_quit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) c.ping_server();
		if (key[1] && !old_key[1]) c.message_all();
		if (key[2] && !old_key[2]) b_quit = true;


		for (int i = 0; i < 3; i++) old_key[i] = key[i];


		if (c.is_connected())
		{
			if (!c.incoming_messages().empty())
			{
				auto msg = c.incoming_messages().pop_front().msg;
				std::cout << "received msg" << '\n';
				switch (msg.header_.id_)
				{

				case message_type::JSON:
					std::cout << "JSON MSG RECEIVED:" << '\n';
					std::string js;
					try
					{
						msg >> js;
						auto parsed = nlohmann::json::parse(js);
						std::cout << parsed.dump() << '\n';
					}
					catch (const std::exception& e)
					{
						std::cout << e.what();
					}
					
				}
			}
		}
		else
		{
			c.disconnect();
			std::this_thread::sleep_for(std::chrono::duration<int>(1));
			c.connect("127.0.0.1", 6666);
		}
			//std::this_thread::sleep_for(std::chrono::duration<int>(1));
		}

		return 0;
	}

//
//int main()
//{
//	net::message<message_type> msg;
//	msg.header_.id_ = message_type::JSON;
//
//	nlohmann::json js;
//	js["kokot"] = "pica";
//	js["pica"] = "kokot";
//
//	//std::string d = "ahoj vole";
//	std::vector<char> ss;
//	ss.emplace_back('p');
//	ss.emplace_back('i');
//	ss.emplace_back('c');
//	ss.emplace_back('a');
//
//	msg << js;
//
//	std::cout << msg << '\n';
//	
//	nlohmann::json js_o;
//
//	msg >> js_o;
//
//	std::cout << js_o.dump();
//
//
//	std::this_thread::sleep_for(std::chrono::duration<int>(10));
//
//	//msg >> d >> c >> b >> a;
//
//	return 0;
//}
