#pragma once
#if defined(WIN32) || defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0xA00
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif