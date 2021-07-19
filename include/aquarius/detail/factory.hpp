#pragma once
#include <any>
#include <stdint.h>
#include <type_traits>
#include <unordered_map>
#include "singleton.hpp"
#include "type_traits.hpp"
#include "noncopyable.hpp"
#include <optional>

namespace aquarius
{
	namespace detail
	{
		template<class Key, class T>
		class factory
			: public noncopyable
			, public std::enable_shared_from_this<factory<Key,T>>
			, public singleton<factory<Key, T>>
		{
		public:
			using key_t = Key;
			using function_t = std::function<std::shared_ptr<T>()>;
		public:
			template<class Func>
			bool regist(Key&& topic, Func&& func)
			{
				return regist_impl(std::forward<Key>(topic), std::forward<Func>(func));
			}

			decltype(auto) attach(Key&& topic)
			{
				return attach_impl(std::forward<Key>(topic));
			}

		private:
			template<class Func>
			bool regist_impl(Key&& topic, Func&& func)
			{
				return factory_list_.insert(std::make_pair(std::forward<Key>(topic), std::forward<Func>(func))).second;
			}

			std::optional<function_t> attach_impl(Key&& topic)
			{
				auto iter = factory_list_.find(std::forward<Key>(topic));
				if(iter == factory_list_.end())
					return std::nullopt;

				return iter->second;
			}

			void remove(Key&& topic)
			{
				auto iter = factory_list_.find(std::forward<Key>(topic));
				if(iter == factory_list_.end())
					return;

				factory_list_.erase(iter);
			}

		private:
			std::unordered_map<Key, function_t> factory_list_;
		};

		template<class T>
		using message_register = factory<std::size_t, T>;

		template<class Base,class Context>
		class ContextRegister
		{
			using key_t = typename message_register<Base>::key_t;
		public:
			template<class... Args>
			ContextRegister(key_t&& topic, Args&&... args)
			{
				regist(std::forward<key_t>(topic), std::forward<Args>(args)...);
			}

		private:
			template<class... Args>
			void regist(key_t&& topic, Args&&... args, std::enable_if_t<!std::is_void_v<Context>>* = 0)
			{
				message_register<Base>::instance().regist
				(std::forward<key_t>(topic),
					[&]
					{
						return std::dynamic_pointer_cast<Base>(std::make_shared<Context>(std::forward<Args>(args)...));
					}
				);
			}
		};
	}
}