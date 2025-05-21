#pragma once
#include <array>
#include <map>
#include <memory>
#include <vector>

namespace aquarius
{
	namespace aoi
	{

		template <typename Session, std::size_t Width, std::size_t Height, std::size_t Step>
		class nine_grid
		{
		public:
			constexpr static std::size_t double_step = Step * 2;

			constexpr static std::size_t width = (Width + double_step) / double_step;

			constexpr static std::size_t height = (Height + double_step) / double_step;

			constexpr static std::array<std::pair<int, int>, 8> neighbors = { std::pair{ 0, 1 },  std::pair{ 1, 1 },
																			  std::pair{ 1, 0 },  std::pair{ 1, -1 },
																			  std::pair{ 0, -1 }, std::pair{ -1, -1 },
																			  std::pair{ -1, 0 }, std::pair{ -1, 1 } };

			struct element
			{
				std::size_t uid;
				std::size_t x;
				std::size_t y;
			};

			struct vec2
			{
				float x;
				float y;
			};

		public:
			nine_grid() = default;

		public:
			std::vector<std::size_t> enter(std::shared_ptr<Session> session_ptr)
			{
				auto& element = elements_[session_ptr->uid()];

				auto& [x, y] = session_ptr->position();

				element.x = (x + double_step) % double_step;
				element.y = (y + double_step) / double_step;

				auto index = element.y * width + element.x;

				grids_[index].push_back(session_ptr->uid());

				return calc_neighborhood(element);
			}

			std::vector<std::size_t> leave(std::size_t uid)
			{
				auto& element = elements_[uid];

				auto neighborhood = calc_neighborhood(element);

				elements_.erase(uid);

				const auto grid = grids_[element.y * width + element.x];

				std::erase_if(grid.begin(), grid.end(), [&](auto& id) { return id == uid; });

				return neighborhood;
			}

			template <typename... Args>
			auto move(std::size_t uid, Args... args) -> std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>
			{
				auto v = calc_dir(args...);

				auto x_real_step = (v.x + double_step) / double_step;
				auto y_real_step = (v.y + double_step) / double_step;

				if (x_real_step == 0 && y_real_step == 0)
					return {};

				auto& element = elements_[uid];

				auto slow_x = element.x - 1;
				auto fast_x = element.x + 1;

				auto slow_y = element.y - 1;
				auto fast_y = element.y + 1;

				std::set<std::size_t> add_exists{};
				std::set<std::size_t> remove_exists{};

				if (x_real_step > 0)
				{
					while (x_real_step--)
					{
						remove_exists.insert(slow_x + slow_y * double_step);
						slow_x++;
						fast_x++;
						add_exists.insert(fast_x + fast_y * double_step);
					}
				}
				else
				{
					x_real_step = -x_real_step;
					while (x_real_step--)
					{
						remove_exists.insert(fast_x + fast_y * double_step);
						fast_x--;
						slow_x--;
						add_exists.insert(slow_x + slow_y * double_step);
					}
				}

				element.x = slow_x + 1;

				if (y_real_step > 0)
				{
					while (y_real_step--)
					{
						remove_exists.insert(slow_x + slow_y * double_step);
						slow_y++;
						fast_y++;
						add_exists.insert(fast_x + fast_y * double_step);
					}
				}
				else
				{
					y_real_step = -y_real_step;
					while (y_real_step--)
					{
						remove_exists.insert(fast_x + fast_y * double_step);
						fast_y--;
						slow_y--;
						add_exists.insert(slow_x + slow_y * double_step);
					}
				}

				element.y = slow_y + 1;
				std::vector<std::size_t> put_vec;
				std::vector<std::size_t> pop_vec;
				for (auto& s : add_exists)
				{
					const auto& grid = grids_[s];

					std::copy(grid.begin(), grid.end(), std::back_inserter(put_vec));
				}

				for (auto& s : remove_exists)
				{
					const auto& grid = grids_[s];

					std::copy(grid.begin(), grid.end(), std::back_inserter(put_vec));
				}

				return { put_vec, pop_vec };
			}

		private:
			std::vector<std::size_t> calc_neighborhood(const element& element)
			{
				std::vector<std::size_t> neighborhood;

				for (auto& [dx, dy] : neighbors)
				{
					auto nx = element.x + dx;
					auto ny = element.y + dy;

					if (nx < 0 || nx >= width)
						continue;
					if (ny < 0 || ny >= height)
						continue;

					const auto& grid = grids_[ny * width + nx];

					std::copy(grid.begin(), grid.end(), std::back_inserter(neighborhood));
				}
			}

			template <typename... Args>
			vec2 calc_dir(float x, float y, Args...)
			{
				return vec2{ x, y };
			}

		private:
			std::map<int, std::vector<std::size_t>> grids_;

			std::map<std::size_t, element> elements_;
		};
	} // namespace aoi
} // namespace aquarius