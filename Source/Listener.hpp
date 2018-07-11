// -- Imports ------------------------------------------------------------------

#include <stack>

#include <kdb.hpp>

#include "YAMLBaseListener.h"

using std::stack;

using antlr::YAMLBaseListener;
using ValueContext = antlr::YAML::ValueContext;

using CppKey = kdb::Key;
using CppKeySet = kdb::KeySet;

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

public:
  KeyListener(CppKey parent);
  CppKeySet keySet();

  void exitValue(ValueContext *context) override;
};
