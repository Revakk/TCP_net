#pragma once

#include "tcp_common.hpp"
#include "message_queue.hpp"
#include "message.hpp"

namespace net
{
	template<typename T>
	class session : public std::enable_shared_from_this<session<T>>
	{
	public:
		session()
		{

		}

		virtual ~session()
		{

		}

	public:
		bool connect_to_sever();
		bool disconnect();
		bool is_connected() const;

	public:
		bool send_message(const message<T>& _msg);

	protected:
		boost::asio::ip::tcp::socket socket_;

		boost::asio::io_context& asio_context_;

		message_queue<message<T>> q_msgs_out_;

		message_queue<owned_message<T>>& q_messages_in_;

	private:


	};
}
