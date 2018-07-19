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
 * @brief This function returns the data read by the parser.
 *
 * @return The key set representing the data from the textual input
 */
CppKeySet KeyListener::keySet() { return keys; }

/**
 * @brief This function will be called after the parser exits a value.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitValue(ValueContext *context) {
  CppKey key = parents.top();
  key.setString(context->getText());
  keys.append(key);
}

/**
 * @brief This function will be called after the parser enters a key-value pair.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::enterPair(PairContext *context) {
  // Entering a mapping such as `part: …` means that we need to add `part` to
  // the key name
  CppKey child{parents.top().getName(), KEY_END};
  child.addBaseName(context->key()->getText());
  parents.push(child);
}

/**
 * @brief This function will be called after the parser exits a key-value pair.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitPair(PairContext *context __attribute__((unused))) {
  // Returning from a mapping such as `part: …` means that we need need to
  // remove the key for `part` from the stack.
  parents.pop();
}
