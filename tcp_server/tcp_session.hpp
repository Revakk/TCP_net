#pragma once
#define VALIDATION
#include "tcp_common.hpp"
#include "message_queue.hpp"
#include "message.hpp"

namespace net
{
	template<typename T>
	class tcp_server;

	template<typename T>
	class session : public std::enable_shared_from_this<session<T>>
	{
	public:
		enum class owner
		{
			SERVER,
			CLIENT
		};

		session(owner _parent, boost::asio::io_context& _context,boost::asio::ip::tcp::socket _socket,message_queue<owned_message<T>>& _q_in) : asio_context_(_context), socket_(std::move(_socket)),q_messages_in_(_q_in)
		{
			disconnect_callback_ = []() {std::cout << "[DEFAULT DISCONNECT CALLBACK] Client disconnected" << '\n'; };
			owner_type_ = _parent;
#ifdef VALIDATION
			if (owner_type_ == owner::SERVER)
			{
				handshake_out_ = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
				handshake_check_ = scramble(handshake_out_);
			}
			else
			{
				handshake_out_ = 0;
				handshake_in_ = 0;
			}
#endif
		}
		session() = delete;

		virtual ~session()
		{

		}

		uint32_t get_id()
		{
			return id_;
		}

	public:
		void connect_to_server(const boost::asio::ip::tcp::resolver::results_type& _endpoints)
		{
			if (owner_type_ == owner::CLIENT)
			{
				boost::asio::async_connect(socket_, _endpoints,
					[this](std::error_code _ec,boost::asio::ip::tcp::endpoint endpoint)
					{
						if (!_ec)
						{
#ifndef VALIDATION
							read_header();
#else
							std::cout << "read validation" << '\n';
							read_validation();
#endif
						}
						else
						{
							std::cout << "closing socket...";
							//socket_.close();
							disconnect();
							std::cout << "socket closed...";
						}
					});
			}
		}


		void disconnect()
		{
			disconnect_callback_();
			//asio_context_.stop();
			boost::asio::post(asio_context_,
				[this]()
				{
					socket_.close();

				});
		}


		bool is_connected() const
		{
			return socket_.is_open();
		}

		void connect_to_client(net::tcp_server<T>* _server,uint32_t _id)
		{
			if (owner_type_ == owner::SERVER)
			{
				if (socket_.is_open())
				{
					id_ = _id;
#ifndef VALIDATION
					read_header();
#else
					write_validation();
					read_validation(_server);

#endif
				}
				else
				{
					socket_.close();
				}
			}
		}


	public:
		void send_message(const message<T>& _msg)
		{
			boost::asio::post(asio_context_,
				[this, _msg]()
				{ 
					bool b_writing_message = !q_messages_out_.empty();
					q_messages_out_.push_back(_msg);
					if (!b_writing_message)
					{
						write_header();
					}
					
				});
		}

	protected:
		owner owner_type_;

		boost::asio::ip::tcp::socket socket_;

		boost::asio::io_context& asio_context_;

		message_queue<message<T>> q_messages_out_;

		message_queue<owned_message<T>>& q_messages_in_;
		message<T> temporary_msg_;
		std::function<void()> disconnect_callback_;

		uint32_t id_ = 0;

		//handshake validation
		uint64_t handshake_out_ = 0;
		uint64_t handshake_in_ = 0;
		uint64_t handshake_check_ = 0;

	private:
		void read_header()
		{
			boost::asio::async_read(socket_, boost::asio::buffer(&temporary_msg_.header_, sizeof(message_header<T>)),
				[this](std::error_code _ec,std::size_t _length) 
				{
					if (!_ec)
					{
						std::cout << "read header" << '\n';
						if (temporary_msg_.header_.message_size_ >= 0)
						{
							std::cout << "before resize to: " << temporary_msg_.header_.message_size_ << '\n';
							temporary_msg_.data_.resize(temporary_msg_.header_.message_size_);
							std::cout << "after resize" << '\n';
							read_body();
							std::cout << "after read_body" << '\n';
						}
						else {
							add_to_incoming_message_queue();
						}
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Header read failed."<<'\n';
						socket_.close();
						disconnect_callback_();
						//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						//disconnect();
					}
				});
		}

		void read_body()
		{
			boost::asio::async_read(socket_, boost::asio::buffer(temporary_msg_.data_.data(), temporary_msg_.header_.message_size_),
				[this](std::error_code _ec, std::size_t _length)
				{
					if (!_ec)
					{
						std::cout << "read body" << '\n';
						add_to_incoming_message_queue();
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Body read failed." << '\n';
						socket_.close();
					}
				});
		}

		void write_header()
		{
			boost::asio::async_write(socket_, boost::asio::buffer(&q_messages_out_.front().header_, sizeof(message_header<T>)),
				[this](std::error_code _ec, std::size_t _length)
				{
					if (!_ec)
					{
						if (q_messages_out_.front().data_.size() >= 0)
						{
							std::cout << "[write_header] first if" << '\n';
							write_body();
							std::cout << "[write_header] first if, after write body" << '\n';
						}
						else
						{
							std::cout << "[write_header] else" << '\n';
							q_messages_out_.pop_front();
							if (!q_messages_out_.empty())
							{
								std::cout << "[write_header] second if" << '\n';
								write_header();
							}
						}
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Write header failed." << '\n';
						socket_.close();
					}
				});
		}

		void write_body()
		{
			boost::asio::async_write(socket_, boost::asio::buffer(q_messages_out_.front().data_.data(), q_messages_out_.front().data_.size()),
				[this](std::error_code _ec, std::size_t _length)
				{
					if (!_ec)
					{
						q_messages_out_.pop_front();
						if (!q_messages_out_.empty())
						{
							write_header();
						}
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Write body failed." << '\n';
						socket_.close();
					}
				});
		}

		void add_to_incoming_message_queue()
		{
			if (owner_type_ == owner::SERVER)
			{
				q_messages_in_.push_back({ this->shared_from_this(),temporary_msg_ });
			}
			else
			{
				q_messages_in_.push_back({ nullptr,temporary_msg_ });
			}

			read_header();
		}

		uint64_t scramble(uint64_t _input)
		{
			uint64_t out = _input ^ 0xFACEBAEE0ACEFABC;
			out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0xF0F0F0F0F0F0F0) << 4;
			return out ^ 0xC0FEABCBCEFF1243;//
		}

		void write_validation()
		{
			boost::asio::async_write(socket_, boost::asio::buffer(&handshake_out_, sizeof(uint64_t)),
				[this](std::error_code _ec, std::size_t _length)
				{
					if (!_ec)
					{
						if (owner_type_ == owner::CLIENT)
						{
							read_header();
							//std::cout << "read header" << '\n';
						}
							
					}
					else
					{
						socket_.close();
					}
				});

		}

		void read_validation(net::tcp_server<T>* server = nullptr)
		{
			boost::asio::async_read(socket_, boost::asio::buffer(&handshake_in_, sizeof(uint64_t)),
				[this,server](std::error_code _ec, std::size_t _length)
				{
					if (!_ec)
					{
						if (owner_type_ == owner::SERVER)
						{
							if (handshake_in_ == handshake_check_)
							{
								std::cout << "client validated" << '\n';
								server->on_client_validated(this->shared_from_this());

								read_header();
							}
							else
							{
								std::cout << "Client disconnected (Failed validation)" << '\n';
								socket_.close();
							}
						}
						else
						{
							handshake_out_ = scramble(handshake_in_);
							std::cout << "write_validation" << '\n';
							write_validation();
						}
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Body read failed." << '\n';
						socket_.close();
					}
				});
		}

	};
}
