#pragma once


#pragma once

#define AQUARIUS_EXPAND(...) __VA_ARGS__

#define AQUARIUS_CAT1(a, b) a##_##b
#define AQUARIUS_CAT(a, b) AQUARIUS_CAT1(a, b)

#define AQUARIUS_RSEQ_N()                                                                                              \
	31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define AQUARIUS_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
					   _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...)                                       \
	N

#define AQUARIUS_NARG_(...) AQUARIUS_EXPAND(AQUARIUS_ARG_N(__VA_ARGS__))
#define AQUARIUS_NARG(...) AQUARIUS_EXPAND(AQUARIUS_NARG_(__VA_ARGS__, AQUARIUS_RSEQ_N()))

#define AQUARIUS_BIND_IMPL_1(param, element) param element
#define AQUARIUS_BIND_IMPL_2(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_1(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_3(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_2(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_4(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_3(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_5(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_4(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_6(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_5(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_7(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_6(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_8(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_7(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_9(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_8(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_10(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_9(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_11(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_10(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_12(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_11(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_13(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_12(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_14(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_13(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_15(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_14(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_16(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_15(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_17(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_16(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_18(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_17(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_19(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_18(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_20(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_19(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_21(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_20(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_22(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_21(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_23(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_22(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_24(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_23(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_25(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_24(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_26(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_25(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_27(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_26(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_28(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_27(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_29(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_28(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_30(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_29(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_31(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_30(param, __VA_ARGS__))
#define AQUARIUS_BIND_IMPL_32(param, element, ...) param element; AQUARIUS_EXPAND(AQUARIUS_BIND_IMPL_31(param, __VA_ARGS__))

#define AQUARIUS_BIND_WITH(param, ...)																			  \
	AQUARIUS_EXPAND(AQUARIUS_CAT(AQUARIUS_BIND_IMPL, AQUARIUS_NARG(__VA_ARGS__))(param, __VA_ARGS__))
