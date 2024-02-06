#pragma once
#include <aquarius.hpp>

namespace xxx
{
	struct person_body_request
	{
		bool sex;
		std::vector<uint8_t> role_data;
		double mana;
		float hp;
		int32_t age;
		int64_t money;
		std::string name;
		uint32_t back_money;
		uint64_t crc;
		
	private:
		friend class aquarius::access;
		
		template<typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& sex;
			ar& role_data;
			ar& mana;
			ar& hp;
			ar& age;
			ar& money;
			ar& name;
			ar& back_money;
			ar& crc;
		}
	};
	
	
	
	
	
	using person_request = aquarius::request<person_body_request, 10001>;
}
