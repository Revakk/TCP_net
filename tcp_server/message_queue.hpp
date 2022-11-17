#pragma once


namespace net
{
	template<typename T>
	class message_queue
	{
	public:
		message_queue() = default;
		message_queue(const message_queue<T>&) = delete;

		virtual ~message_queue() { clear(); }

	public:

		const T& front()
		{
			std::scoped_lock lock(m_);
			return deQ_.front();
		}

		const T& back()
		{
			std::scoped_lock lock(m_);
			return deQ_.back();
		}

		void push_back(const T& _data)
		{
			std::scoped_lock lock(m_);
			deQ_.emplace_back(std::move(_data));
		}

		void push_front(const T& _data)
		{
			std::scoped_lock lock(m_);
			deQ_.emplace_front(std::move(_data));
		}

		bool empty()
		{
			std::scoped_lock lock(m_);
			return deQ_.empty();
		}

		bool size()
		{
			std::scoped_lock lock(m_);
			return deQ_.size();
		}

		void clear()
		{
			std::scoped_lock lock(m_);
			deQ_.clear();
		}

		T pop_front()
		{
			std::scoped_lock lock(m_);
			auto t = std::move(deQ_.front());
			deQ_.pop_front();
			return t;
		}

		T pop_back()
		{
			std::scoped_lock lock(m_);
			auto t = std::move(deQ_.back());
			deQ_.pop_back();
			return t;
		}


	private:
		std::deque<T> deQ_;
		std::mutex m_;
	};
}
