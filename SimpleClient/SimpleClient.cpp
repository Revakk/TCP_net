#include "tcp_network.hpp"
#include <Windows.h>

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


};



int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);

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
		if (key[2] && !old_key[2]) b_quit = true;


		for (int i = 0; i < 3; i++) old_key[i] = key[i];


		if (c.is_connected())
		{
			if (!c.incoming_messages().empty())
			{
				auto msg = c.incoming_messages().pop_front().msg;

				switch (msg.header_.id_)
				{
				case CustomMsgType::ServerPing:
				{
					std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point time_then;

					msg >> time_then;
					std::cout << "Ping: " << std::chrono::duration<double>(time_now - time_then).count() << '\n';
				}
				break;
					
				}
			}
		}
		else
		{
			b_quit = true;
		}
	}


	return 0;
}
