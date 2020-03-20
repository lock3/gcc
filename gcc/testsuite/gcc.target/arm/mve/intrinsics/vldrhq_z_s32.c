/* { dg-do compile  } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-add-options arm_v8_1m_mve } */
/* { dg-additional-options "-O2" } */

#include "arm_mve.h"

int32x4_t
foo (int16_t const * base, mve_pred16_t p)
{
  return vldrhq_z_s32 (base, p);
}

/* { dg-final { scan-assembler "vldrht.s32"  }  } */