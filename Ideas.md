# Indentation Rules

- Peek into lookahead (after `NL`) and count spaces at the beginning of a line
  - Set global values accordingly to check if the new line is indented less or more
- Add indent and detent tokens
  - Check proper indentation afterwards (`$text.length`)
- Overwrite `nextToken` method
  - Save tokens read until now

# Conversion to `KeySet`

## Example

### Input

```yaml
bla:
  blubb:
   hello
  ma:
    bu:
     mu
hell:
  ooo
```

### Converted `KeySet`

```
bla/blubb: hello
bla/ma/bu: mu
hell:      ooo
```

### Algorithm

Enter Rule:

  - If part of key (`map` or `seq`): Save as part of current path
  - If value: Set value at current path

Exit Rule:

- Drop last part of path for `map` or `seq`

#### Data Structures

`path`: Stack containing each part of path
