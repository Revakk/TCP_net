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
			std::lock_guard(m_);
			return deQ_.front();
		}

		const T& back()
		{
			std::lock_guard(m_);
			return deQ_.back();
		}

		void push_back(const T& _data)
		{
			std::lock_guard(m_);
			deQ_.emplace_back(std::move(_data));
		}

		void push_front(const T& _data)
		{
			std::lock_guard(m_);
			deQ_.emplace_front(std::move(_data));
		}

		bool empty() const
		{
			std::lock_guard(m_);
			return deQ_.empty();
		}

		bool size() const
		{
			std::lock_guard(m_);
			return deQ_.size();
		}

		void clear()
		{
			std::lock_guard(m_);
			deQ_.clear();
		}

		T pop_front()
		{
			std::lock_guard(m_);
			auto t = std::move(deQ_.front());
			deQ_.pop_front();
			return t;
		}

		T pop_back()
		{
			std::lock_guard(m_);
			auto t = std::move(deQ_.back());
			deQ_.pop_back();
			return t;
		}


	private:
		std::deque<T> deQ_;
		std::mutex m_;
	};
}
