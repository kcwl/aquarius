#include "sdtree.h"

namespace serviced
{
	std::shared_ptr<sdnode> sdtree::find_leaf(const std::string& path)
	{
		auto iter = leafs_.find(path);

		if (iter == leafs_.end())
		{
			return nullptr;
		}

		return iter->second;
	}

	bool sdtree::create_leaf(const std::string& path, const std::string& name)
	{
		auto iter = leafs_.find(path);

		if (iter == leafs_.end())
			return false;

		auto leaf = std::make_shared<sdnode>();
		leaf->path(path);
		leaf->name(name);
		iter->second->add_child(leaf);

		return true;
	}

	void sdtree::remove_leaf(const std::string& path)
	{
		auto iter = leafs_.find(path);

		if (iter == leafs_.end())
			return;

		auto leaf = iter->second;

		auto prev = leaf->parent();

		if (prev)
		{
			prev->remove_child(leaf->self_iter());
		}

		std::shared_ptr<sdnode>{ nullptr }.swap(leaf);
	}
} // namespace serviced