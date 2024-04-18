#pragma once

#include "tcp_common.hpp"
#include <iterator>

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
			os << "ID:" << int(msg.header_.id_) << " Size:" << msg.header_.message_size_;
			return os;
		}

		template<typename DataType>
		friend message<T>& operator << (message<T>& _msg, const DataType& _data)
		{
			std::cout << "not a container" << '\n';
			
			if constexpr (std::is_same_v<std::string, DataType>) {
				for (auto c : _data) {
					_msg << c;
				}
			}
			else {
				static_assert(std::is_standard_layout<DataType>::value, "Data type must be a standart layout!");
				size_t data_size = _msg.data_.size();

				_msg.data_.resize(data_size + sizeof(DataType));

				std::memcpy(_msg.data_.data() + data_size, &_data, sizeof(DataType));

				_msg.header_.message_size_ = _msg.data_.size();
			}			
			return _msg;
		}

		template<typename DataType, template <typename,typename = std::allocator<DataType>> class Container>
		friend message<T>& operator << (message<T>& _msg,const Container<DataType>& _data)
		{
			std::cout << " container" << '\n';
			static_assert(is_iterable_v<Container<DataType>>, "Data type must be iterable!");
			if (is_iterable_v<Container<DataType>>)
			{
				std::cout << " is iterable" << '\n';
				typedef std::remove_reference_t<decltype(*std::begin(_data))> underlying_type;
				static_assert(std::is_standard_layout<underlying_type>::value, "Data type must be a standart layout!");
				for (auto it =std::begin(_data); it != std::end(_data);it++)
				{
					size_t data_size = _msg.data_.size();

					_msg.data_.resize(data_size + sizeof(underlying_type));

					std::memcpy(_msg.data_.data() + data_size, &(*it), sizeof(underlying_type));

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
			if constexpr (std::is_same_v<std::string, DataType>) {
				_data = std::string(_msg.data_.begin(), _msg.data_.end());
			}
			else {
				static_assert(std::is_standard_layout<DataType>::value, "Data type must be a standart layout!");
				size_t data_size = _msg.data_.size() - sizeof(DataType);

				std::memcpy(&_data, _msg.data_.data() + data_size, sizeof(DataType));

				_msg.data_.resize(data_size);

				_msg.header_.message_size_ = _msg.size();
			}

				
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