#pragma once
#include <aquarius/session/session.hpp>

namespace aquarius
{
	class channel
	{
	public:
		channel(const std::string& topic)
			: channel_topic_(topic)
			, anythings_()
			, subscribes_()
			, dormancies_()
		{

		}

		~channel() = default;

	public:
		std::string channel_topic_;

		std::any anythings_;

		std::vector<std::shared_ptr<xsession>> subscribes_;

		std::vector<std::shared_ptr<xsession>> dormancies_;
	};
} // namespace aquarius