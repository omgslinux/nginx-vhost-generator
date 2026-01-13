# Generic PHP template based on the default vhost
# Copy the necessary files conf files

# Copy the necessary files conf files
if [[ ! -f ../conf.d/php.conf ]];then
    cp -p ${CONFS_DIR}/php.conf ../conf.d
fi

PHP_BLOCK="
    ### BEGIN PHP BLOCK
    index ${INDEX_BLOCK:-index.php};

    location / {
        # try to serve file directly, fallback to index.php
        try_files \$uri \$uri/ /index.php\$is_args\$args;
    }

    location ~ \.php\$ {
        fastcgi_pass ${FASTCGI_PASS:-\$upstream_php};
        include snippets/fastcgi-php.conf;
		${SSLCLIENT_FASTCGI:+include snippets/sslclient-fastcgi.conf;}
        ${EXTRA_INDEX_LOCATION_BLOCK}
    }

"

CUSTOM_BLOCK="${PHP_BLOCK}"

unset PHP_BLOCK
