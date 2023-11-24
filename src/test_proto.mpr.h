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
		int32_t age;		//����
		std::string name;	//����
		bool sex;
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
