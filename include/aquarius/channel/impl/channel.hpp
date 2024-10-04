#pragma once
#include <memory>
#include <string>
#include <vector>

namespace aquarius
{
	namespace channel
	{
		template<typename _Role>
		class basic_channel
		{
		public:
			using role_t = _Role;

		public:
			basic_channel() = default;
			~basic_channel() = default;

		public:
			std::string topic() const
			{
				return topic_;
			}

			void subscribe(role_t* subscribe_role)
			{
				subscirbers_.push_back(subscribe_role);
			}

			void call(const std::string& command, std::size_t id)
			{
				for (auto& subs : subscirbers_)
				{
					subs->apply(command, id);
				}
			}

		private:
			std::string topic_;

			std::vector<role_t*> subscirbers_;
		};
	} // namespace channel
} // namespace aquarius