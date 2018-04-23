# Indentation Rules

- Peek into lookahead (after `NL`) and count spaces at the beginning of a line
  - Set global values accordingly to check if the new line is in
- Add indent and detent tokens
  - Check proper indentation afterwards (`$text.length`)
- Overwrite `nextToken` method
  - Save tokens read until now
