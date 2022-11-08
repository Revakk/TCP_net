#pragma once

#include "tcp_common.hpp"
#include "message_queue.hpp"
#include "message.hpp"

namespace net
{
	template<typename T>
	class connection : public std::enable_shared_from_this<connection<T>>
	{
	public:
		connection()
		{

		}

		virtual ~connection()
		{

		}

	private:


	};
}
