#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/trie.hpp>
#include <aquarius/noncopyable.hpp>
#include <aquarius/singleton.hpp>
#include <functional>

namespace aquarius
{
	template <typename Key, typename Value>
	class router_base : public noncopyable
	{
		using func_trie = detail::trie<Value>;

	public:
		router_base()
			: map_invokes_(new func_trie())
		{

		}

		virtual ~router_base() = default;

	protected:
		std::shared_ptr<func_trie> map_invokes_;
	};

	template <typename Result, typename... Args>
	class basic_router : public router_base<std::string_view, std::function<Result(Args...)>>
	{
	public:
		basic_router() = default;

	public:
		Result invoke(std::string_view key, Args... args)
		{
			auto func = this->map_invokes_->find(key);

			return func == nullptr ? Result{} : func(args...);
		}
	};

	template <typename T, typename Result, typename... Args>
	class single_router : public basic_router<Result, Args...>, public singleton<T>
	{};

} // namespace aquarius
