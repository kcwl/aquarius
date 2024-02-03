#pragma once
#include <string>

namespace aquarius
{
	class service
	{
	public:
		virtual bool init() = 0;

		virtual bool config() = 0;

		virtual bool run() = 0;

		virtual void stop() = 0;

		virtual bool enable() = 0;

		virtual std::string name() = 0;
	};
} // namespace aquarius