#!/usr/bin/env fish

set parser "Build/badger"

set IFS (printf '\n\b')
for file in (find Input -depth 1 -type file -name '*.yaml')
    printf "• Test file “%s”\n" "$file"
    set error_message (eval $parser "\"$file\"" 2>&1 >/dev/null)
    if test "$status" -ne 0
        printf "Unable to parse “%s”:\n\n" "$file" >&2
        printf '%s\n\n' "$error_message" >&2
        set failed 'true'
    end
end

if test "$failed" = 'true'
    exit 1
end
