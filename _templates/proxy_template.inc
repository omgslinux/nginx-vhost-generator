# Copy the necessary files conf files
if [[ ! -f ../snippets/proxy_ssl_client-upstream.conf ]];then
    cp -p ${SNIPPETS_DIR}/proxy_ssl_client-upstream.conf ../snippets/
fi

if [[ ${USE_WEBSOCKET} ]];then
    read -r -d '' WEBSOCKET_BLOCK << EOB
        # Websocket
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection $connection_upgrade;
EOB
    fi
PROXY_BLOCK="
    ### BEGIN PROXY BLOCK

    add_header Strict-Transport-Security \"max-age=15552000; includeSubDomains\" always;
    add_header Pragma \"no-cache\";
    add_header Cache-Control \"private, max-age=0, no-cache, no-store\";

    proxy_connect_timeout       600;
    proxy_send_timeout          600;
    proxy_read_timeout          600;
    send_timeout                600;

    ## PROXY backend
    location / {
        proxy_pass ${PROXY_PASS};
        include proxy_params;
        proxy_buffering off;

		${SSLCLIENT_FASTCGI:+include snippets/proxy_ssl_client-upstream.conf;}
        ${USE_WEBSOCKET:+WEBSOCKET_BLOCK}
    }

    ### END OF PROXY BLOCK
"
CUSTOM_BLOCK="${PROXY_BLOCK}"

unset PROXY_BLOCK USE_WEBSOCKET WEBSOCKET_BLOCK
