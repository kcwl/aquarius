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
