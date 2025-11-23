#pragma once
#include <aquarius/sql/sql_core.hpp>
#include <aquarius/detail/tag.hpp>
#include <aquarius/tag_invoke.hpp>
#include <aquarius/coroutine.hpp>
#include <aquarius/detail/io_service_pool.hpp>

namespace aquarius
{

	class sql_pool_core
	{
	public:
		sql_pool_core()
			: sql_pool_()
			, next_sql_adaptor_()
		{}

		virtual ~sql_pool_core() = default;

	public:
		auto run() -> awaitable<error_code>
		{
			database_param param{};

			value_to<db_tag>(param);

			for (int i = 0; i < param.pool_size; ++i)
			{
				auto sql_ptr = std::make_shared<sql_core>(param);

				auto ec = co_await sql_ptr->async_connect();

				if (ec)
					co_return ec;

				sql_pool_.push_back(sql_ptr);
			}
		}

		std::shared_ptr<sql_core> get_sql_adaptor()
		{
			if (next_sql_adaptor_ == sql_pool_.size())
				next_sql_adaptor_ = 0;

			return sql_pool_[next_sql_adaptor_++];
		}

	private:
		std::vector<std::shared_ptr<sql_core>> sql_pool_;

		std::size_t next_sql_adaptor_;
	};

	inline sql_pool_core& sql_pool()
	{
		static sql_pool_core pool;

		return pool;
	}
} // namespace aquarius