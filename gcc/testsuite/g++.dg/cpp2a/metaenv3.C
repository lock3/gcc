// { dg-do compile }
// { dg-additional-options "-std=c++2a -Kfoo=5 -Kbar=3 -Ktest -Kb=yes -Knb=no -Ka1=Truthy -Ka2=Falsey -Ka3=FileNotFoundy -fdump-build-environment" }
// { dg-output "knob .nb.: .no." }
// { dg-output "knob .test.: .." }
// { dg-output "knob .a2.: .Falsey." }
// { dg-output "knob .a1.: .Truthy." }
// { dg-output "knob .b.: .yes." }
// { dg-output "knob .bar.: .3." }
// { dg-output "knob .a3.: .FileNotFoundy." }
// { dg-output "knob .foo.: .5." }

