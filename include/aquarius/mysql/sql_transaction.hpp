#pragma once
#include <string>
#include <vector>
#include <memory>

namespace sqlpro
{
	class sql_transaction
	{
	public:
		void append(const std::string& sql)
		{
			return queries_.push_back(sql);
		}

		template<typename F, typename... Args>
		void append(F&& sql, Args&&... args)
		{
			return append(fmt(std::forward<F>(sql), std::forward<Args>(args)...));
		}

		template<typename C>
		bool execute(std::shared_ptr<C> conn_ptr)
		{
			if (!conn_ptr->execute_transaction(this))
			{
				return true;
			}

			clear();

			return false;
		}

	private:
		void clear()
		{
			queries_.clear();
		}

	public:
		std::vector<std::string> queries_;
	};
}