/* { dg-do compile  } */
/* { dg-require-effective-target arm_v8_1m_mve_fp_ok } */
/* { dg-add-options arm_v8_1m_mve_fp } */
/* { dg-additional-options "-O2" } */

#include "arm_mve.h"

float32_t
foo (float32_t a, float32x4_t b)
{
  return vmaxnmavq_f32 (a, b);
}

/* { dg-final { scan-assembler "vmaxnmav.f32"  }  } */

float32_t
foo1 (float32_t a, float32x4_t b)
{
  return vmaxnmavq (a, b);
}

/* { dg-final { scan-assembler "vmaxnmav.f32"  }  } */