#include "tcp_network.hpp"

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
		return true;
	}

	virtual void on_client_disconnect(std::shared_ptr<net::session<CustomMsgType>> _client)
	{

	}

	virtual void on_message(std::shared_ptr<net::session<CustomMsgType>> _client,net::message<CustomMsgType>& _msg)
	{
		switch (_msg.header_.id_)
		{
		case CustomMsgType::ServerPing:
		{
			std::cout << "[" << _client->get_id() << "]: Server Ping\n";
			_client->send_message(_msg);
		}
		break;
		}
	}

};



int main()
{
	CustomServer server(60000);
	server.start_server();

	while (1)
	{
		server.update();
	}
	return 0;
}
