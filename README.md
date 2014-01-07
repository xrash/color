# Enchant your terminal

**color** makes it easy to highlight texts on your terminal.

It can be used for simple tasks like localizing some string among a big blob or more complexs ones such as highlighting a programming language syntax.

**color** is written in C and depends on PCRE, so make sure you got it on your system.

Installing:

    make install

Usage examples:

    cat BIGTEXT | color rainbow

    cat BIGTEXT | color rainbow -c blue/green

    cat BIGTEXT | color rain(cross)?bow -c blue/green

    color -f /path/to/file -c red WARNING

    color -m char \
        'if(?= ?\()' -c cyan \
        'true|false' -c cyan \
        '\(|\)' -c yellow \
        'then|else' -c green \
        '".*"' -c red \
    <<EOF
    if (true) then "if (" else false
    EOF
