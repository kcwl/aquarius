#include "cmd.virgo.h"

struct cmd_op_req_body::impl
{
	string command;
	std::vector<string> paramers;
};
cmd_op_req_body::cmd_op_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void cmd_op_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void cmd_op_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<cmd_op_req_body::impl>(buffer); 
}
string cmd_op_req_body::command() const
{
	return impl_ptr_->command;
}
string& cmd_op_req_body::command()
{
	return impl_ptr_->command;
}
std::vector<string> cmd_op_req_body::paramers() const
{
	return impl_ptr_->paramers;
}
std::vector<string>& cmd_op_req_body::paramers()
{
	return impl_ptr_->paramers;
}
struct cmd_op_resp_body::impl
{
	string output;
};
cmd_op_resp_body::cmd_op_resp_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void cmd_op_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void cmd_op_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<cmd_op_resp_body::impl>(buffer); 
}
string cmd_op_resp_body::output() const
{
	return impl_ptr_->output;
}
string& cmd_op_resp_body::output()
{
	return impl_ptr_->output;
}
