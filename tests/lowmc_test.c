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

#include "../io.h"
#include "../lowmc.h"
#include "../picnic_impl.h"

#include <inttypes.h>
#include <stdint.h>

#include "utils.h"

static int lowmc_enc(const picnic_params_t param, const uint8_t* key, const uint8_t* plaintext,
                     const uint8_t* expected) {
  const picnic_instance_t* pp = picnic_instance_get(param);
  if (!pp) {
    return -1;
  }
  const lowmc_t* lowmc = pp->lowmc;

  mzd_local_t* sk  = mzd_local_init(1, lowmc->k);
  mzd_local_t* pt  = mzd_local_init(1, lowmc->n);
  mzd_local_t* ct  = mzd_local_init(1, lowmc->n);
  mzd_local_t* ctr = mzd_local_init(1, lowmc->n);

  mzd_from_char_array(sk, key, pp->input_size);
  mzd_from_char_array(pt, plaintext, pp->output_size);
  mzd_from_char_array(ct, expected, pp->output_size);

  pp->impls.lowmc(sk, pt, ctr);

  int ret = 0;
  if (!mzd_local_equal(ctr, ct, 1, lowmc->n)) {
    ret = 2;
  }

  mzd_local_free(ctr);
  mzd_local_free(ct);
  mzd_local_free(pt);
  mzd_local_free(sk);

  return ret;
}


static int LowMC_test_vectorL1_1(void) {
  const uint8_t key[16] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x0E, 0x30, 0x72, 0x0B, 0x9F, 0x64, 0xD5, 0xC2,
                                           0xA7, 0x77, 0x1C, 0x8C, 0x23, 0x8D, 0x8F, 0x70};

  return lowmc_enc(Picnic_L1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL1_2(void) {
  const uint8_t key[16] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x0E, 0x59, 0x61, 0xE9, 0x99, 0x21, 0x53, 0xB1,
                                           0x32, 0x45, 0xAF, 0x24, 0x3D, 0xD7, 0xDD, 0xC0};

  return lowmc_enc(Picnic_L1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL1_3(void) {
  const uint8_t key[16] = {0x08, 0x4c, 0x2a, 0x6e, 0x19, 0x5d, 0x3b, 0x7f,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xf7, 0xb3, 0xd5, 0x91, 0xe6, 0xa2, 0xc4, 0x80,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x91, 0x5c, 0x63, 0x21, 0xd7, 0x86, 0x46, 0xb6,
                                           0xc7, 0x65, 0x43, 0xff, 0xb8, 0x52, 0x3b, 0x4d};

  return lowmc_enc(Picnic_L1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_1(void) {
  const uint8_t key[24] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0xA8, 0x5B, 0x82, 0x44, 0x34, 0x4A, 0x2E, 0x1B,
                                           0x10, 0xA1, 0x7B, 0xAB, 0x04, 0x30, 0x73, 0xF6,
                                           0xBB, 0x64, 0x9A, 0xE6, 0xAF, 0x65, 0x9F, 0x6F};

  return lowmc_enc(Picnic_L3_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_2(void) {
  const uint8_t key[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0x21, 0x0B, 0xBC, 0x4A, 0x43, 0x4B, 0x32, 0xDB,
                                           0x1E, 0x85, 0xAE, 0x7A, 0x27, 0xFE, 0xE9, 0xE4,
                                           0x15, 0x82, 0xFA, 0xC2, 0x1D, 0x03, 0x5A, 0xA1};

  return lowmc_enc(Picnic_L3_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_3(void) {
  const uint8_t key[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0xE4, 0x82, 0xBC, 0xF9, 0xAD, 0x2C, 0x04, 0x48,
                                           0x31, 0x48, 0xD4, 0x6F, 0xBE, 0x1F, 0x8B, 0x51,
                                           0x46, 0x0D, 0xCC, 0x3E, 0x8E, 0xFB, 0x31, 0x01};

  return lowmc_enc(Picnic_L3_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_1(void) {
  const uint8_t key[32] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0xB8, 0xF2, 0x0A, 0x88, 0x8A, 0x0A, 0x9E, 0xC4,
                                           0xE4, 0x95, 0xF1, 0xFB, 0x43, 0x9A, 0xBD, 0xDE,
                                           0x18, 0xC1, 0xD3, 0xD2, 0x9C, 0xF2, 0x0D, 0xF4,
                                           0xB1, 0x0A, 0x56, 0x7A, 0xA0, 0x2C, 0x72, 0x67};

  return lowmc_enc(Picnic_L5_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_2(void) {
  const uint8_t key[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0xEE, 0xEC, 0xCE, 0x6A, 0x58, 0x4A, 0x93, 0x30,
                                           0x6D, 0xAE, 0xA0, 0x75, 0x19, 0xB4, 0x7A, 0xD6,
                                           0x40, 0x2C, 0x11, 0xDD, 0x94, 0x2A, 0xA3, 0x16,
                                           0x65, 0x41, 0x44, 0x49, 0x77, 0xA2, 0x14, 0xC5};

  return lowmc_enc(Picnic_L5_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_3(void) {
  const uint8_t key[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0x03, 0x37, 0x33, 0x26, 0xC0, 0xF5, 0x0E, 0x3B,
                                           0x6B, 0x2E, 0x1C, 0xE8, 0xF9, 0x43, 0x0F, 0xF5,
                                           0xEB, 0x0E, 0xC3, 0x45, 0xC7, 0x27, 0xA4, 0x74,
                                           0x8F, 0xCF, 0x73, 0x17, 0x9D, 0x48, 0xE7, 0x9B};

  return lowmc_enc(Picnic_L5_FS, key, plaintext, ciphertext_expected);
}

#if defined(WITH_LOWMC_M1)
static int LowMC_test_vectorL1_M1_1(void) {
  const uint8_t key[16] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x5C, 0xB8, 0xDA, 0x74, 0x60, 0x01, 0x9E, 0x41,
                                           0x23, 0x08, 0x86, 0xCB, 0xCA, 0xF3, 0xEB, 0xA2};

  return lowmc_enc(Picnic_L1_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL1_M1_2(void) {
  const uint8_t key[16] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x56, 0x3C, 0xC9, 0x08, 0x4A, 0x86, 0xD6, 0x7E,
                                           0x7F, 0x24, 0x59, 0x55, 0x40, 0x1F, 0x42, 0x95};

  return lowmc_enc(Picnic_L1_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL1_M1_3(void) {
  const uint8_t key[16] = {0x08, 0x4c, 0x2a, 0x6e, 0x19, 0x5d, 0x3b, 0x7f,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xf7, 0xb3, 0xd5, 0x91, 0xe6, 0xa2, 0xc4, 0x80,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x85, 0x15, 0x48, 0xC9, 0xF2, 0x3F, 0x5D, 0x7F,
                                           0x44, 0x0F, 0x6F, 0x1F, 0x42, 0x6C, 0xF8, 0x48};

  return lowmc_enc(Picnic_L1_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_M1_1(void) {
  const uint8_t key[24] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0xD6, 0x53, 0xAD, 0xE0, 0x15, 0x0C, 0x83, 0x1B,
                                           0xF0, 0xD9, 0x9E, 0x65, 0x2C, 0xC5, 0x6C, 0x37,
                                           0x3F, 0xC3, 0x65, 0xC1, 0x70, 0xA4, 0x73, 0xB3};

  return lowmc_enc(Picnic_L3_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_M1_2(void) {
  const uint8_t key[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0x92, 0x17, 0x7B, 0xAA, 0x8D, 0x87, 0xCD, 0xED,
                                           0x57, 0x91, 0xBF, 0x3D, 0xA7, 0x2C, 0x73, 0x07,
                                           0x9A, 0xD2, 0xFB, 0xD2, 0x39, 0xD0, 0x2C, 0x4A};

  return lowmc_enc(Picnic_L3_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL3_M1_3(void) {
  const uint8_t key[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0x3B, 0x82, 0xEC, 0xDF, 0x1B, 0xD9, 0x19, 0x45,
                                           0x95, 0xD8, 0xF9, 0x4A, 0x2B, 0xBF, 0xB2, 0x9D,
                                           0xA0, 0xF6, 0x10, 0x77, 0xF6, 0x7E, 0x5D, 0xA1};

  return lowmc_enc(Picnic_L3_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_M1_1(void) {
  const uint8_t key[32] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0x2C, 0x2F, 0xA7, 0xB1, 0xCF, 0x1D, 0xE4, 0x75,
                                           0x7C, 0x4B, 0x77, 0x0E, 0x04, 0xBD, 0x2F, 0xD0,
                                           0x56, 0x3D, 0x3C, 0x2F, 0xC1, 0xE6, 0x0C, 0x7D,
                                           0x08, 0xEB, 0x9E, 0xB2, 0xB9, 0x56, 0xA4, 0x44};

  return lowmc_enc(Picnic_L5_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_M1_2(void) {
  const uint8_t key[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0x46, 0xE8, 0xD4, 0x5D, 0x3F, 0x2D, 0x5B, 0x51,
                                           0x9F, 0xEF, 0x85, 0x79, 0xBB, 0x24, 0x4B, 0x55,
                                           0x88, 0xAE, 0x12, 0x7D, 0xA5, 0xA0, 0x18, 0xE5,
                                           0x00, 0x78, 0x8F, 0x6F, 0x8B, 0x98, 0x46, 0x15};

  return lowmc_enc(Picnic_L5_1_FS, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vectorL5_M1_3(void) {
  const uint8_t key[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0xB8, 0x91, 0x7F, 0x8F, 0x11, 0x23, 0xD1, 0x83,
                                           0xD3, 0x5F, 0x52, 0xB8, 0x02, 0x5C, 0x49, 0xF2,
                                           0x41, 0x62, 0xC9, 0xEA, 0xDE, 0x63, 0x11, 0xD0,
                                           0xA9, 0xD4, 0x2A, 0xFD, 0x29, 0xEF, 0x9D, 0x95};

  return lowmc_enc(Picnic_L5_1_FS, key, plaintext, ciphertext_expected);
}
#endif


typedef int (*test_fn_t)(void);

static const test_fn_t tests[] = {
  LowMC_test_vectorL1_1, LowMC_test_vectorL1_2, LowMC_test_vectorL1_3,
  LowMC_test_vectorL3_1, LowMC_test_vectorL3_2, LowMC_test_vectorL3_3,
  LowMC_test_vectorL5_1, LowMC_test_vectorL5_2, LowMC_test_vectorL5_3,
#if defined(WITH_LOWMC_M1)
  LowMC_test_vectorL1_M1_1, LowMC_test_vectorL1_M1_2, LowMC_test_vectorL1_M1_3,
  LowMC_test_vectorL3_M1_1, LowMC_test_vectorL3_M1_2, LowMC_test_vectorL3_M1_3,
  LowMC_test_vectorL5_M1_1, LowMC_test_vectorL5_M1_2, LowMC_test_vectorL5_M1_3,

#endif
};

static const size_t num_tests = sizeof(tests) / sizeof(tests[0]);

#if defined(__WIN32__)
#define SIZET_FMT "%Iu"
#else
#define SIZET_FMT "%zu"
#endif

int main(void) {
  int ret = 0;
  for (size_t s = 0; s < num_tests; ++s) {
    const int t = tests[s]();
    if (t) {
      printf("ERR: lowmc_enc " SIZET_FMT " FAILED (%d)\n", s, t);
      ret = -1;
    }
  }

  return ret;
}
