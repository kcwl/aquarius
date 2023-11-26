#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>

/****************
*test_proto
*****************/
namespace xxx
{
	//message person that number is 10001
	class person
	{
		friend class aquarius::access;
		
		template<typename _Archive>
		void serialize(_Archive& ar)
		{
			ar& age;
			ar& name;
			ar& sex;
			ar& money;
			ar& back_money;
			ar& crc;
			ar& role_data;
			ar& hp;
			ar& mana;
			ar& input_stream;
			ar& output_stream;
		}
		
	public:
		int32_t age;		//年龄
		std::string name;	//姓名
		bool sex;
		int64_t money;	//钱
		uint32_t back_money; //仓库里的钱
		uint64_t crc; //crc验证码
		bytes role_data; //人物数据
		float hp; //人物血量
		double mana; //人物蓝量
		fixed32_t input_stream; //输入流
		fixed64_t output_stream; //输出流
	};
}
