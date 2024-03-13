#!/bin/bash

if [[ -z $@ ]];then exit 1; fi

function initVars()
{
	# Variables to be unset at the beginning of each vhost. You can set vhost defaults in defaults.inc
	unset SERVER SERVERNAME SUFFIX DOCROOT HTTP_PORT HTTP_ENV HTTPS_PORT HTTPS_ENV APP_ENV VHOST_TYPE
	unset SSL_BLOCK CUSTOM_BLOCK PROXY_PASS SSLCLIENT_FASTCGI SERVER_BLOCK
	unset LOGDIRFORMAT SSL_CERTIFICATE SSL_CERTIFICATE_KEY SSL_CLIENT_CERTIFICATE SSL_VERIFY_CLIENT
}

function main()
{
	for VHOST in $@;do
		if [[ -d ${VHOST} ]];then
			initVars
			. defaults.inc
			for VHOSTFILE in ${VHOST}/*.inc;do
				. ${VHOSTFILE}
			done
			if [[ ${VHOST_TYPE} ]];then
				TEMPLATE_FILE="_templates/${VHOST_TYPE}_template.inc"
				if [[ -f ${TEMPLATE_FILE} ]];then
					. ${TEMPLATE_FILE}
					getLogDirFormat
					LOGDIR="/var/log/nginx/${LOGDIRFORMAT:-${SERVER}.${SUFFIX}}"
					processServers
					writeBlocks

					rm ../sites-enabled/${SERVER} 2>/dev/null
					ln -s ../sites-available/${SERVER} ../sites-enabled/
					echo "Vhost ${VHOST} created, along with ${LOGDIR} and site-enabled symlink"
				else
					echo "Invalid VHOST_TYPE for ${VHOST}. Choose one of _templates/<VHOST_TYPE>_template.inc"
				fi
			else
				echo "No VHOST_TYPE variable set in ${VHOST}"
			fi
		fi
	done

}

function processServerBlock()
{

    SERVER_BLOCK="
server {
    ${LISTEN_BLOCK}
    server_name ${SERVERNAME:-${SERVER}${SUFFIX:+ ${SERVER}.${SUFFIX}}};
    root ${DOCROOT};
    charset utf-8;

    error_log ${LOGDIR}/${SERVER}${SSL_BLOCK:+-ssl}_error.log;
    access_log ${LOGDIR}/${SERVER}${SSL_BLOCK:+-ssl}_access.log;

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
	DEFAULT_HTTP_PORT="80"
	DEFAULT_HTTPS_PORT="443"
	URL_HTTP_PORT=""
	URL_HTTPS_PORT=""
	if [[ ${DEFAULT_HTTP_PORT} != ${HTTP_PORT} ]];then
		URL_HTTP_PORT=":${HTTP_PORT}"
	fi
	if [[ ${DEFAULT_HTTPS_PORT} != ${HTTPS_PORT} ]];then
		URL_HTTPS_PORT=":${HTTPS_PORT}"
	fi
    REDIRECT_BLOCK="
server {
    listen ${HTTP_PORT};
    listen [::]:${HTTP_PORT};
    server_name ${SERVERNAME:-${SERVER} ${SERVER}.${SUFFIX}};
    # Prevent nginx HTTP Server Detection
    server_tokens off;
    return 301 https://\$server_name${URL_HTTPS_PORT}/\$request_uri;
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
    unset APP_ENV

	SSL_BLOCK="
	# BEGIN SSL_BLOCK
	add_header Strict-Transport-Security \"max-age=31536000;\";
	add_header Pragma \"no-cache\";
	add_header Cache-Control \"private, max-age=0, no-cache, no-store\";

	# BEGIN CERT BLOCK
	${SSL_CERTIFICATE:+ssl_certificate ${SSL_CERTIFICATE};}
	${SSL_CERTIFICATE_KEY:+ssl_certificate_key ${SSL_CERTIFICATE_KEY};}
	${SSL_CLIENT_CERTIFICATE:+ssl_client_certificate ${SSL_CLIENT_CERTIFICATE};}
	${SSL_VERIFY_CLIENT:+ssl_verify_client ${SSL_VERIFY_CLIENT};}
	# END CERT BLOCK

	ssl_protocols        TLSv1 TLSv1.1 TLSv1.2;
	keepalive_timeout    70;
	ssl_session_cache    shared:SSL:10m;
	ssl_session_timeout  10m;
	# END SSL_BLOCK
"

	if [[ ${SSL_CERTIFICATE} ]];then
	    if [[ ${HTTPS_ENV} ]];then
	        APP_ENV="set \$app_env ${HTTPS_ENV};"
	    fi


	    LISTEN_BLOCK="${LISTEN_HTTPS_BLOCK}"

	    processServerBlock
	    HTTPS_BLOCK="${SERVER_BLOCK}"
	fi
}

function writeBlocks()
{
	# This block moved to a function for proper indenting out of main for loop
	echo "# Generated http server block for ${VHOST} site
	${HTTP_BLOCK}

	${HTTPS_BLOCK:+# Generated https server block for ${VHOST} site}
	${HTTPS_BLOCK}">../sites-available/${SERVER}
	if [ ! -d ${LOGDIR} ];then
		mkdir -p ${LOGDIR}
	fi
}


main $@
