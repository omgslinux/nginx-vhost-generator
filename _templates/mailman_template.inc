# Source: https://www.nginx.com/resources/wiki/start/topics/recipes/mailman/


# Make sure JAVA_URL is set with a suitable default
MAILMAN_INTERNAL_URL=${MAILMAN_INTERNAL_URL:-http://127.0.0.1/}
MAILMAN_DATA_DIR=${MAILMAN_DATA_DIR:-/var/lib/mailman}
DOCROOT=${DOCROOT:-/usr/lib}

COMMON_BLOCK="
    proxy_pass        ${APPSERVER_URL};
    proxy_set_header  X-Real-IP \$remote_addr;
    proxy_set_header  X-Forwarded-For \$proxy_add_x_forwarded_for;
    proxy_set_header  Host \$http_host;
"

MAILMAN_BLOCK="
    location = / {
        rewrite ^ /mailman/listinfo permanent;
    }

    location / {
        rewrite ^ /mailman\$uri?\$args;
    }

    location = /mailman/ {
        rewrite ^ /mailman/listinfo permanent;
    }

    location /mailman/ {
        include proxy_params;
        proxy_pass ${MAILMAN_INTERNAL_URL};
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
    }

    location /cgi-bin {
        rewrite ^/cgi-bin(.*)\$ \$1 permanent;
    }

    location /images/mailman {
        alias ${MAILMAN_DATA_DIR}/icons;
    }

    location /pipermail {
        alias ${MAILMAN_DATA_DIR}/archives/public;
        autoindex on;
    }
"

CUSTOM_BLOCK="${MAILMAN_BLOCK}"

unset COMMON_BLOCK MAILMAN_BLOCK MAILMAN_INTERNAL_URL MAILMAN_DATA_DIR
