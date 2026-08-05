#pragma once
#include <aquarius/logger.hpp>
#include <aquarius/module/player_module.hpp>
#include <boost/msm/back11/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace aquarius
{
	namespace gateway
	{
		struct init
		{};

		struct verify
		{};

		struct complete
		{};

		class player_define : public boost::msm::front::state_machine_def<player_define>
		{
		public:
			player_define() = default;

		public:
			template <class Event, class FSM>
			void on_entry(Event const&, FSM&)
			{
				XLOG_DEBUG() << "entering: Player" << std::endl;
			}
			template <class Event, class FSM>
			void on_exit(Event const&, FSM&)
			{
				XLOG_DEBUG() << "leaving: Player" << std::endl;
			}

			struct init_state : public boost::msm::front::state<>
			{
				// if the play event arrives in this state, defer it until a state handles it or
				// rejects it
				typedef boost::mpl::vector<init> deferred_events;
				// every (optional) entry/exit methods get the event passed.
				template <class Event, class FSM>
				void on_entry(Event const&, FSM&)
				{
					XLOG_DEBUG() << "entering: init_state" << std::endl;
				}
				template <class Event, class FSM>
				void on_exit(Event const&, FSM&)
				{
					XLOG_DEBUG() << "leaving: init_state" << std::endl;
				}
			};

			struct verify_state : public boost::msm::front::state<>
			{
				// if the play event arrives in this state, defer it until a state handles it or
				// rejects it
				typedef boost::mpl::vector<verify> deferred_events;
				// every (optional) entry/exit methods get the event passed.
				template <class Event, class FSM>
				void on_entry(Event const&, FSM&)
				{
					XLOG_DEBUG() << "entering: verify_state" << std::endl;
				}
				template <class Event, class FSM>
				void on_exit(Event const&, FSM&)
				{
					XLOG_DEBUG() << "leaving: verify_state" << std::endl;
				}
			};

			struct complete_state : public boost::msm::front::state<>
			{
				// if the play event arrives in this state, defer it until a state handles it or
				// rejects it
				typedef boost::mpl::vector<complete> deferred_events;
				// every (optional) entry/exit methods get the event passed.
				template <class Event, class FSM>
				void on_entry(Event const&, FSM&)
				{
					XLOG_DEBUG() << "entering: complete_state" << std::endl;
				}
				template <class Event, class FSM>
				void on_exit(Event const&, FSM&)
				{
					XLOG_DEBUG() << "leaving: complete_state" << std::endl;
				}
			};

			void inits(const init&);

			void verify_user_handler(const verify&);

			void verify_complete_handler(const complete&);

			struct transition_table
				: boost::mpl::vector<
					  //    Start     Event         Next      Action                   Guard
					  //  +---------+-------------+---------+------------------------+----------------------+
					  a_row<init_state, init, verify_state, &player_define::inits>,
					  a_row<verify_state, verify, complete_state, &player_define::verify_user_handler>,
					  a_row<complete_state, complete, complete_state, &player_define::verify_complete_handler>>
			{};

			typedef boost::mpl::vector<init_state, verify_state> initial_state;

			template <class Event, class FSM>
			void no_transition(Event const& e, FSM&, int state)
			{
				std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
			}
		};

		class player : public boost::msm::back11::state_machine<player_define>, public aquarius::player_base
		{
		public:
			player(std::size_t id);

		private:
			std::size_t id_;
		};
	} // namespace gateway
} // namespace aquarius