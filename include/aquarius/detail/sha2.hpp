#pragma once
#include <array>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
	struct prime
	{};
	struct k
	{};

	template <typename _F, std::size_t I, std::size_t N>
	struct integer_traits;

	template <std::size_t I, std::size_t N>
	struct integer_traits<prime, I, N>
	{
		constexpr static auto suffix = I;

		static constexpr std::array<uint32_t, 8> value = { static_cast<uint32_t>(0xc6a09e667u),
														   0xbb67ae85u,
														   0x3c6ef372u,
														   0xa54ff53au,
														   0x510e527fu,
														   0x9b05688cu,
														   0x1f83d9abu,
														   0x5be0cd19u };
	};

	template <>
	struct integer_traits<prime, 384, 8>
	{
		constexpr static auto suffix = 384;

		static constexpr std::array<uint64_t, 8> value = { 0xcbbb9d5dc1059ed8ull, 0x629a292a367cd507ull,
														   0x9159015a3070dd17ull, 0x152fecd8f70e5939ull,
														   0x67332667ffc00b31ull, 0x8eb44a8768581511ull,
														   0xdb0c2e0d64f98fa7ull, 0x47b5481dbefa4fa4ull };
	};

	template <>
	struct integer_traits<prime, 512, 8>
	{
		constexpr static auto suffix = 512;

		static constexpr std::array<uint64_t, 8> value = { 0x6a09e667f3bcc908ull, 0xbb67ae8584caa73bull,
														   0x3c6ef372fe94f82bull, 0xa54ff53a5f1d36f1ull,
														   0x510e527fade682d1ull, 0x9b05688c2b3e6c1full,
														   0x1f83d9abfb41bd6bull, 0x5be0cd19137e2179ull };
	};

	template <std::size_t I, std::size_t N>
	struct integer_traits<k, I, N>
	{
		constexpr static auto suffix = I;

		static constexpr std::array<uint32_t, 64> value = {
			0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
		};
	};

	template <>
	struct integer_traits<k, 384, 80>
	{
		constexpr static auto suffix = 384;

		static constexpr std::array<uint64_t, 80> value = {
			0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
			0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
			0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
			0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
			0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
			0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
			0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
			0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
			0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
			0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
			0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
			0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
			0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
			0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
			0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
			0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
			0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
			0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
			0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
			0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
		};
	};

	using prime256 = integer_traits<prime, 256, 8>;
	using prime384 = integer_traits<prime, 384, 8>;
	using prime512 = integer_traits<prime, 512, 8>;
	using k256 = integer_traits<k, 256, 64>;
	using k384 = integer_traits<k, 384, 80>;
	using k512 = integer_traits<k, 384, 80>;

	template <typename _Ty, typename... _Args>
	inline constexpr _Ty xor_s(_Ty x, _Args&&... args)
	{
		return ((x ^= args), ...);
	}

	template <typename _Ty>
	inline constexpr static _Ty rotr(_Ty x, _Ty n)
	{
		return (x >> n) | x << ((sizeof(x) << 3) - n);
	}

	template <typename _Ty>
	inline constexpr static _Ty ch(_Ty x, _Ty y, _Ty z)
	{
		return xor_s<_Ty>((x & y), (~x & z));
	}

	template <typename _Ty>
	inline constexpr static _Ty maj(_Ty x, _Ty y, _Ty z)
	{
		return xor_s<_Ty>((x & y), (x & z), (y & z));
	}

	template <typename _Ty, typename... _Args>
	inline constexpr static _Ty rotr_s(_Ty x, _Args&&... args)
	{
		return (rotr<_Ty>(x, std::forward<_Args>(args)) ^ ...);
	}

	template <typename _Ty, std::size_t N>
	struct logic_sig;

	template <typename _Ty>
	struct logic_sig<_Ty, 256>
	{
		static constexpr _Ty ssig0(_Ty x) { return rotr_s<_Ty>(x, 2, 13, 22); }

		static constexpr _Ty ssig1(_Ty x) { return rotr_s<_Ty>(x, 6, 11, 25); }

		static constexpr _Ty sig0(_Ty x) { return rotr_s<_Ty>(x, 7, 18) ^ (x >> 3); }

		static constexpr _Ty sig1(_Ty x) { return rotr_s<_Ty>(x, 17, 19) ^ (x >> 10); }
	};

	template <typename _Ty>
	struct logic_sig<_Ty, 384>
	{
		static constexpr _Ty ssig0(_Ty x) { return rotr_s<_Ty>(x, 28, 34, 39); }

		static constexpr _Ty ssig1(_Ty x) { return rotr_s<_Ty>(x, 14, 18, 41); }

		static constexpr _Ty sig0(_Ty x) { return rotr_s<_Ty>(x, 1, 8) ^ (x >> 7); }

		static constexpr _Ty sig1(_Ty x) { return rotr_s<_Ty>(x, 19, 61) ^ (x >> 6); }
	};

	template <typename _Ty>
	struct logic_sig<_Ty, 512>
	{
		static constexpr _Ty ssig0(_Ty x) { return rotr_s<_Ty>(x, 28, 34, 39); }

		static constexpr _Ty ssig1(_Ty x) { return rotr_s<_Ty>(x, 14, 18, 41); }

		static constexpr _Ty sig0(_Ty x) { return rotr_s<_Ty>(x, 1, 8) ^ (x >> 7); }

		static constexpr _Ty sig1(_Ty x) { return rotr_s<_Ty>(x, 19, 61) ^ (x >> 6); }
	};

	template <std::size_t N>
	struct complement;

	template <>
	struct complement<256>
	{
		static constexpr std::size_t comp = 448;
		static constexpr std::size_t end = 64;
		static constexpr std::size_t message_block_size = 512;
	};

	template <>
	struct complement<384>
	{
		static constexpr std::size_t comp = 896;
		static constexpr std::size_t end = 128;
		static constexpr std::size_t message_block_size = 1024;
	};

	template <std::size_t N>
	inline static constexpr std::size_t sequence_length = (complement<N>::message_block_size / 32);

	inline static constexpr std::size_t message_schedule_length = 64;
	inline static constexpr std::size_t char_len_bits = 8;
	inline static constexpr std::size_t working_varlen = 8;
} // namespace

namespace sha2
{
	namespace detail
	{
		template <typename _Ty>
		using array2 = std::vector<std::vector<_Ty>>;

		template <typename _Base, typename _Prime, typename _K>
		class sha
		{
			constexpr static auto prime_value = _Prime::value;

			constexpr static auto k_vlaue = _K::value;

			using h_t = std::array<_Base, char_len_bits>;

		public:
			std::string hash(const std::string& input)
			{
				std::size_t nbuffer{};

				h_t h{};

				array2<_Base> buffer = preprocess((uint8_t*)input.c_str(), nbuffer);
				process(buffer, nbuffer, h);

				return digest(h);
			}

		private:
			std::string digest(h_t h)
			{
				std::stringstream ss{};

				for (auto& v : h)
				{
					ss << std::hex << std::setw(8) << std::setfill('0') << v;
				}

				return ss.str();
			}

			array2<_Base> preprocess(const uint8_t* input, std::size_t& n)
			{
				std::size_t len = strlen((char*)input);
				std::size_t l = len * char_len_bits;

				std::size_t k = 0;

				k = (complement<_Prime::suffix>::comp - 1 - l) % complement<_Prime::suffix>::message_block_size;
				n = (l + 1 + k + complement<_Prime::suffix>::end) / complement<_Prime::suffix>::message_block_size;

				array2<_Base> buffer{ n };

				for (auto& v : buffer)
				{
					v.resize(sequence_length<_Prime::suffix>);
				}

				_Base in{};
				std::size_t index{};

				for (std::size_t i = 0; i < n; ++i)
				{
					for (std::size_t j = 0; j < sequence_length<_Prime::suffix>; ++j)
					{
						in = 0;

						for (std::size_t k = 0; k < 4; ++k)
						{
							index = i * 64 + j * 4 + k;
							if (index < len)
							{
								in = in << 8 | static_cast<_Base>(input[index]);
							}
							else if (index == len)
							{
								in = in << 8 | 0x80;
							}
							else
							{
								in = in << 8 | 0;
							}
						}

						buffer[i][j] = in;
					}
				}

				buffer[n - 1][sequence_length<_Prime::suffix> - 1] = static_cast<_Base>(l);
				buffer[n - 1][sequence_length<_Prime::suffix> - 2] = 0;

				return buffer;
			}

			void process(array2<_Base> buffer, std::size_t n, h_t& h)
			{
				std::array<_Base, working_varlen> s{};
				std::array<_Base, message_schedule_length> w{};

				std::copy(prime_value.begin(), prime_value.end(), h.begin());

				for (auto& v : buffer)
				{
					std::copy(v.begin(), v.end(), w.begin());

					for (std::size_t j = 16; j < message_schedule_length; j++)
					{
						w[j] = w[j - 16] + logic_sig<_Base, _Prime::suffix>::sig0(w[j - 15]) + w[j - 7] +
							   logic_sig<_Base, _Prime::suffix>::sig1(w[j - 2]);
					}

					std::copy(h.begin(), h.end(), s.begin());

					for (std::size_t j = 0; j < message_schedule_length; ++j)
					{
						_Base tmp1 = s[7] + logic_sig<_Base, _Prime::suffix>::ssig1(s[4]) +
									 ch<_Base>(s[4], s[5], s[6]) + k_vlaue[j] + w[j];
						_Base tmp2 = logic_sig<_Base, _Prime::suffix>::ssig0(s[0]) + maj(s[0], s[1], s[2]);

						s[7] = s[6];
						s[6] = s[5];
						s[5] = s[4];
						s[4] = s[3] + tmp1;
						s[3] = s[2];
						s[2] = s[1];
						s[1] = s[0];
						s[0] = tmp1 + tmp2;
					}

					for (std::size_t j = 0; j < working_varlen; ++j)
					{
						h[j] += s[j];
					}
				}
			}
		};
	} // namespace detail

	using sha256 = detail::sha<uint32_t, prime256, k256>;
	using sha384 = detail::sha<uint64_t, prime384, k384>;
	using sha512 = detail::sha<uint64_t, prime512, k512>;
} // namespace sha2