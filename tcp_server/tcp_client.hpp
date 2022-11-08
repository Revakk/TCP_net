#pragma once

#include "tcp_common.hpp"

namespace net
{
	template<typename T>
	class client_interface
	{
	public:

		client_interface() : socket_(context_)
		{

		}

		virtual ~client_interface()
		{
			disconnect();
		}


	public:

		bool connect(const std::string& _host, const uint16_t _port)
		{
			try
			{
				session_ = std::make_unique<session<T>>();

				boost::asio::ip::tcp::resolver resolver(context_);

				auto endpoints = resolver.resolve(_host, std::to_string(_port));

				session_->connect_to_server(endpoints);

			}
			catch (std::exception& e)
			{
				std::cerr << "Client exception: " << e.what() << '\n';
				return false;
			}

			return false;
		}

		void disconnect()
		{
			if (is_connected())
			{
				session_->disconnect();
			}

			context_.stop();

			if (thr_context_.joinable())
			{
				thr_context_.join();
			}

			session_->release();
		}

		bool is_connected()
		{
			if (session_)
				return session_->is_connected();
			else
				return false;
		}

		
		messages_queue<owned_message<T>>& incoming_messages()
		{
			return q_messages_in_;
		}
		

	protected:
		boost::asio::io_context context_;

		std::thread thr_context_;

		boost::asio::ip::tcp::socket socket_;
		
		std::unique_ptr<session<T>> session_ = nullptr;


	private:

		message_queue<owned_message<T>> q_messages_in_;



	};
}
