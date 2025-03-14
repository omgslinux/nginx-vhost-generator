# Taken from https://www.nginx.com/resources/wiki/start/topics/recipes/wordpress/

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

# Copy the necessary files conf for wordpress blogs
if [[ ! -f ../conf.d/wordpress.conf ]];then
    cp -p ${CONFS_DIR}/wordpress.conf ../conf.d
fi


unset WORDPRESS_MULTISITE_SUBDIR_BLOCK WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK WORDPRESS_SINGLESITE_BLOCK

read -r -d '' LOCATION_PHP_COMMON_CONTENT_BLOCK << EOB
        try_files \$uri =404;
        include fastcgi.conf;
        fastcgi_pass \$upstream_php;
EOB

if [[ ${WORDPRESS_SINGLESITE} ]];then
    # Default SUBDIR to "/"
    if [[ -z ${WORDPRESS_SUBDIR} ]];then
        #WORDPRESS_SUBDIR="/"
        WORDPRESS_SUBDIR=${WORDPRESS_SUBDIR:-/}
    fi

    WORDPRESS_ROOT=""
    if [[ ${WORDPRESS_SUBDIR} = '/' ]];then
        WORDPRESS_ROOT="1"
    fi

    read -r -d '' WORDPRESS_SINGLESITE_BLOCK <<EOB
    location = /favicon.ico {
        log_not_found off;
        access_log off;
    }

    location = /robots.txt {
        allow all;
        log_not_found off;
        access_log off;
    }

    location ${WORDPRESS_SUBDIR} {
        # This is cool because no php is touched for static content.
        # include the "?\$args" part so non-default permalinks doesn't break when using query string
        try_files \$uri \$uri/ ${WORDPRESS_SUBDIR}/index.php?\$args;
    }

    location ~ \.php\$ {
        #NOTE: You should have "cgi.fix_pathinfo = 0;" in php.ini
        ${LOCATION_PHP_COMMON_CONTENT_BLOCK}
        ${WORDPRESS_ROOT:+fastcgi_split_path_info ^(${WORDPRESS_SUBDIR})(/.*)\$;}
    }

    location ~* \.(js|css|png|jpg|jpeg|gif|ico)\$ {
        expires max;
        log_not_found off;
    }
EOB

elif [[ ${WORDPRESS_MULTISITE_SUBDIR} ]];then

    WORDPRESS_MULTISITE_SUBDIR_BLOCK="
    location ~ ^(/[^/]+)?/files/(.+) {
        try_files /wp-content/blogs.dir/\$blogiddir/files/\$2 /wp-includes/ms-files.php?file=\$2 ;
        access_log off;
        log_not_found off;
        expires max;
    }

    #avoid php readfile()
    location ^~ /blogs.dir {
        internal;
        alias ${DOCROOT}/wp-content/blogs.dir ;
        access_log off;
        log_not_found off;
        expires max;
    }

    if (!-e \$request_filename) {
        rewrite /wp-admin\$ \$scheme://\$host\$uri/ permanent;
        rewrite ^(/[^/]+)?(/wp-.*) \$2 last;
        rewrite ^(/[^/]+)?(/.*\.php) \$2 last;
    }

    location / {
        try_files \$uri \$uri/ /index.php?\$args ;
    }

    location ~ \.php\$ {
        ${LOCATION_PHP_COMMON_CONTENT_BLOCK}
    }
    "
    else
        SERVERNAME="${SUFFIX} *.${SUFFIX}"
        WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK="
    location / {
        try_files \$uri \$uri/ /index.php?\$args ;
    }

    location ~ \.php\$ {
        ${LOCATION_PHP_COMMON_CONTENT_BLOCK}
    }

    #WPMU Files
    location ~ ^/files/(.*)\$ {
        try_files /wp-content/blogs.dir/\$blogiddom/\$uri /wp-includes/ms-files.php?file=\$1 ;
        access_log off;
        log_not_found off;
        expires max;
    }

    #WPMU x-sendfile to avoid php readfile()
    location ^~ /blogs.dir {
        internal;
        alias ${DOCROOT}/htdocs/wp-content/blogs.dir;
        access_log off;
        log_not_found off;
        expires max;
    }
"
fi

WORDPRESS_BLOCK="
    index index.php;

# IMPORTANT: If your wordpress is behind a reverse proxy using https,
# you may need to add this php snippet in your wp-config.php file,
# right below the database settings:
#
#if (isset (\$_SERVER['HTTP_X_FORWARDED_PROTO'])){
#//error_log (\$_SERVER['HTTP_X_FORWARDED_PROTO'] . \"\n\", 3, \"./http_log\");
#        if (\$_SERVER['HTTP_X_FORWARDED_PROTO'] == 'https'){
#                \$_SERVER['HTTPS']='on';
#                define('FORCE_SSL_ADMIN', true);
#                define('FORCE_SSL_LOGIN', true);
#                \$_SERVER['HTTP_HOST'] = \$_SERVER['HTTP_X_FORWARDED_HOST'];
#                define('WP_SITEURL', 'https://${SERVER}.${SUFFIX}'); // Full website FQDN
#                define('WP_HOME', 'https://${SERVER}.${SUFFIX}'); // Full website FQDN
#        }
#}

    ${WORDPRESS_SINGLESITE_BLOCK}
    ${WORDPRESS_MULTISITE_SUBDIR_BLOCK}
    ${WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK}
"


CUSTOM_BLOCK="${WORDPRESS_BLOCK}"
unset WORDPRESS_SUBDIR WORDPRESS_SINGLESITE WORDPRESS_MULTISITE_SUBDIR WORDPRESS_MULTISITE_SUBDOMAIN
unset WORDPRESS_MULTISITE_SUBDIR_BLOCK WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK WORDPRESS_SINGLESITE_BLOCK
unset WORDPRESS_SUBDIR WORDPRESS_ROOT WORDPRESS_BLOCK
