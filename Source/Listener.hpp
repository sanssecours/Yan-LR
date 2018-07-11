// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <numeric>

#include <kdb.hpp>
#include <kdbease.h>

#include "YAMLBaseListener.h"

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
  /**
   * @brief This constructor creates a new empty key storage using the given
   * parent key.
   *
   * @param parent This key specifies the parent of all keys stored in the
   * object.
   */
  KeyListener(CppKey parent) : keys{} { parents.push(parent); }

  /**
   * @brief This function will be called after the parser exits a scalar value.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void exitValue(ValueContext *context) override {
    CppKey key = parents.top();
    key.setString(context->getText());
    keys.append(key);
  }

  /**
   * @brief This function returns the data read by the parser.
   *
   * @return The key set representing the data from the textual input
   */
  CppKeySet keySet() { return keys; }
};
