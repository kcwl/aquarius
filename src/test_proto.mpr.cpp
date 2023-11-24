#include <test_proto.mpr.h>


namespace xxx
{
	bool person::internal_from_binary(aquarius::flex_buffer_t& buffer)
	{
		if (!aquarius::from_binary(age, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(name, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(sex, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(money, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(back_money, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(crc, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(role_data, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(hp, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(mana, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(input_stream, buffer))
		{
			return false;
		}
		
		if (!aquarius::from_binary(output_stream, buffer))
		{
			return false;
		}
		
		return true;
	}
	bool person::internal_to_binary(aquarius::flex_buffer_t& buffer)
	{
		if (!aquarius::to_binary(age, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(name, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(sex, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(money, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(back_money, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(crc, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(role_data, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(hp, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(mana, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(input_stream, buffer))
		{
			return false;
		}
		
		if (!aquarius::to_binary(output_stream, buffer))
		{
			return false;
		}
		
		return true;
	}
}
