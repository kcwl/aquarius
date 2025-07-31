#pragma once
#include <list>
#include <memory>
#include <string>

namespace serviced
{
	enum class node_type
	{
		dir,
		file
	};

	class sdnode : public std::enable_shared_from_this<sdnode>
	{
	public:
		using node_iterator = typename std::list<std::shared_ptr<sdnode>>::iterator;

	public:
		sdnode() = default;

		sdnode(std::size_t id);

	public:
		void add_child(std::shared_ptr<sdnode> child);

		void remove_child(node_iterator iter);

		std::list<std::shared_ptr<sdnode>> get_childs() const;

		node_iterator self_iter();

		std::string path() const;

		void path(const std::string& path);

		std::string name() const;

		void name(const std::string& name);

		std::shared_ptr<sdnode> parent();

		std::size_t id() const;

		void id(std::size_t id);

	private:
		std::string path_;

		std::string name_;

		std::size_t id_;

		node_type type_;

		std::shared_ptr<sdnode> prev_;

		std::list<std::shared_ptr<sdnode>> childs_;

		node_iterator self_iter_;
	};
} // namespace serviced