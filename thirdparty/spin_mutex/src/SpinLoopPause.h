// SPDX-License-Identifier: MIT

/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
#ifndef SPIN_LOOP_PAUSE_H_INCLUDED
#define SPIN_LOOP_PAUSE_H_INCLUDED

// Copyright (c) 2019 Maxim Egorushkin. MIT License. See the full licence in file LICENSE.

#include <atomic>

#if defined(__x86_64__) || defined(_M_X64) || \
    defined(__i386__) || defined(_M_IX86)
#include <emmintrin.h>
static inline void spin_loop_pause() noexcept {
    _mm_pause();
}
#elif defined(__arm__) || defined(__aarch64__)
static inline void spin_loop_pause() noexcept {
#if (defined(__ARM_ARCH_6K__) || \
     defined(__ARM_ARCH_6Z__) || \
     defined(__ARM_ARCH_6ZK__) || \
     defined(__ARM_ARCH_6T2__) || \
     defined(__ARM_ARCH_7__) || \
     defined(__ARM_ARCH_7A__) || \
     defined(__ARM_ARCH_7R__) || \
     defined(__ARM_ARCH_7M__) || \
     defined(__ARM_ARCH_7S__) || \
     defined(__ARM_ARCH_8A__) || \
     defined(__aarch64__))
    asm volatile ("yield" ::: "memory");
#else
    asm volatile ("nop" ::: "memory");
#endif
}
#else
#error "Unknown CPU architecture."
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPIN_LOOP_PAUSE_H_INCLUDED
