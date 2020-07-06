/* { dg-require-effective-target arm_v8_1m_mve_fp_ok } */
/* { dg-add-options arm_v8_1m_mve_fp } */
/* { dg-additional-options "-O2" } */

#include "arm_mve.h"

mve_pred16_t
foo (float32x4_t a, float32x4_t b, mve_pred16_t p)
{
  return vcmpneq_m_f32 (a, b, p);
}

/* { dg-final { scan-assembler "vpst" } } */
/* { dg-final { scan-assembler "vcmpt.f32"  }  } */

mve_pred16_t
foo1 (float32x4_t a, float32x4_t b, mve_pred16_t p)
{
  return vcmpneq_m (a, b, p);
}

/* { dg-final { scan-assembler "vpst" } } */
