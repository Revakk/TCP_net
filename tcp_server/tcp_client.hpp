#pragma once

#include "tcp_common.hpp"
#include "message.hpp"
#include "message_queue.hpp"
#include "tcp_session.hpp"

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

				boost::asio::ip::tcp::resolver resolver(context_);
				//boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(_port));

				auto endpoints = resolver.resolve(_host, std::to_string(_port));

				session_ = std::make_unique<session<T>>(
					session<T>::owner::CLIENT,
					context_,boost::asio::ip::tcp::socket(context_),q_messages_in_);

				session_->connect_to_server(endpoints);
				thr_context_ = std::thread([this]() {context_.run(); });

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

			session_.release();
		}

		void send(const message<T>& _msg)
		{
			if (is_connected())
			{
				session_.send_message(_msg);
			}
		}

		bool is_connected()
		{
			if (session_)
				return session_->is_connected();
			else
				return false;
		}

		
		message_queue<owned_message<T>>& incoming_messages()
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
