#pragma once
#include <vector>
#include <tuple>
#include <map>

namespace aquarius
{

    template<std::size_t Width, std::size_t Height, std::size_t Step>
    struct non_zeor_coordinate
    {
        static auto calc_index(int x, int y) -> std::tuple<int, int>
        {
            if (x > Width || y > Height)
                return {};

            if (x < 0 || y < 0)
                return {};

            int temp_x = (x + Step) / Step;
            int temp_y = (y + Step) / Step;

            return  { temp_x, temp_y };
        }

        static std::vector<int> calc_nine_square(int index)
        {
            auto temp_x = index % Width;
            auto temp_y = index / Height;

            std::vector<int> result{};

            std::vector<std::pair<int, int>> dirs
            {
                {0, 0},
                {temp_x - 1, temp_y - 1},
                {temp_x - 1, temp_y},
                {temp_x - 1, temp_y + 1},
                {temp_x, temp_y - 1},
                {temp_x, temp_y + 1},
                {temp_x + 1, temp_y - 1},
                {temp_x + 1, temp_y},
                {temp_x + 1, temp_y + 1}
            };

            for (auto& dir : dirs)
            {
                if (dir.first < 0 || dir.second < 0)
                    continue;
                if (dir.first >= Width || dir.second >= Height)
                    continue;

                result.push_back(temp_y * Width + temp_x);
            }


            return result;
        }
    };

    template<typename Session, typename Cacl, std::size_t Width, std::size_t Height, std::size_t Step>
    class nine_square_grid
    {
        constexpr static std::size_t width_number = (Width + Step) / Step;

        constexpr static std::size_t height_number = (Height + Step) / Step;

        constexpr static std::size_t total = width_number * height_number;

        struct element
        {
            int slow_x;
            int slow_y;
            int x;
            int y;
            std::shared_ptr<Session> session_ptr;
        };

    public:
        void push(std::shared_ptr<Session> session_ptr)
        {
            auto& [x, y] = calc_index(session_ptr);

            int index = y * width_number + x;

            if (index == -1)
                return;

            auto& element = elements_[session_ptr->uid()];
            element.slow_x = x - 1;
            element.slow_y = y - 1;
            element.index = index;
            element.session_ptr = session_ptr;

            sessions_.push_back(session_ptr);
        }

        void pop(std::size_t uid)
        {
            sessions_.erase(uid);
            elements_.erase(uid);
        }

        template<typename Message>
        void move(std::shared_ptr<Session> session_ptr, Message put_msg, Message pop_msg, vec2 v)
        {
            auto x_real_step = (session_ptr->step().x - v.x + Step) / Step;
            auto y_real_step = (session_ptr->step().y - v.y + Step) / Step;

            if (x_real_step == 0 && y_real_step == 0)
                return;

            auto& element = elements_[session_ptr->uid()];

            std::set<std::size_t> add_exists{};
            std::set<std::size_t> remove_exists{};

            if (x_real_step > 0)
            {
                for (int i = 0; i < x_real_step; i++)
                {
                    add_exists.insert((element.slow_x + i + 3) + element.slow_y * width_number);
                    remove_exists.insert((element.slow_x + i) + element.slow_y * width_number);
                }
            }
            else
            {
                x_real_step = -x_real_step;
                for (int i = 0; i < x_real_step; i++)
                {
                    add_exists.insert((element.slow_x - i - 1) + element.slow_y * width_number);
                    remove_exists.insert((element.slow_x - i + 2) + element.slow_y * width_number);
                }
            }

            element.slow_x += x_real_step;

            if (y_real_step > 0)
            {
                for (int i = 0; i < y_real_step; i++)
                {
                    add_exists.insert((element.slow_x + i + 3) + element.slow_y * width_number);
                    remove_exists.insert((element.slow_x + i) + element.slow_y * width_number);
                }
            }
            else
            {
                y_real_step = -y_real_step;
                for (int i = 0; i < y_real_step; i++)
                {
                    add_exists.insert((element.slow_x - i - 1) + element.slow_y * width_number);
                    remove_exists.insert((element.slow_x - i + 2) + element.slow_y * width_number);
                }
            }

            element.slow_y += y_real_step;

            element.x += x_real_step;
            element.y += y_real_step;

            for (auto& s : add_exists)
            {
                for (auto& ss : sessions_[s])
                {
                    ss->send(put_msg);
                }
            }

            for (auto& s : remove_exists)
            {
                for (auto& ss : sessions_[s])
                {
                    ss->send(pop_msg);
                }
            }
        }

        template<typename Message>
        void broadcast(int index, Message msg)
        {
            auto indexs = Cacl::calc_nine_square(index);

            for (auto& index : indexs)
            {
                auto iter = grids_.find(index);

                if (iter == grids_.end())
                    continue;

                for (auto& [_, session_ptr] : iter->second.sessions_)
                {
                    session_ptr->send(msg);
                }
            }
        }

    private:
        int calc_index(std::shared_ptr<Session> session_ptr)
        {
            auto& [x, y] = session_ptr->position();

            return Cacl::calc_index<Width, Height, Step>(x, y);
        }

    private:
        std::map<std::size_t, std::shared_ptr<Session>> sessions_;

        std::map<std::size_t, element> elements_;
    };
}