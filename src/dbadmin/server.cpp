#include "server.h"

namespace db
{
	server::server(uint16_t listen_port, int32_t pool_size,
				   const std::string& server_name, const std::string& db_addr, const std::string& db_user, const std::string& db_passwd)
		: base_type(listen_port, pool_size, server_name)
	{
		io_service_pool_.sql_set_params(db_addr, db_user, db_passwd);
	}


} // namespace db