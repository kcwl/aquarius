#include "sdnode.h"

namespace serviced
{

	sdnode::sdnode(std::size_t id)
		: id_(id)
	{}

	void sdnode::add_child(std::shared_ptr<sdnode> child)
	{
		if (child->prev_)
		{
			std::shared_ptr<sdnode>{ nullptr }.swap(child->prev_);
		}

		child->prev_ = this->shared_from_this();

		childs_.push_back(child);
	}

	void sdnode::remove_child(node_iterator iter)
	{
		if (iter == childs_.end())
			return;

		std::shared_ptr<sdnode>{ nullptr }.swap((*iter)->prev_);

		childs_.erase(iter);
	}

	std::list<std::shared_ptr<sdnode>> sdnode::get_childs() const
	{
		return childs_;
	}

	sdnode::node_iterator sdnode::self_iter()
	{
		return self_iter_;
	}

	std::string sdnode::path() const
	{
		return path_;
	}

	void sdnode::path(const std::string& path)
	{
		path_ = path;
	}

	std::string sdnode::name() const
	{
		return name_;
	}

	void sdnode::name(const std::string& name)
	{
		name_ = name;
	}

	std::shared_ptr<sdnode> sdnode::parent()
	{
		return prev_;
	}

	std::size_t sdnode::id() const
	{
		return id_;
	}

	void sdnode::id(std::size_t id)
	{
		id_ = id;
	}
} // namespace serviced