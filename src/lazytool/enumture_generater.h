#pragma once
#include "cpp_generater.h"

namespace aquarius
{
	namespace lazytool
	{
		namespace cpp
		{
            class enumture_generate : public generater
            {
                using generater::generate_error;

            public:
                enumture_generate() = default;

                virtual ~enumture_generate() = default;

            public:
                virtual generate_error visit(std::shared_ptr<parser> parser, std::ofstream& ofs_h,
                                             std::ofstream& ofs_cpp) override;
            };
		} // namespace cpp
	} // namespace lazytool
} // namespace aquarius