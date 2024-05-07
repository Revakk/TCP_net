#include "tcp_network.hpp"
#include <Windows.h>

#include <execution>

enum class CustomMsgType : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomClient : public net::client_interface<CustomMsgType>
{
public:

	void ping_server()
	{
		net::message<CustomMsgType> msg;
		msg.header_.id_ = CustomMsgType::ServerPing;

		std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();

		msg << time_now;
		send(msg);
	}

	void message_all()
	{
		net::message<CustomMsgType> msg;
		msg.header_.id_ = CustomMsgType::MessageAll;
		send(msg);
	}
};


int main()
{
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

				case CustomMsgType::ServerAccept:
				{
					std::cout << "server accepted message" << '\n';
					std::cout << msg.header_.message_size_ << '\n';
					for (auto a : msg.data_)
					{
						std::cout << char(a);
					}
				}
				break;

				case CustomMsgType::ServerPing:
				{
					std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point time_then;

					msg >> time_then;
					std::cout << "Ping: " << std::chrono::duration<double>(time_now - time_then).count() << '\n';
				}
				break;

				case CustomMsgType::ServerMessage:
				{
					uint32_t client_ID;
					msg >> client_ID;
					std::cout << "hello from [" << client_ID << "]\n";
				}
				break;


				}
			}
		}
		else
		{
			c.disconnect();
			std::this_thread::sleep_for(std::chrono::duration<int>(1));
			c.connect("127.0.0.1", 60000);
		}
		//std::this_thread::sleep_for(std::chrono::duration<int>(1));
	}

	return 0;
}
