// -- Imports ------------------------------------------------------------------

#include "Listener.hpp"

// -- Class --------------------------------------------------------------------

/**
 * @brief This constructor creates a new empty key storage using the given
 *        parent key.
 *
 * @param parent This key specifies the parent of all keys stored in the
 *               object.
 */
KeyListener::KeyListener(CppKey parent) : keys{} { parents.push(parent); }

/**
 * @brief This function will be called after the parser exits a scalar value.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitValue(ValueContext *context) {
  CppKey key = parents.top();
  key.setString(context->getText());
  keys.append(key);
}

/**
 * @brief This function returns the data read by the parser.
 *
 * @return The key set representing the data from the textual input
 */
CppKeySet KeyListener::keySet() { return keys; }
