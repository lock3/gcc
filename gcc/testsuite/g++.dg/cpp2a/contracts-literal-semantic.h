#include <experimental/contracts>
using std::experimental::contracts::attribute;
using std::experimental::contracts::semantic;

struct [[contract_label(assume)]] assume_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::assume;
  }
};

struct [[contract_label(ignore)]] ignore_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::ignore;
  }
};

struct [[contract_label(check_never_continue)]] check_never_continue_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::enforce;
  }
};

struct [[contract_label(check_maybe_continue)]] check_maybe_continue_label
{
  static consteval semantic adjust_semantic(attribute, semantic)
  {
    return semantic::observe;
  }
};

