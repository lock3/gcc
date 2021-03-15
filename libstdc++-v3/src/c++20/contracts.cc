// -*- C++ -*- std::experimental::contracts and friends
// Copyright (C) 2021-2021 Free Software Foundation, Inc.
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#include <experimental/contracts>
#include <vector>
#include <algorithm>
//#include <exception>
#include <iostream>
//#include <cstdlib>
//#include <cstdio>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

namespace experimental
{
namespace contracts
{

violation::violation (int line_number,
  string_view file_name,
  string_view function_name,
  string_view comment,
  attribute contract_attribute,
  semantic contract_semantic,
  span<string_view> labels)
  : line_number_(line_number),
    file_name_(file_name),
    function_name_(function_name),
    comment_(comment),
    attribute_(contract_attribute),
    semantic_(contract_semantic),
    labels_(labels)
  { }

uint_least32_t
violation::line_number () const noexcept
{
  return this->line_number_;
}

string_view
violation::file_name () const noexcept
{
  return this->file_name_;
}

string_view
violation::function_name () const noexcept
{
  return this->function_name_;
}

string_view
violation::comment () const noexcept
{
  return this->comment_;
}

attribute
violation::contract_attribute () const noexcept
{
  return this->attribute_;
}

semantic
violation::contract_semantic () const noexcept
{
  return this->semantic_;
}

span<string_view>
violation::labels () const noexcept
{
  return this->labels_;
}

} // namespace contracts
} // namespace experimental

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std


__attribute__ ((weak)) void
handle_contract_violation (const std::experimental::contracts::violation &violation)
{
  std::cerr << "default std::handle_contract_violation called: " << std::endl
    << " " << violation.file_name()
    << " " << violation.line_number()
    << " " << violation.function_name()
    << " " << violation.comment()
    << " " << (int)violation.contract_attribute()
    << " " << (int)violation.contract_semantic()
    << " " << violation.labels().size()
    << " :";
  for (auto label : violation.labels())
    std::cerr << " " << label;
  std::cerr
    << std::endl;
}

// TODO if we require or implicitly import this header, span, string_view we
// could make objects static constinit and only require stack space for the
// frame of this trampoline and the actual handle function?
// We take POD types here to make synthesis easier
int
__on_contract_violation (int line_number,
			 const char *file_name,
			 const char *function_name,
			 const char *comment,
			 int attr, int sem,
			 const char **labels,
			 int label_count)
{
  using namespace std::experimental::contracts;
  semantic s = static_cast<semantic>(sem);

  std::vector<std::string_view> label_views;
  std::transform(labels, labels + label_count, std::back_inserter(label_views),
		 [](const char *label) { return std::string_view(label); });


  violation v (line_number,
	       file_name,
	       function_name,
	       comment,
	       static_cast<attribute>(attr),
	       s,
	       std::span<std::string_view>(label_views));
  handle_contract_violation (v);

  if (s != semantic::observe)
    std::abort ();

  return 0;
}

