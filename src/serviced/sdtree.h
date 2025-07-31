#pragma once
#include "sdnode.h"
#include <unordered_map>

namespace serviced
{
	class sdtree
	{
	public:
		sdtree() = default;

	public:
		std::shared_ptr<sdnode> find_leaf(const std::string& path);

		bool create_leaf(const std::string& path, const std::string& name);

		void remove_leaf(const std::string& path);

	private:
		std::shared_ptr<sdnode> root_;

		std::unordered_map<std::string, std::shared_ptr<sdnode>> leafs_;
	};
} // namespace serviced