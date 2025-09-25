#pragma once
#include <aquarius/detail/trie.hpp>
#include <memory>
#include <functional>

namespace aquarius
{
	template <typename Session>
	class basic_router
	{
	public:
		using session = Session;
		using buffer_t = detail::flex_buffer<char>;
		using function_type = std::function<bool(std::shared_ptr<Session>, buffer_t)>;
		using func_trie = detail::trie<function_type>;

	public:
		basic_router()
			: map_invokes_(new func_trie())
		{}

		virtual ~basic_router() = default;

	protected:
		std::shared_ptr<func_trie> map_invokes_;
	};
} // namespace aquarius