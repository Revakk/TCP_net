#pragma once
#include "tcp_common.hpp"
#include "tcp_network.hpp"


namespace net
{
	template <typename T>
	class tcp_server
	{
	public:
		tcp_server(uint16_t _port) : asio_acceptor_(asio_context_,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),_port))
		{

		}

		virtual ~tcp_server()
		{
			stop();
		}

		bool start_server()
		{
			try
			{
				wait_for_client_connection();

				thread_context_ = std::thread([this]() {asio_context_.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << "[SERVER] exception: " << e.what() << '\n';
				return false;
			}

			std::cout << "[SERVER] started!" << '\n';
			return true;
		}

		void stop()
		{
			asio_context_.stop();

			if (thread_context_.joinable())
			{
				thread_context_.join();
			}

			std::cout << "[SERVER] Stopped!" << '\n';
		}

		/**
		 * async.
		 * 
		 */
		void wait_for_client_connection()
		{
			asio_acceptor_.async_accept([this] (std::error_code _ec, boost::asio::ip::tcp::socket _socket)
			{
				if (!_ec)
				{
					std::cout << "[SERVER] New Connection: " << _socket.remote_endpoint() << '\n';

					/*std::shared_ptr<session<T>> new_connection = std::make_shared<session<T>>(session<T>::owner::server,asio_context_,std::move(socket),messages_in_);*/

					/*if (on_client_connect(new_connection))
					{
						deq_connections_.push_back(std::move(new_connection));

						deq_connections_.back()->connect_to_client(ID_counter_++);

						std::cout << "[" << deq_connections_->back()->get_id() << "] connection approved" << '\n';
					}
					else
					{
						std::cout << "[----] Connection denied!" << '\n';
					}*/
				}
				else
				{
					std::cout << "[SERVER] new connection error: " << _ec.message() << '\n';
				}

				wait_for_client_connection();
			});

		}

		void message_client(std::shared_ptr<session<T>> _client, const message<T>& _msg)
		{
			if (_client && _client->is_connected())
			{
				_client->send(_msg);
			}
			else
			{
				on_client_disconnect(_client);
				_client.reset();
				deq_connections_.erase(std::remove(deq_connections_.begin(), deq_connections_.end(), _client), deq_connections_.end());
			}
		}

		void broadcast_message(const message<T>& _msg, std::shared_ptr<session<T>> _client_to_ignore = nullptr)
		{
			bool invalid_client_exists = false;

			for (auto& client : deq_connections_)
			{
				if (client && client->is_connected())
				{
					if (client != _client_to_ignore)
					{
						client->send(_msg);
					}
					
				}
				else
				{
					on_client_disconnect(client);
					client.reset();
				}
			}

			if (invalid_client_exists)
			{
				deq_connections_.erase(std::remove(deq_connections_.begin(), deq_connections_.end(), nullptr), deq_connections_.end());
			}
		}

		void update(size_t _max_messages = -1)
		{
			size_t message_count = 0;
			while (message_count < _max_messages && !messages_in_.empty())
			{
				auto msg = messages_in_.pop_front();

				on_message(msg.remote, msg.msg);

				message_count++;
			}
		}


	protected:
		virtual bool on_client_connect(std::shared_ptr<session<T>> _client)
		{
			return false;
		}

		virtual void on_client_disconnect(std::shared_ptr<session<T>> _client)
		{

		}

		virtual void on_message(std::shared_ptr<session<T>> _client, message<T>& _msg)
		{

		}


	protected:
		message_queue<owned_message<T>> messages_in_;

		std::deque<std::shared_ptr<session<T>>> deq_connections_;

		boost::asio::io_context asio_context_;
		std::thread thread_context_;

		boost::asio::ip::tcp::acceptor asio_acceptor_;
		uint32_t ID_counter_ = 10000;
	};

}
