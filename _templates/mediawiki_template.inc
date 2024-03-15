# Source: https://www.nginx.com/resources/wiki/start/topics/recipes/mediawiki/

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

MEDIAWIKI_BLOCK="
    index  index.php;

    client_max_body_size 5m;
    client_body_timeout 60;

    location / {
        try_files \$uri \$uri/ @rewrite;
    }

    location @rewrite {
        rewrite ^/(.*)\$ /index.php?title=\$1&\$args;
    }

    location ^~ /maintenance/ {
        return 403;
    }

     location /rest.php {
        try_files \$uri \$uri/ /rest.php?\$args;
    }

    location ~ \.php\$ {
        include fastcgi_params;
        fastcgi_pass \$upstream_php;
        fastcgi_param SCRIPT_FILENAME \$request_filename;
    }

    location ~* \.(js|css|png|jpg|jpeg|gif|ico)\$ {
        try_files \$uri /index.php;
        expires max;
        log_not_found off;
    }

    location = /_.gif {
        expires max;
        empty_gif;
    }

    location ^~ /cache/ {
        deny all;
    }

    location /dumps {
        root ${DOCROOT}/local;
        autoindex on;
    }
"

CUSTOM_BLOCK="${MEDIAWIKI_BLOCK}"

unset MEDIAWIKI_BLOCK
