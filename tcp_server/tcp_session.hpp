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
		enum class owner
		{
			SERVER,
			CLIENT
		};

		session(owner _parent, boost::asio::io_context& _context,boost::asio::ip::tcp::socket _socket,message_queue<owned_message<T>>& _q_in) : asio_context_(_context), socket_(std::move(_socket)),q_messages_in_(_q_in)
		{
			owner_type_ = _parent;
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
							read_header();
						}
						else
						{

						}
					});
			}
		}


		void disconnect()
		{
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

		void connect_to_client(uint32_t _id)
		{
			if (owner_type_ == owner::SERVER)
			{
				if (socket_.is_open())
				{
					id_ = _id;
					read_header();
				}
			}
		}


	public:
		bool send_message(const message<T>& _msg)
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

		uint32_t id_ = 0;

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
					}
					else
					{
						std::cout << "[" << id_ << "]" << "Header read failed."<<'\n';
						socket_.close();
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

	};
}
