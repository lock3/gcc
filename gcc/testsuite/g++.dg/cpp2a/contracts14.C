// ensure that exceptions thrown inside a custom contract violation handler
// are catchable up the call stack
// { dg-do run }
// { dg-options "-std=c++2a -Kassert=observe" }
#include <iostream>
#include <experimental/contracts>
using std::experimental::contracts::violation;

int
__on_contract_violation (
        int line_number,
        const char * file_name,
        const char * function_name,
        const char * comment,
        int attr, int sem,
        const char ** labels,
        int label_count);

void handle_contract_violation(const violation &violation) {
  std::cerr << "custom std::handle_contract_violation called:"
    << " " << violation.line_number()
    << " " << violation.file_name()
    << std::endl;
  throw -(int)violation.line_number();
}

int fun() {
  int x = 0;
  [[ assert: x < 0 ]];
  return 0;
}

int fun2() {
  const char *labels[2] = {"T4", "T5"};
  __on_contract_violation(1, "T1", "T2", "T3", 0, 3, labels, 2);
  return 1;
}

int fun3() {
  fun();
  return 2;
}

int main(int, char**) {
  try {
    int x = 0;
    [[ assert: x < 0 ]];
  } catch(int &ex) {
    std::cerr << "synth caught direct: " << ex << std::endl;
  }

  try {
    fun2();
  } catch(int &ex) {
    std::cerr << "caught lib direct: " << ex << std::endl;
  }

  try {
    fun();
  } catch(int &ex) {
    std::cerr << "synth caught indirect: " << ex << std::endl;
  }

  try {
    fun3();
  } catch(int &ex) {
    std::cerr << "synth caught double indirect: " << ex << std::endl;
  }

  std::cerr << "end main" << std::endl;
  return 0;
}

// { dg-output "custom std::handle_contract_violation called: 47 .*/contracts14.C(\n|\r\n|\r)*" }
// { dg-output "synth caught direct: -47(\n|\r\n|\r)*" }
// { dg-output "custom std::handle_contract_violation called: 1 T1(\n|\r\n|\r)*" }
// { dg-output "caught lib direct: -1(\n|\r\n|\r)*" }
// { dg-output "custom std::handle_contract_violation called: 29 .*/contracts14.C(\n|\r\n|\r)*" }
// { dg-output "synth caught indirect: -29(\n|\r\n|\r)*" }
// { dg-output "custom std::handle_contract_violation called: 29 .*/contracts14.C(\n|\r\n|\r)*" }
// { dg-output "synth caught double indirect: -29(\n|\r\n|\r)*" }
// { dg-output "end main" }

