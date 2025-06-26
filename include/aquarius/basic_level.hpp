#pragma once
#include <list>

namespace aquarius
{
	template <std::size_t Step>
	class level
	{
		struct element;

		using axis_type = std::list<element>;

		using senty_type = typename axis_type::iterator;

		struct element
		{
			std::size_t id;

			senty_type slow;

			senty_type fast;

			int64_t pos;
		};

	public:
		template <typename... Args>
		void append(std::size_t id, int64_t pos, Args...)
		{
			auto iter = std::find_if(axis_.begin(), axis_.end(), [&](const element& e) { return e.pos > pos; });

			if (iter != axis_.end())
			{
				axis_.insert(iter, element{ id, axis_.begin(), axis_.end(), pos });

				for (auto slow = iter; slow != axis_.begin(); --slow)
				{
					if (pos - iter->x < Step)
						continue;

					iter->slow = std::next(iter);
				}

				for (auto fast = iter; fast != axis_.end(); ++fast)
				{
					if (iter->x - pos < Step)
						continue;

					iter->fast = std::prev(iter);
				}
			}
			else
			{
				auto it = axis_.insert(element{ id, axis_.begin(), axis_.end(), pos });

				while (iter-- != axis_.begin())
				{
					if (pos - iter->x < Step)
						continue;

					it->slow = std::next(iter);
				}
			}
		}

		void remove(std::size_t id)
		{
			auto iter = std::find_if(axis_.begin(), axis_.end(), [&](const element& e) { return e.id == id; });

			if (iter == axis_.end())
				return;

			axis_.erase(iter);
		}

		template <typename... Args>
		void update(std::size_t id, int64_t pos, Args...)
		{
			auto iter = std::find_if(axis_.begin(), axis_.end(), [&](const element& e) { return e.id == id; });

			if (iter == axis_.end())
				return;

			auto dist = iter->x - pos;

			if (dist < 0)
			{
				for (auto it = iter->slow; it != axis_.begin(); --it)
				{
					if (iter->slow->x - it->x < dist)
						continue;

					iter->slow = std::next(it);
					break;
				}

				for (auto it = iter; it != axis_.begin(); --it)
				{
					if (iter->x - it->x < dist)
						continue;

					axis_.erase(iter);
					axis_.insert(it, iter);
					break;
				}
			}
			else
			{
				for (auto it = iter->fast; it != axis_.end(); ++it)
				{
					if (pos - iter->fast->x < dist)
						continue;

					iter->fast = std::prev(it);
					break;
				}

				for (auto it = iter; it != axis_.end(); ++it)
				{
					if (it->x - iter->x < dist)
						continue;

					axis_.erase(iter);
					axis_.insert(std::prev(it), iter);
					break;
				}
			}
		}

	private:
		axis_type axis_;
	};
} // namespace aquarius