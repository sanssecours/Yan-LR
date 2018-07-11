// -- Imports ------------------------------------------------------------------

#include <stack>

#include <kdb.hpp>

#include "YAMLBaseListener.h"

using std::stack;

using antlr::YAMLBaseListener;
using ValueContext = antlr::YAML::ValueContext;

using CppKey = kdb::Key;
using CppKeySet = kdb::KeySet;

// -- Functions ----------------------------------------------------------------

string numberToArrayBaseName(uintmax_t const number) {
  size_t digits = 1;
  string output = "#";

  for (uintmax_t value = number; value > 9; digits++) {
    value /= 10;
  }

  return "#" + string(digits - 1, '_') + to_string(number);
}

// -- Class --------------------------------------------------------------------

/**
 * @brief This class creates a key set by listening to matches of grammar rules
 * specified via YAML.g4.
 */
class KeyListener : public YAMLBaseListener {
  /** This variable stores a key set representing the textual input. */
  CppKeySet keys;
  /** This stack stores a key for each level of the current key name below
   * parent. */
  stack<CppKey> parents;
  stack<uintmax_t> indices;

public:
  KeyListener(CppKey parent);
  CppKeySet keySet();

  void exitValue(ValueContext *context) override;
};
