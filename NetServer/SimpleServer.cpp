#include "tcp_network.hpp"
#include<fstream>
enum class CustomMsgType : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomServer : public net::tcp_server<CustomMsgType>
{
public:
	CustomServer(uint16_t _port) : net::tcp_server<CustomMsgType>(_port)
	{

	}

protected:
	virtual bool on_client_connect(std::shared_ptr<net::session<CustomMsgType>> _client)
	{
		std::cout << "on client connect" << '\n';
		net::message<CustomMsgType> msg;
		msg.header_.id_ = CustomMsgType::ServerAccept;

		_client->send_message(msg);

		return true;
	}

	virtual void on_client_disconnect(std::shared_ptr<net::session<CustomMsgType>> _client)
	{

	}

	virtual void on_client_validated(std::shared_ptr<net::session<CustomMsgType>> _client)
	{
		std::cout << "client has successfully validated" << '\n';

		net::message<CustomMsgType> msg;

		msg.header_.id_= CustomMsgType::ServerAccept;
		std::ifstream ifs("gg2.txt");
		std::string content((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
		std::string ss{ "ahoj ty kreteneasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf" };

		auto ss1 = ss.data();
		auto ss2 = "ahoj ty kreteneasdfasdfasdfasdfasdfasdfasdfasdfasdfasdf";

		std::array<int,18> ii{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 };


		msg << 345345345;

		_client->send_message(msg);
		//std::cout << msg.header_.message_size_ << '\n';
	}

	virtual void on_message(std::shared_ptr<net::session<CustomMsgType>> _client,net::message<CustomMsgType>& _msg)
	{
		std::cout << "on message" << '\n';
		switch (_msg.header_.id_)
		{
		case CustomMsgType::ServerPing:
		{
			std::cout << "[" << _client->get_id() << "]: Server Ping\n";
			_client->send_message(_msg);
		}
		break;

		case CustomMsgType::MessageAll:
		{
			net::message<CustomMsgType> msg;
			msg.header_.id_ = CustomMsgType::MessageAll;
			broadcast_message(msg);
		}

		}
	}

};


int main()
{
	CustomServer server(60000);
	server.start_server();

	while (1)
	{
		server.update(-1,true);
	}
	return 0;
}
