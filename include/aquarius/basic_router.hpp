#pragma once
#include <aquarius/detail/trie.hpp>
#include <memory>
#include <map>

namespace aquarius
{
	template <typename Session, typename Key, typename Func>
	class basic_router
	{
	public:
		using session = Session;
		using function_type = Func;
		using func_trie = detail::trie<function_type>;

	public:
		basic_router()
			: map_invokes_()
		{}

		virtual ~basic_router() = default;

	public:
		void push(Key k, std::string_view proto, Func&& f)
		{
			auto& ptr = this->map_invokes_[k];
			if (!ptr)
				ptr = std::make_shared<func_trie>();

			ptr->add(proto, f);
		}

	protected:
		std::map<Key, std::shared_ptr<func_trie>> map_invokes_;
	};
} // namespace aquarius