// { dg-do compile }
// { dg-additional-options { -std=c++2a -Kfoo=5 -Ktest -Kb=yes -Kpre=no -Kpost=very  -fdump-build-environment } }
// { dg-output "knob .post.: .very." }
// { dg-output "knob .assert.: .enforce." }
// { dg-output "knob .assume.: .ignore." }
// { dg-output "knob .b.: .yes." }
// { dg-output "knob .pre.: .no." }
// { dg-output "knob .test.: .." }
// { dg-output "knob .foo.: .5." }

int main(int, char**)
{
}
