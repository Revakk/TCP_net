#pragma once
#include "message.hpp"


namespace net {

	template<typename UnderlyingType,typename MessageType>
	struct message_gen {
		
		message<MessageType> to_message(uint32_t _type, UnderlyingType _data) {
			if constexpr (std::is_same_v<UnderlyingType, std::string>) {
				net::message<MessageType> msg;
				msg.header_ = _type;
				for (auto c : _data) {
					msg << c;
				}
				return msg;
			} 
		}
	};
}