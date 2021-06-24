/*
    FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

    Copyright (c) 2019 by HID Global, Inc. All rights reserved.

    DigitalPersona, FingerJet, and FingerJetFX are registered trademarks 
    or trademarks of DigitalPersona, Inc. in the United States and other
    countries.

    FingerJetFX OSE is open source software that you may modify and/or
    redistribute under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of the 
    License, or (at your option) any later version, provided that the 
    conditions specified in the COPYRIGHT.txt file provided with this 
    software are met.
 
    For more information, please visit digitalpersona.com/fingerjetfx.
*/ 

#include <stdlib.h>
#include <time.h>
#include <FRFXLL.h>

#ifdef WIN32
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _STLP_WCE
long _InterlockedCompareExchange(long volatile *, long, long);
#pragma intrinsic(_InterlockedCompareExchange)
# define InterlockedCompareExchange _InterlockedCompareExchange
#else
_STLP_IMPORT_DECLSPEC long _STLP_STDCALL InterlockedCompareExchange(long volatile *, long, long);
#endif

#ifdef __cplusplus
}
#endif
#endif

#undef HAS_SYNC_FUNCTIONS
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
# define HAS_SYNC_FUNCTIONS
#endif


static void * m_malloc(size_t size, void * _) {
  return malloc(size);
}
static void m_free(void * p, void * _) {
  free(p);
}

static unsigned int get_current_tick_in_ms() {
  return (unsigned int)(clock());
}

static long m_interlocked_increment(volatile long * pv) {
#ifdef WIN32
  return InterlockedIncrement(pv);
#elif defined(HAS_SYNC_FUNCTIONS)
  __sync_fetch_and_add(pv, 1L);
  return *pv;
#else
  return ++(*pv);
#endif
}

static long m_interlocked_decrement(volatile long * pv) {
#ifdef WIN32
  return InterlockedDecrement(pv);
#elif defined(HAS_SYNC_FUNCTIONS)
  __sync_fetch_and_sub(pv, 1L);
  return *pv;
#else
  return --(*pv);
#endif
}

static long m_interlocked_exchange(volatile long * pv, long val) {
#ifdef WIN32
  return InterlockedExchange(pv, val);
#elif defined(HAS_SYNC_FUNCTIONS)
  return __sync_lock_test_and_set(pv, val);
#else
  long old = *pv;
  *pv = val;
  return old;
#endif
}

static long m_interlocked_compare_exchange(volatile long * pv, long val, long cmp) {
#ifdef WIN32
  return InterlockedCompareExchange(pv, val, cmp);
#elif defined(HAS_SYNC_FUNCTIONS)
  return __sync_val_compare_and_swap(pv, cmp, val);
#else
  long old = *pv;
  if (*pv == cmp) *pv = val;
  return old;
#endif
}

FRFXLL_RESULT FRFXLLCreateLibraryContext(
  FRFXLL_HANDLE_PT phContext          ///< [out] pointer to where to put an open handle to created context
) {
  FRFXLL_CONTEXT_INIT ci = {sizeof(ci)};
  ci.malloc = &m_malloc;
  ci.free = &m_free;
  if (CLOCKS_PER_SEC == (clock_t)1000)
  {
    ci.get_current_tick_in_ms = &get_current_tick_in_ms;
  }
  ci.interlocked_increment = m_interlocked_increment;
  ci.interlocked_decrement = m_interlocked_decrement;
  ci.interlocked_exchange  = m_interlocked_exchange;
  ci.interlocked_compare_exchange = m_interlocked_compare_exchange;
  
// not sure this is cross compiler correct... (might need to remove)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  return FRFXLLCreateContext(&ci, phContext);
#pragma GCC diagnostic pop
}








