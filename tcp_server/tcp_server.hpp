#pragma once
#include <memory>
#include <boost/asio.hpp>


namespace net
{
	class session : std::enable_shared_from_this<session>
	{
	public:
		session(boost::asio::ip::tcp::socket&& _socket) : socket_(std::move(_socket))
		{

		}

	private:
		boost::asio::ip::tcp::socket socket_;
		boost::asio::streambuf streambuf_;
	};



	class tcp_server
	{

	};

}
