#include "test_proto.mpr.h"


namespace xxx
{
	bool person::internal_from_binary(elastic::flex_buffer_t& buffer)
	{
		if (!elastic::from_binary(age, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(name, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(sex, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(money, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(back_money, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(crc, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(role_data, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(hp, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(mana, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(input_stream, buffer))
		{
			return false;
		}
		
		if (!elastic::from_binary(output_stream, buffer))
		{
			return false;
		}
		
		return true;
	}
	bool person::internal_to_binary(elastic::flex_buffer_t& buffer)
	{
		if (!elastic::to_binary(age, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(name, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(sex, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(money, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(back_money, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(crc, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(role_data, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(hp, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(mana, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(input_stream, buffer))
		{
			return false;
		}
		
		if (!elastic::to_binary(output_stream, buffer))
		{
			return false;
		}
		
		return true;
	}
}
