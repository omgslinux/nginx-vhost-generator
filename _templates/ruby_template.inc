# Source: https://www.nginx.com/resources/wiki/start/topics/examples/simplerubyfcgi/

# Make sure JAVA_URL is set with a suitable default
PROXY_PASS=${PROXY_PASS:-http://unix:/var/run/my_app.sock:}


RUBY_BLOCK="
    index index.html index.htm;

    location / {
        proxy_set_header  X-Real-IP        \$remote_addr;
        proxy_set_header  X-Forwarded-For  \$proxy_add_x_forwarded_for;
        proxy_set_header  Host             \$http_host;
        proxy_redirect    off;
        proxy_pass        ${PROXY_PASS};
    }
"

CUSTOM_BLOCK="${RUBY_BLOCK}"

# Clean up local useless variables except CUSTOM_BLOCK
unset RUBY_BLOCK PROXY_PASS
