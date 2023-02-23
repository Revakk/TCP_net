#pragma once

#include "tcp_common.hpp"


namespace net
{
	template<typename T,typename = void>
	struct is_iterable : std::false_type {};

	template<typename T>
	struct is_iterable < T, std::void_t<decltype(std::begin(std::declval<T&>())),
		decltype(std::end(std::declval<T&>()))>> : std::true_type{};

	template<typename T>
	constexpr bool is_iterable_v = is_iterable<T>::value;

	template<typename T>
	struct message_header
	{
		T id_{};
		uint32_t message_size_ = 0;
	};


	template<typename T>
	struct message
	{
	
		message_header<T> header_{};
		std::vector<uint8_t> data_{};

		size_t size() const
		{
			return (sizeof(header_) + data_.size());
		}

		template<typename T>
		static size_t size(const message<T>& _mes)
		{
			return (sizeof(_mes.header_) + _mes.data_.size());
		}

		friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
		{
			os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
			return os;
		}

		template<typename DataType>
		friend message<T>& operator << (message<T>& _msg, const DataType& _data)
		{
			if constexpr (is_iterable_v<DataType>)
			{
				typedef std::remove_reference_t<decltype(*std::begin(_data))> underlying_type;
				static_assert(std::is_standard_layout<underlying_type>::value, "Data type must be a standart layout!");
				for (auto& it : std::begin(_data))
				{
					size_t data_size = _msg.data_.size();

					_msg.data_.resize(data_size + sizeof(underlying_type));

					std::memcpy(_msg.data_.data() + data_size, it, sizeof(underlying_type));

					_msg.header_.message_size_ = _msg.data_.size();
				}
			}
			else
			{
				static_assert(std::is_standard_layout<DataType>::value, "Data type must be a standart layout!");

				size_t data_size = _msg.data_.size();

				_msg.data_.resize(data_size + sizeof(DataType));

				std::memcpy(_msg.data_.data() + data_size, &_data, sizeof(DataType));

				_msg.header_.message_size_ = _msg.data_.size();
			}

			return _msg;

		}

		//replace with iterators later
		template<typename DataType>
		friend message<T>& operator >> (message<T>& _msg, DataType& _data)
		{
			static_assert(std::is_standard_layout<DataType>::value, "Data type must be a standart layout!");

			size_t data_size = _msg.data_.size() - sizeof(DataType);


			std::memcpy(&_data, _msg.data_.data() + data_size, sizeof(DataType));

			_msg.data_.resize(data_size);

			_msg.header_.message_size_ = _msg.size();

			return _msg;

		}
		
	};

	template<typename T>
	class session;

	template<typename T>
	struct owned_message
	{
		std::shared_ptr<session<T>> remote = nullptr;
		message<T> msg;

		friend std::ofstream& operator << (std::ofstream& _os, owned_message<T>& _mes)
		{
			_os << _mes.msg;
			return _os;
		}
	};

	
}
