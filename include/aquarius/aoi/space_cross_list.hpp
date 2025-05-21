#pragma once
#include <map>
#include <memory>
#include <vector>

namespace aquarius
{
	namespace aoi
	{
		template <typename Session, std::size_t Step>
		class space_cross_list
		{
			constexpr static std::size_t double_step = Step * 2;

			using senty_iterator_type = typename std::list<std::shared_ptr<Session>>::iterator;

			struct senty_type
			{
				senty_iterator_type slow_iter;
				senty_iterator_type fast_iter;
				senty_iterator_type self_iter;
			};

			struct element_type
			{
				std::shared_ptr<Session> session_ptr;

				senty_iterator_type x;

				senty_iterator_type y;

				senty_iterator_type z;
			};

			struct vec3
			{
				float x;
				float y;
				float z;
			};

		public:
			std::vector<std::size_t> enter(std::shared_ptr<Session> session_ptr)
			{
				auto& element = elements_[session_ptr->uid()];

				element.x.slow_iter = x_list_.begin();

				x_list_.size() > double_step ? std::advance(element.x.fast_iter, double_step)
											 : element.x.fast_iter = x_list_.end();

				element.y.slow_iter = y_list_.begin();

				y_list_.size() > double_step ? std::advance(element.y.fast_iter, double_step)
											 : element.y.fast_iter = y_list_.end();

				element.z.slow_iter = z_list_.begin();

				z_list_.size() > double_step ? std::advance(element.z.fast_iter, double_step)
											 : element.z.fast_iter = z_list_.end();

				auto position = session_ptr->position();

				for (auto iter = x_list_.begin(); iter != x_list_.end(); ++iter)
				{
					auto cur = elements_[*iter];

					if (!cur)
						continue;

					auto temp = position.x - cur.x;

					if (temp < Step)
					{
						element.x.self_iter = x_list_.insert(iter, session_ptr);
						break;
					}

					element.x.slow_iter++;

					element.x.fast_iter != x_list_.end() ? element.x.fast_iter++ : 0;
				}

				for (auto iter = y_list_.begin(); iter != y_list_.end(); ++iter)
				{
					auto cur = elements_[*iter];

					if (!cur)
						continue;

					auto temp = position.y - cur.position().y;

					if (temp < Step)
					{
						element.y.self_iter = y_list_.insert(iter, session_ptr);

						break;
					}

					element.y.slow_iter++;
					element.y.fast_iter != y_list_.end() ? element.y.fast_iter++ : 0;
				}

				for (auto iter = z_list_.begin(); iter != z_list_.end(); ++iter)
				{
					auto cur = elements_[*iter];

					if (!cur)
						continue;

					auto temp = position.z - cur.position().z;

					if (temp < Step)
					{
						element.z.self_iter = z_list_.insert(iter, session_ptr);

						break;
					}

					element.z.slow_iter++;
					element.z.fast_iter != z_list_.end() ? element.z.fast_iter++ : 0;
				}

				return calc_neighbor(element);
			}

			std::vector<std::size_t> leave(std::size_t uid)
			{
				auto& element = elements_[uid];

				auto neighbor = calc_neighbor(element);

				x_list_.erase(element.x.self_iter);
				y_list_.erase(element.y.self_iter);
				z_list_.erase(element.z.self_iter);

				elements_.erase(uid);

				return neighbor;
			}

			template <typename Message, typename... Args>
			auto move(std::size_t uid, Args... args) -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>
			{
				auto v = calc_dir(args...);

				auto& element = elements_[uid];

				auto position = element.session_ptr->position();

				std::map<std::size_t, int> add_exists{};
				std::map<std::size_t, int> remove_exists{};

				bool interpret = false;

				if (v.x > 0)
				{
					for (auto iter = element.x.self_iter; iter != x_list_.end(); ++iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.x - cur->position().x > Step)
						{
							element.x.self_iter = x_list_.insert(iter, uid);
							interpret = true;
						}

						remove_exists[element.x_slow_iter->session_ptr.get()]++;
						element.x_slow_iter++;
						element.x_fast_iter != x_list_.end() ? element.x_fast_iter++ : 0;
						add_exists[element.x_fast_iter->session_ptr.get()]++;

						if (interpret)
							break;
					}
				}
				else
				{
					for (auto iter = element.x_session_iter; iter != x_list_.begin(); --iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.x - cur->session_ptr->position().x > Step)
						{
							element.x.self_iter = x_list_.insert(iter, uid);
							interpret = true;
						}

						element.x.slow_iter != x_list_.begin() ? element.x.slow_iter-- : 0;
						add_exists[element.x.slow_iter->session_ptr.get()]++;
						remove_exists[element.x.fast_iter->session_ptr.get()]++;
						element.x.fast_iter--;

						if (interpret)
							break;
					}
				}

				interpret = false;

				if (v.y > 0)
				{
					for (auto iter = element.y.self_iter; iter != y_list_.end(); ++iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.y - cur->session_ptr->position().y > Step)
						{
							element.y.self_iter = y_list_.insert(iter, uid);
							interpret = true;
						}

						remove_exists[element.y_slow_iter->session_ptr.get()]++;
						element.y_slow_iter++;
						element.y_fast_iter != y_list_.end() ? element.y_fast_iter++ : 0;
						add_exists[element.y_fast_iter->session_ptr.get()]++;

						if (interpret)
							break;
					}
				}
				else
				{
					for (auto iter = element.y_session_iter; iter != y_list_.begin(); --iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.y - cur.session_ptr->position().y > Step)
						{
							element.y.self_iter = y_list_.insert(iter, uid);
							interpret = true;
						}

						element.y.slow_iter != y_list_.begin() ? element.y.slow_iter-- : 0;
						add_exists[element.y.slow_iter->session_ptr.get()]++;
						remove_exists[element.y.fast_iter->session_ptr.get()]++;
						element.y_fast_iter--;

						if (interpret)
							break;
					}
				}

				interpret = false;

				if (v.z > 0)
				{
					for (auto iter = element.z.self_iter; iter != z_list_.end(); ++iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.z - cur->session_ptr->position().z > Step)
						{
							element.z.self_iter = z_list_.insert(iter, uid);
							interpret = true;
						}

						remove_exists[element.z.slow_iter->session_ptr.get()]++;
						element.z.slow_iter++;
						element.z.fast_iter != z_list_.end() ? element.z.fast_iter++ : 0;
						add_exists[element.z.fast_iter->session_ptr.get()]++;

						if (interpret)
							break;
					}
				}
				else
				{
					for (auto iter = element.z.session_iter; iter != z_list_.begin(); --iter)
					{
						auto& cur = elements_[*iter];
						if (!cur)
							continue;

						if (position.z - cur.session_ptr->position().z > Step)
						{
							element.z.self_iter = z_list_.insert(iter, uid);
							interpret = true;
						}

						element.z.slow_iter != z_list_.begin() ? element.z.slow_iter-- : 0;
						add_exists[element.z.slow_iter->session_ptr.get()]++;
						remove_exists[element.z.fast_iter->session_ptr.get()]++;
						element.z_fast_iter--;

						if (interpret)
							break;
					}
				}

				std::vector<std::size_t> put_vec{};
				std::vector<std::size_t> pop_vec{};

				for (auto& s : remove_exists)
				{
					if (s.second != 3)
						continue;

					pop_vec.push_back(s.first);
				}

				for (auto& s : add_exists)
				{
					if (s.second != 3)
						continue;

					put_vec.push_back(s.first);
				}

				return { put_vec, pop_vec };
			}

		private:
			std::vector<std::size_t> calc_neighbor(const element_type& element)
			{
				std::vector<std::size_t> result;

				std::map<std::size_t, int> exists{};

				for (auto iter = element.x.slow_iter; iter != element.y.fast_iter; ++iter)
				{
					exists[*iter] = 0;
				}

				for (auto iter = element.y.slow_iter; iter != element.y.fast_iter; ++iter)
				{
					exists[*iter]++;
				}

				for (auto iter = element.z.slow_iter; iter != element.z.fast_iter; ++iter)
				{
					exists[*iter]++;
				}

				for (auto& s : exists)
				{
					if (s.second != 2)
						continue;

					result.push_back(s.first);
				}

				return result;
			}

			template <typename... Args>
			vec3 calc_dir(float x, float y, float z, Args...)
			{
				return vec3{ x, y, z };
			}

		private:
			std::list<std::size_t> x_list_;

			std::list<std::size_t> y_list_;

			std::list<std::size_t> z_list_;

			std::map<std::size_t, element_type> elements_;
		};
	} // namespace aoi
} // namespace aquarius