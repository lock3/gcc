// Test to ensure the [[versioned]] attribute issues a diagnostic when used
// incorrectly.
// { dg-additional-options "-fmodules-ts -fcontracts" }
export module foo; // { dg-warning "not writing module" }

export [[versioned(a, b, c)]] void foo1() {} // { dg-error "arguments" }
export [[versioned]] int foo2; // { dg-warning "ignored" }
export [[versioned]] enum foo3 { FOO3A, FOO3B }; // { dg-warning "ignored" }

int main(int, char**)
{
  [[versioned]] if(true) return 0; // { dg-warning "ignored" }
  [[versioned]] while(true) // { dg-warning "ignored" }
  {
    [[versioned]] continue; // { dg-warning "ignored" }
  }
  return 0;
}

