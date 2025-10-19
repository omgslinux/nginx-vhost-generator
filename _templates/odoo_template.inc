# Odoo is a proxy to two upstream servers, so lets inherit first as a proxy

. ${TEMPLATES_DIR}/proxy_template.inc

if [[ ${PROXY_LONGPOLLING} ]];then
    LONGPOLLING_BLOCK="
    # Chat block. If using an old version, try using /longpolling instead of /websocket
    location /websocket {
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;
        proxy_pass ${PROXY_LONGPOLLING};
        include proxy_params;
        proxy_set_header X-Forwarded-Host $host;
    }
"
fi

# We already have a generic proxy CUSTOM_BLOCK
ODOO_BLOCK="

${CUSTOM_BLOCK}
    #Restrict extern access to database manager
    location ~ (\/web/database) {
      allow 127.0.0.1;
      deny all;
    }

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
