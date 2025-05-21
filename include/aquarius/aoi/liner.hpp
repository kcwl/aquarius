#pragma once

namespace aquarius
{
	namespace aoi
	{

		template <typename Session, std::size_t Step>
		class liner
		{
			using senty_type = std::list<std::shared_ptr<Session>>::iterator;

		public:
			struct element
			{
				std::shared_ptr<Session> self;

				senty_type slow_iter;
				senty_type fast_iter;
				senty_type self_iter;
			};

		public:
			std::vector<std::size_t> push(std::shared_ptr<Session> session)
			{
				auto& element = objects_[session->uid()];
				element.slow_iter = sessions_.begin();

				if (sessions_.size() < Step)
					element.fast_iter = sessions_.end();
				else
					std::advance(element.fast_iter, Step);

				auto position = session->position();

				bool interpret = false;

				for (auto iter = sessions_.begin(); iter != sessions_.end(); ++iter)
				{
					if (position < (*iter)->position())
					{
						element.self_iter = sessions_.insert(iter, session);

						interpret = true;
					}

					if (position - (*iter)->position() > Step)
					{
						element.slow_iter++;
					}

					element.fast_iter != sessions_.end() ? element.fast_iter++ : 0;

					if (interpret)
						break;
				}

				return calc_neighborhood(element);
			}

			std::vector<std::size_t> remove(std::size_t uid)
			{
				auto& element = objects_[uid];

				auto neighborhood = calc_neighborhood(element);

				sessions_.erase(element.self_iter);

				objects_.erase(uid);

				return neighborhood;
			}

			template <typename Message, typename... Args>
			auto move(std::size_t uid, Args... args)
			{
				auto v = calc_dir(args...);

				auto& element = objects_[uid];

				auto position = element.self->position();

				bool interpret = false;
				if (v > 0)
				{
					for (auto iter = element.session_iter; iter != sessions_.end(); ++iter)
					{
						if (position - (*iter)->get_step() > v)
						{
							element.session_iter = sessions_.insert(iter, session);
							interpret = true;
						}

						(*element.slow_iter)->send(pop_msg);
						element.slow_iter++;
						element.fast_iter != sessions_.end() ? element.fast_iter++ : 0;
						(*element.fast_iter)->send(put_msg);

						if (interpret)
							break;
					}
				}
				else
				{
					for (auto iter = element.session_iter; iter != sessions_.begin(); --iter)
					{
						if (session->get_step() - (*iter)->get_step() > step)
						{
							element.session_iter = sessions_.insert(iter, session);
							break;
						}

						(*element.slow_iter)->send(put_msg);
						(*element.fast_iter)->send(pop_msg);
						element.slow_iter != sessions_.begin() ? element.slow_iter-- : 0;
						element.fast_iter--;
					}
				}
			}

		private:
			std::vector<std::size_t> calc_neighborhood(const element& element)
			{
				std::vector<std::size_t> result;

				for (auto iter = element.slow_iter; iter != element.fast_iter; ++iter)
				{
					result.push_back(iter->uid());
				}

				return result;
			}

			template <typename... Args>
			float calc_dir(float x, Args...)
			{
				return x;
			}

		private:
			std::list<std::shared_ptr<Session>> sessions_;

			std::map<std::size_t, element> objects_;
		};

	} // namespace aoi
} // namespace aquarius