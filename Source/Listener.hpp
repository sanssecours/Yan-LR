// -- Imports ------------------------------------------------------------------

#include <stack>

#include <kdb.hpp>

#include "YAMLBaseListener.h"

using std::stack;

using antlr::YAMLBaseListener;
using ScalarContext = antlr::YAML::ScalarContext;

using CppKey = kdb::Key;
using CppKeySet = kdb::KeySet;

// -- Class --------------------------------------------------------------------

/**
 * @brief This class creates a key set by listening to matches of grammar rules
 *        specified via YAML.g4.
 */
class KeyListener : public YAMLBaseListener {
  /** This variable stores a key set representing the textual input. */
  CppKeySet keys;
  /**
   * This stack stores a key for each level of the current key name below
   * parent.
   */
  stack<CppKey> parents;

public:
  /**
   * @brief This constructor creates a new empty key storage using the given
   *        parent key.
   *
   * @param parent This key specifies the parent of all keys stored in the
   *               object.
   */
  KeyListener(CppKey parent);

  /**
   * @brief This function returns the data read by the parser.
   *
   * @return The key set representing the data from the textual input
   */
  CppKeySet keySet();

  /**
   * @brief This function will be called after the parser exits a scalar value.
   *
   * @param context The context specifies data matched by the rule.
   */
  void exitScalar(ScalarContext *context) override;
};
