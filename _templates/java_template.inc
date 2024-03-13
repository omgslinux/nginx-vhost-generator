# Source: https://www.nginx.com/resources/wiki/start/topics/examples/javaservers/

# Make sure JAVA_URL is set with a suitable default
APPSERVER_URL=${APPSERVER_URL:-http://localhost:8080}

COMMON_BLOCK="
    proxy_pass        ${APPSERVER_URL};
    proxy_set_header  X-Real-IP \$remote_addr;
    proxy_set_header  X-Forwarded-For \$proxy_add_x_forwarded_for;
    proxy_set_header  Host \$http_host;
"
JAVA_BLOCK="
  location / {
    index index.jsp;
  }

  location ~ \.do\$ {
${COMMON_BLOCK}
  }

  location ~ \.jsp$ {
${COMMON_BLOCK}
  }

  location ^~/servlets/* {
${COMMON_BLOCK}
  }
"

CUSTOM_BLOCK="${JAVA_BLOCK}"

unset COMMON_BLOCK JAVA_BLOCK APPSERVER_URL
