#pragma once
#include <aquarius/detail/trie.hpp>
#include <memory>
#include <functional>
#include <aquarius/virgo/http_fields.hpp>

namespace aquarius
{
	template <typename Session, typename Func>
	class basic_router
	{
	public:
		using session = Session;
		using function_type = Func;
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