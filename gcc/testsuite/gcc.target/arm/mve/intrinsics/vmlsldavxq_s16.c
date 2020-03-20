/* { dg-do compile  } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-add-options arm_v8_1m_mve } */
/* { dg-additional-options "-O2" } */

#include "arm_mve.h"

int64_t
foo (int16x8_t a, int16x8_t b)
{
  return vmlsldavxq_s16 (a, b);
}

/* { dg-final { scan-assembler "vmlsldavx.s16"  }  } */

int64_t
foo1 (int16x8_t a, int16x8_t b)
{
  return vmlsldavxq (a, b);
}

/* { dg-final { scan-assembler "vmlsldavx.s16"  }  } */