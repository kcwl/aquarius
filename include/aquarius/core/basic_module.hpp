#pragma once


namespace aquarius
{
	namespace core
	{
		class basic_module
		{
		public:
			basic_module() = default;
			virtual ~basic_module() = default;

		public:
			virtual bool config() = 0;

			virtual bool initialize() = 0;

			virtual bool start() = 0;

			virtual bool stop() = 0;

			virtual bool enable() = 0;
		};
	}
}