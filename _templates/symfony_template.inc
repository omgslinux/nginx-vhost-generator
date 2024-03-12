
SYMFONY_BLOCK="
    ### BEGIN SYMFONY BLOCK

    # Add index.php to the list if you are using PHP
    index index.php;

    location / {
        # try to serve file directly, fallback to index.php
        try_files \$uri /index.php\$is_args\$args;
    }

    location ~ ^/index\.php(/|\$) {
        fastcgi_pass \$upstream_php;
        include snippets/fastcgi-php.conf;
        fastcgi_param APP_ENV \$app_env;

		${SSLCLIENT_FASTCGI:+include snippets/sslclient-fastcgi.conf;}

    }

    # pass PHP scripts to FastCGI server
    #
    location ~ \.php\$ {
        return 404;
    }

    # deny access to .htaccess files, if Apache's document root
    # concurs with nginx's one
    #
    location ~ /\.ht {
        deny all;
    }
    ### END OF SYMFONY BLOCK
"

CUSTOM_BLOCK="${SYMFONY_BLOCK}"
