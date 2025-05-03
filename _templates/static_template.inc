# Generic static template based on the default vhost
# No interpreter, just plain html

STATIC_BLOCK="
    ### BEGIN PHP BLOCK
    index ${INDEX_BLOCK:-index.html};

"

CUSTOM_BLOCK="${STATIC_BLOCK}"

unset STATIC_BLOCK
