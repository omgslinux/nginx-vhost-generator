function initVars()
{
    SERVER=""
    SUFFIX=""
    DOCROOT=""
    HTTP_PORT=""
    HTTP_ENV=""
    HTTPS_PORT=""
    HTTPS_ENV=""
    REDIRECT_BLOCK=""
    SSLCLIENT_FASTCGI=""
    SSLCERT_VERIFY=""
    SERVER_BLOCK=""
    HTTP_BLOCK=""
    HTTPS_BLOCK=""
    SSL_BLOCK=""
    CUSTOM_BLOCK=""
}

function processServerBlock()
{

    SERVER_BLOCK="
server {
    ${LISTEN_BLOCK}
    server_name ${SERVER} ${SERVER}.${SUFFIX};
    root ${DOCROOT};
    charset utf-8;

    error_log ${LOGDIR}/${SERVER}_error.log;
    access_log ${LOGDIR}/${SERVER}_access.log;

    ${APP_ENV}

    location ~* \.(css|js|jpg)\$ {
        access_log off;

        add_header Cache-Control public;
        add_header Pragma public;
        add_header Vary Accept-Encoding;
        expires 1M;
    }
    ${SSL_BLOCK}

    ${CUSTOM_BLOCK}
}
    "
}

function processServers()
{
    REDIRECT_BLOCK="
server {
    listen ${HTTP_PORT};
    listen [::]:${HTTP_PORT};
    server_name ${SERVER} ${SERVER}.${SUFFIX};
    return 301 https://\$server_name:${HTTPS_PORT}/\$request_uri;
}
    "

    LISTEN_HTTP_BLOCK="
    listen ${HTTP_PORT};
    listen [::]:${HTTP_PORT};"
    LISTEN_HTTPS_BLOCK="
    listen ${HTTPS_PORT} ssl http2;
    listen [::]:${HTTPS_PORT};"
    APP_ENV=""
    if [[ ${HTTP_ENV} ]];then
        APP_ENV="set \$app_env ${HTTP_ENV};"
    fi

    LISTEN_BLOCK="${LISTEN_HTTP_BLOCK}"
    processServerBlock

    if [[ $HTTP_REDIRECT ]];then
        HTTP_BLOCK="${REDIRECT_BLOCK}"
    else
        HTTP_BLOCK="${SERVER_BLOCK}"
    fi
    APP_ENV=""
    if [[ ${HTTPS_ENV} ]];then
        APP_ENV="set \$app_env ${HTTPS_ENV};"
    fi


    LISTEN_BLOCK="${LISTEN_HTTPS_BLOCK}"
    SSL_BLOCK="
    add_header Strict-Transport-Security \"max-age=31536000;\";
    add_header Pragma \"no-cache\";
    add_header Cache-Control \"private, max-age=0, no-cache, no-store\";

    ${CERTS}
    ${SSLCLIENT_VERIFY}
"

    processServerBlock
    HTTPS_BLOCK="${SERVER_BLOCK}"
    #HTTPS_BLOCK="${_HTTPS_BLOCK}"
}
