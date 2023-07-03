#pragma once
#include "../session.hpp"
#include "singleton.hpp"


namespace aquarius
{
	namespace detail
	{
		class session_manager : public singleton<session_manager>
		{
		public:
			bool push(std::shared_ptr<basic_session> session_ptr)
			{
				sessions_.emplace(session_ptr->uid(), session_ptr);

				return true;
			}

			bool erase(std::size_t id)
			{
				return sessions_.erase(id) != 0;
			}

			std::shared_ptr<basic_session> find(std::size_t id)
			{
				auto iter = sessions_.find(id);

				if (iter == sessions_.end())
					return nullptr;
				
				return iter->second;
			}

		private:
			std::unordered_map<std::size_t, std::shared_ptr<basic_session>> sessions_;
		};
	}
}