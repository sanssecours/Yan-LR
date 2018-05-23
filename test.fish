#!/usr/bin/env fish

set parser "Build/badger"

for file in (find Input -type file -name '*.yaml')
    set error_message (eval $parser "$file" 2>&1 >/dev/null)
    if test "$status" -ne 0
        printf "Unable to parse “%s”:\n\n" "$file" >&2
        printf '%s\n\n' "$error_message" >&2
        set failed 'true'
    end
end

if test "$failed" = 'true'
    exit 1
end
