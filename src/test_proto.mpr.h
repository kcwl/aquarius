#pragma once
#include <aquarius/elastic.hpp>


/****************
*test_proto
*****************/
namespace xxx
{
	//message person that number is 10001
	class person final : public aquarius::message_lite
	{
	public:
		person() = default;
		virtual ~person() = default;
		
	private:
		virtual bool internal_from_binary(aquarius::flex_buffer_t& buffer) final;
		
		virtual bool internal_to_binary(aquarius::flex_buffer_t& buffer) final;
		
	public:
		int32_t age;		//����
		std::string name;	//����
		std::vector<bool> sex;
		int64_t money;	//Ǯ
		uint32_t back_money; //�ֿ����Ǯ
		uint64_t crc; //crc��֤��
		bytes role_data; //��������
		float hp; //����Ѫ��
		double mana; //��������
		fixed32_t input_stream; //������
		fixed64_t output_stream; //�����
	};
}
