/* { dg-do compile  } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-add-options arm_v8_1m_mve } */
/* { dg-additional-options "-O2" } */

#include "arm_mve.h"

uint32x4_t
foo (uint32x4_t a, int32_t b)
{
  return vrshlq_n_u32 (a, b);
}

/* { dg-final { scan-assembler "vrshl.u32"  }  } */

uint32x4_t
foo1 (uint32x4_t a, int32_t b)
{
  return vrshlq (a, b);
}

/* { dg-final { scan-assembler "vrshl.u32"  }  } */