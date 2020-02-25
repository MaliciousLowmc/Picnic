/*
 *  This file is part of the optimized implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mpc_lowmc.h"
#include "mzd_additional.h"

#if !defined(_MSC_VER)
#include <stdalign.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if defined(WITH_OPT)
#include "simd.h"
#endif

#define MPC_LOOP_CONST(function, result, first, second, sc)                                        \
  do {                                                                                             \
    for (unsigned int e = 0; e < (sc); ++e) {                                                      \
      function((result)[e], (first)[e], (second));                                                 \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_SHARED(function, result, first, second, sc)                                       \
  do {                                                                                             \
    for (unsigned int o = 0; o < (sc); ++o) {                                                      \
      function((result)[o], (first)[o], (second)[o]);                                              \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_SHARED_1(function, result, first, sc)                                             \
  do {                                                                                             \
    for (unsigned int o = 0; o < (sc); ++o) {                                                      \
      function((result)[o], (first)[o]);                                                           \
    }                                                                                              \
  } while (0)

#define MPC_LOOP_CONST_C_0(function, result, first, second, sc)                                    \
  function((result)[0], (first)[0], (second))

#define MPC_LOOP_CONST_C_ch(function, result, first, second, sc, c)                                \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      MPC_LOOP_CONST_C_0(function, result, first, second, sc);                                     \
    } else if ((c) == (sc)) {                                                                      \
      function((result)[(sc)-1], first[(sc)-1], (second));                                         \
    }                                                                                              \
  } while (0)

static void mpc_and_uint64(uint64_t* res, uint64_t const* first, uint64_t const* second,
                           uint64_t const* r, view_t* view, unsigned viewshift) {
  for (unsigned m = 0; m < SC_PROOF; ++m) {
    const unsigned j = (m + 1) % SC_PROOF;
    uint64_t tmp1    = second[m] ^ second[j];
    uint64_t tmp2    = first[j] & second[m];
    tmp1             = tmp1 & first[m];
    tmp1             = tmp1 ^ tmp2;
    tmp2             = r[m] ^ r[j];
    res[m] = tmp1 = tmp1 ^ tmp2;
    if (viewshift) {
      tmp1       = tmp1 >> viewshift;
      view->t[m] = view->t[m] ^ tmp1;
    } else {
      // on first call (viewshift == 0), view->t[0..2] == 0
      view->t[m] = tmp1;
    }
  }
}

static void mpc_and_verify_uint64(uint64_t* res, uint64_t const* first, uint64_t const* second,
                                  uint64_t const* r, view_t* view, uint64_t const mask,
                                  unsigned viewshift) {
  for (unsigned m = 0; m < (SC_VERIFY - 1); ++m) {
    const unsigned j = (m + 1);
    uint64_t tmp1    = second[m] ^ second[j];
    uint64_t tmp2    = first[j] & second[m];
    tmp1             = tmp1 & first[m];
    tmp1             = tmp1 ^ tmp2;
    tmp2             = r[m] ^ r[j];
    res[m] = tmp1 = tmp1 ^ tmp2;
    if (viewshift || m) {
      tmp1       = tmp1 >> viewshift;
      view->t[m] = view->t[m] ^ tmp1;
    } else {
      // on first call (viewshift == 0), view->t[0] == 0
      view->t[m] = tmp1;
    }
  }

  const uint64_t rsc = view->t[SC_VERIFY - 1] << viewshift;
  res[SC_VERIFY - 1] = rsc & mask;
}


static void mpc_sbox_layer_bitsliced_uint64_10(uint64_t* in, view_t* view, uint64_t const* rvec) {
  //bitsliced_step_1_uint64_10(SC_PROOF);

  //mpc_and_uint64(r0m, x0s, x1s, r2m, view, 0);
  //mpc_and_uint64(r2m, x1s, x2m, r1s, view, 1);
  //mpc_and_uint64(r1m, x0s, x2m, r0s, view, 2);

  //bitsliced_step_2_uint64_10(SC_PROOF - 1);
}

static void mpc_sbox_layer_bitsliced_verify_uint64_10(uint64_t* in, view_t* view,
                                                      uint64_t const* rvec) {
  //bitsliced_step_1_uint64_10(SC_VERIFY);

  //mpc_and_verify_uint64(r0m, x0s, x1s, r2m, view, MASK_X2I, 0);
  //mpc_and_verify_uint64(r2m, x1s, x2m, r1s, view, MASK_X2I, 1);
  //mpc_and_verify_uint64(r1m, x0s, x2m, r0s, view, MASK_X2I, 2);

  //bitsliced_step_2_uint64_10(SC_VERIFY);
}

#if defined(WITH_LOWMC_126_126_4)
#include "lowmc_126_126_4.h"
#endif
#if defined(WITH_LOWMC_192_192_4)
#include "lowmc_192_192_4.h"
#endif
#if defined(WITH_LOWMC_255_255_4)
#include "lowmc_255_255_4.h"
#endif

#define SBOX_uint64(sbox, y, x, views, r, n, shares, shares2)                                      \
  do {                                                                                             \
    uint64_t in[shares];                                                                           \
    for (unsigned int count = 0; count < shares; ++count) {                                        \
      in[count] = CONST_BLOCK(x[count], 0)->w64[(n) / (sizeof(word) * 8) - 1];                     \
    }                                                                                              \
    sbox(in, views, r);                                                                            \
    for (unsigned int count = 0; count < shares2; ++count) {                                       \
      memcpy(BLOCK(y[count], 0)->w64, CONST_BLOCK(x[count], 0)->w64,                               \
             ((n) / (sizeof(word) * 8) - 1) * sizeof(word));                                       \
      BLOCK(y[count], 0)->w64[(n) / (sizeof(word) * 8) - 1] = in[count];                           \
    }                                                                                              \
  } while (0)

#define R_uint64 const uint64_t* r = rvec[i].t

#if !defined(NO_UINT64_FALLBACK)
// uint64 based implementation
#include "lowmc_fns_uint64_L1.h"
#define SIGN mpc_lowmc_call_uint64_128
#define VERIFY mpc_lowmc_call_verify_uint64_128
#include "mpc_lowmc.c.i"

#include "lowmc_fns_uint64_L3.h"
#define SIGN mpc_lowmc_call_uint64_192
#define VERIFY mpc_lowmc_call_verify_uint64_192
#include "mpc_lowmc.c.i"

#include "lowmc_fns_uint64_L5.h"
#define SIGN mpc_lowmc_call_uint64_256
#define VERIFY mpc_lowmc_call_verify_uint64_256
#include "mpc_lowmc.c.i"
#endif

#if defined(WITH_OPT)
#if defined(WITH_SSE2) || defined(WITH_NEON)
#if defined(WITH_SSE2)
#define FN_ATTR ATTR_TARGET_SSE2
#endif

// L1 using SSE2/NEON
#include "lowmc_fns_s128_L1.h"
#define SIGN mpc_lowmc_call_s128_128
#define VERIFY mpc_lowmc_call_verify_s128_128
#include "mpc_lowmc.c.i"

// L3 using SSE2/NEON
#include "lowmc_fns_s128_L3.h"
#define SIGN mpc_lowmc_call_s128_192
#define VERIFY mpc_lowmc_call_verify_s128_192
#include "mpc_lowmc.c.i"

// L5 using SSE2/NEON
#include "lowmc_fns_s128_L5.h"
#define SIGN mpc_lowmc_call_s128_256
#define VERIFY mpc_lowmc_call_verify_s128_256
#include "mpc_lowmc.c.i"

#undef FN_ATTR
#endif

#if defined(WITH_AVX2)
#define FN_ATTR ATTR_TARGET_AVX2

// L1 using AVX2
#include "lowmc_fns_s256_L1.h"
#define SIGN mpc_lowmc_call_s256_128
#define VERIFY mpc_lowmc_call_verify_s256_128
#include "mpc_lowmc.c.i"

// L3 using AVX2
#include "lowmc_fns_s256_L3.h"
#define SIGN mpc_lowmc_call_s256_192
#define VERIFY mpc_lowmc_call_verify_s256_192
#include "mpc_lowmc.c.i"

// L5 using AVX2
#include "lowmc_fns_s256_L5.h"
#define SIGN mpc_lowmc_call_s256_256
#define VERIFY mpc_lowmc_call_verify_s256_256
#include "mpc_lowmc.c.i"

#undef FN_ATTR

#endif
#endif

zkbpp_lowmc_implementation_f get_zkbpp_lowmc_implementation(const lowmc_t* lowmc) {
  ASSUME(lowmc->m == 10);
  ASSUME(lowmc->n == 128 || lowmc->n == 192 || lowmc->n == 256);

#if defined(WITH_OPT)
#if defined(WITH_AVX2)
  if (CPU_SUPPORTS_AVX2) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
      case 128:
        return mpc_lowmc_call_s256_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
      case 192:
        return mpc_lowmc_call_s256_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
      case 256:
        return mpc_lowmc_call_s256_256_10;
#endif
      }
    }
  }
#endif
#if defined(WITH_SSE2) || defined(WITH_NEON)
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
      case 128:
        return mpc_lowmc_call_s128_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
      case 192:
        return mpc_lowmc_call_s128_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
      case 256:
        return mpc_lowmc_call_s128_256_10;
#endif
      }
    }
  }
#endif
#endif

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->m == 10) {
    switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
    case 128:
      return mpc_lowmc_call_uint64_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
    case 192:
      return mpc_lowmc_call_uint64_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
    case 256:
      return mpc_lowmc_call_uint64_256_10;
#endif
    }
  }

#endif

  return NULL;
}

zkbpp_lowmc_verify_implementation_f get_zkbpp_lowmc_verify_implementation(const lowmc_t* lowmc) {
  ASSUME(lowmc->m == 10);
  ASSUME(lowmc->n == 128 || lowmc->n == 192 || lowmc->n == 256);

#if defined(WITH_OPT)
#if defined(WITH_AVX2)
  if (CPU_SUPPORTS_AVX2) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
      case 128:
        return mpc_lowmc_call_verify_s256_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
      case 192:
        return mpc_lowmc_call_verify_s256_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
      case 256:
        return mpc_lowmc_call_verify_s256_256_10;
#endif
      }
    }
  }
#endif
#if defined(WITH_SSE2) || defined(WITH_NEON)
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
      case 128:
        return mpc_lowmc_call_verify_s128_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
      case 192:
        return mpc_lowmc_call_verify_s128_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
      case 256:
        return mpc_lowmc_call_verify_s128_256_10;
#endif
      }
    }
  }
#endif
#if defined(WITH_NEON)
  if (CPU_SUPPORTS_NEON) {
    if (lowmc->m == 10) {
      switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
      case 128:
        return mpc_lowmc_call_verify_s128_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
      case 192:
        return mpc_lowmc_call_verify_s128_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
      case 256:
        return mpc_lowmc_call_verify_s128_256_10;
#endif
      }
    }
  }
#endif
#endif

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->m == 10) {
    switch (lowmc->n) {
#if defined(WITH_LOWMC_126_126_4)
    case 128:
      return mpc_lowmc_call_verify_uint64_128_10;
#endif
#if defined(WITH_LOWMC_192_192_4)
    case 192:
      return mpc_lowmc_call_verify_uint64_192_10;
#endif
#if defined(WITH_LOWMC_255_255_4)
    case 256:
      return mpc_lowmc_call_verify_uint64_256_10;
#endif
    }
  }
#endif

  return NULL;
}

#if !defined(NO_UINT64_FALLBACK)
static void mzd_share_uint64_128(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_128(r, v1, v2);
  mzd_xor_uint64_128(r, r, v3);
}

static void mzd_share_uint64_192(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_192(r, v1, v2);
  mzd_xor_uint64_192(r, r, v3);
}

static void mzd_share_uint64_256(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                                 const mzd_local_t* v3) {
  mzd_xor_uint64_256(r, v1, v2);
  mzd_xor_uint64_256(r, r, v3);
}
#endif

#if defined(WITH_OPT)
#if defined(WITH_SSE2) || defined(WITH_NEON)
#if defined(WITH_SSE2)
#define FN_ATTR ATTR_TARGET_SSE2
#else
#define FN_ATTR
#endif

FN_ATTR
static void mzd_share_s128_128(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                               const mzd_local_t* v3) {
  mzd_xor_s128_128(r, v1, v2);
  mzd_xor_s128_128(r, r, v3);
}

FN_ATTR
static void mzd_share_s128_256(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                               const mzd_local_t* v3) {
  mzd_xor_s128_256(r, v1, v2);
  mzd_xor_s128_256(r, r, v3);
}

#undef FN_ATTR
#endif

#if defined(WITH_AVX2)
ATTR_TARGET_AVX2
static void mzd_share_s256_128(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                               const mzd_local_t* v3) {
  mzd_xor_s256_128(r, v1, v2);
  mzd_xor_s256_128(r, r, v3);
}

ATTR_TARGET_AVX2
static void mzd_share_s256_256(mzd_local_t* r, const mzd_local_t* v1, const mzd_local_t* v2,
                               const mzd_local_t* v3) {
  mzd_xor_s256_256(r, v1, v2);
  mzd_xor_s256_256(r, r, v3);
}
#endif
#endif

zkbpp_share_implementation_f get_zkbpp_share_implentation(const lowmc_t* lowmc) {
#if defined(WITH_OPT)
#if defined(WITH_AVX2)
  if (CPU_SUPPORTS_AVX2) {
    if (lowmc->n <= 128) {
      return mzd_share_s256_128;
    } else {
      return mzd_share_s256_256;
    }
  }
#endif
#if defined(WITH_SSE2) || defined(WITH_NEON)
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
    if (lowmc->n <= 128) {
      return mzd_share_s128_128;
    } else {
      return mzd_share_s128_256;
    }
  }
#endif
#endif

#if !defined(NO_UINT64_FALLBACK)
  if (lowmc->n <= 128) {
    return mzd_share_uint64_128;
  } else if (lowmc->n <= 192) {
    return mzd_share_uint64_192;
  } else {
    return mzd_share_uint64_256;
  }
#endif
}
