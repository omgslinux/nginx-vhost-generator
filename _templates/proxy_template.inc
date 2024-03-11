if [[ ${SSL_CLIENT} ]];then
    PROXY_SSL_CLIENT="include snippets/proxy_ssl_client-upstream.conf;"
fi
PROXY_BLOCK="
    # BEGIN PROXY BLOCK

    add_header Strict-Transport-Security \"max-age=15552000; includeSubDomains\" always;
    add_header Pragma \"no-cache\";
    add_header Cache-Control \"private, max-age=0, no-cache, no-store\";

    fastcgi_buffers 64 4K;

    proxy_connect_timeout       600;
    proxy_send_timeout          600;
    proxy_read_timeout          600;
    send_timeout                600;

    ## PROXY backend
    location / {
        proxy_pass ${PROXY_PASS};
        include proxy_params;
        ${PROXY_SSL_CLIENT}
        proxy_buffering off;
    }

    # END OF PROXY BLOCK
"
CUSTOM_BLOCK="${PROXY_BLOCK}"
