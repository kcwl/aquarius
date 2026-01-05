#pragma once
#include <boost/msm/back11/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <aquarius/logger.hpp>

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

			struct verify_complete_state : public boost::msm::front::state<>
			{
				// if the play event arrives in this state, defer it until a state handles it or
				// rejects it
				typedef boost::mpl::vector<verify_complete> deferred_events;
				// every (optional) entry/exit methods get the event passed.
				template <class Event, class FSM>
				void on_entry(Event const&, FSM&)
				{
					XLOG_DEBUG() << "entering: verify_complete_state" << std::endl;
				}
				template <class Event, class FSM>
				void on_exit(Event const&, FSM&)
				{
					XLOG_DEBUG() << "leaving: verify_complete_state" << std::endl;
				}
			};

			void inits(const init&);

			void verify_user_handler(const verify_user&);

			void verify_passwd_handler(const verify_passwd&);

			void verify_complete_handler(const verify_complete&);

			struct transition_table
				: boost::mpl::vector<
					  //    Start     Event         Next      Action                   Guard
					  //  +---------+-------------+---------+------------------------+----------------------+
					  a_row<init_state, init, verify_user_state, &player_define::inits>,
					  a_row<verify_user_state, verify_user, verify_passwd_state, &player_define::verify_user_handler>,
					  a_row<verify_passwd_state, verify_passwd, verify_complete_state,
							&player_define::verify_passwd_handler>,
					  a_row<verify_complete_state, verify_complete, verify_complete_state,
							&player_define::verify_complete_handler>>
			{};

			typedef boost::mpl::vector<init_state, verify_user_state> initial_state;

			template <class Event, class FSM>
			void no_transition(Event const& e, FSM&, int state)
			{
				std::cout << "no transition from state " << state << " on event " << typeid(e).name() << std::endl;
			}
		};

		class player : public boost::msm::back11::state_machine<player_define>
		{
		public:
			player() = default;
		};
	} // namespace gateway
} // namespace aquarius