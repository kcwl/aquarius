#pragma once
#include <elastic.hpp>


/****************
*test_proto
*****************/
namespace xxx
{
	//message person that number is 10001
	class person final : public elastic::message_lite
	{
	public:
		person() = default;
		virtual ~person() = default;
		
	private:
		virtual bool internal_from_binary(elastic::flex_buffer_t& buffer) final;
		
		virtual bool internal_to_binary(elastic::flex_buffer_t& buffer) final;
		
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
