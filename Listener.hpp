// -- Imports ------------------------------------------------------------------

#include <iostream>
#include <numeric>

#include <kdb.hpp>
#include <kdbease.h>

#include "YAMLBaseListener.h"

using antlr::YAMLBaseListener;
using ElementContext = antlr::YAMLParser::ElementContext;
using MappingContext = antlr::YAMLParser::MappingContext;
using SequenceContext = antlr::YAMLParser::SequenceContext;
using ValueContext = antlr::YAMLParser::ValueContext;

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
   * @brief This function will be called after the parser enters a mapping.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void enterMapping(MappingContext *context) override {
    // Entering a mapping such as `part: …` means that we need to add `part` to
    // the key name
    CppKey child{parents.top().getName(), KEY_END};
    child.addBaseName(context->key()->getText());
    parents.push(child);
  }

  /**
   * @brief This function will be called after the parser exits a mapping.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void exitMapping(MappingContext *context
                           __attribute__((unused))) override {
    cerr << "(exitMapping) Remove top element “" << parents.top().getName()
         << "” with value “" << parents.top().getString() << "”" << endl;
    // Returning from a mapping such as `part: …` means that we need need to
    // remove the key for `part` from the stack.
    parents.pop();
  }

  /**
   * @brief This function will be called after the parser enters a sequence.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void enterSequence(SequenceContext *context
                             __attribute__((unused))) override {
    cerr << "(enterSequence) Top element: “" << parents.top().getName() << "”"
         << endl;
    indices.push(0);
    parents.top().setMeta("array", ""); // We start with an empty array
  }

  /**
   * @brief This function will be called after the parser exits a sequence.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void exitSequence(SequenceContext *context
                            __attribute__((unused))) override {
    // We add the parent key of all array elements after we leave the sequence
    keys.append(parents.top());
    indices.pop();
  }

  /**
   * @brief This function will be called after the parser recognizes an element
   * of a sequence.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void enterElement(ElementContext *context
                            __attribute__((unused))) override {

    CppKey key{parents.top().getName(), KEY_END};
    key.addBaseName(numberToArrayBaseName(indices.top()));

    uintmax_t index = indices.top();
    indices.pop();
    if (index < UINTMAX_MAX) {
      index++;
    }
    indices.push(index);

    parents.top().setMeta("array", key.getBaseName());
    cerr << "(enterElement) Add new key: “" << key.getName() << "”" << endl;

    parents.push(key);
  }

  /**
   * @brief This function will be called after the parser read an element of a
   * sequence.
   *
   * @param context The context specifies data matched by the rule.
   */
  virtual void exitElement(ElementContext *context
                           __attribute__((unused))) override {
    parents.pop(); // Remove the key for the current array entry
  }

  /**
   * @brief This function returns the data read by the parser.
   *
   * @return The key set representing the data from the textual input
   */
  CppKeySet keySet() { return keys; }
};
