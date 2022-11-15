#include "tcp_network.hpp"

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


};



int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);
	return 0;
}
