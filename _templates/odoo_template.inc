# Odoo is a proxy to two upstream servers, so lets inherit first as a proxy

. ${TEMPLATES_DIR}/proxy_template.inc

if [[ ${PROXY_LONGPOLLING} ]];then
    LONGPOLLING_BLOCK="
    location /longpolling {
        proxy_pass ${PROXY_LONGPOLLING};
    }
"
fi

# We already have a generic proxy CUSTOM_BLOCK
ODOO_BLOCK="

${CUSTOM_BLOCK}
    ${LONGPOLLING_BLOCK}
    # common gzip
    gzip_types text/css text/scss text/plain text/xml application/xml application/json application/javascript;
    gzip on;

"

CUSTOM_BLOCK="${ODOO_BLOCK}"

# To avoid static block, preventing css to load
VHOST_TYPE="proxy"
unset PROXY_LONGPOLLING LONGPOLLING_BLOCK
unset ODOO_BLOCK
