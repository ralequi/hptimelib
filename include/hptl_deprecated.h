/************************************************
 * This API is deprecated. It remains only for compatibility
 ************************************************/
/**
  Language: C

  License: MIT License
  (c) HPCN 2014-2017
  (c) Naudit HPCN S.L. 2018-2025

  Author: Rafael Leira
  E-Mail: rafael.leira@naudit.es

  Description: Old header file for hptl

  Thread Safe: No

  Platform Dependencies: Linux-like and Intel Processor
*/
#ifndef __HPTLIB_DEPRECATED__H__
#define __HPTLIB_DEPRECATED__H__

/* This header remains for compatibility. Prefer including <hptl_legacy.h>. */
#if defined(__GNUC__) || defined(__clang__)
#pragma message("hptl_deprecated.h is deprecated; use hptl_legacy.h instead")
#endif

#include "hptl_legacy.h"

#endif
