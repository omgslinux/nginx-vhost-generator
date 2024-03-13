# Taken from https://www.nginx.com/resources/wiki/start/topics/recipes/wordpress/
# For multisite to work, copy the wordpress.conf file to /etc/conf.d
if [[ ! -f ../conf.d/wordpress.conf ]];then
    cp -p wordpress.conf ../conf.d
fi

if [[ ! -f ../conf.d/upstream_php.conf ]];then
    cp -p upstream_php.conf ../conf.d
fi

unset WORDPRESS_MULTISITE_SUBDIR_BLOCK WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK WORDPRESS_SINGLESITE_BLOCK

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
    WORDPRESS_SINGLESITE_BLOCK="
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
        # include the \"?\$args\" part so non-default permalinks doesn't break when using query string
        try_files \$uri \$uri/ ${WORDPRESS_SUBDIR}/index.php?\$args;
    }

    location ~ \.php\$ {
        #NOTE: You should have \"cgi.fix_pathinfo = 0;\" in php.ini
        include fastcgi_params;
        ${WORDPRESS_ROOT:+fastcgi_split_path_info ^(${WORDPRESS_SUBDIR})(/.*)$;}
        fastcgi_intercept_errors on;
        fastcgi_pass \$upstream_php;
        #The following parameter can be also included in fastcgi_params file
        #fastcgi_param  SCRIPT_FILENAME \$document_root\$fastcgi_script_name;
    }

    location ~* \.(js|css|png|jpg|jpeg|gif|ico)\$ {
        expires max;
        log_not_found off;
    }
    "

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
    alias /var/www/example.com/htdocs/wp-content/blogs.dir ;
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
    try_files \$uri =404;
    include fastcgi_params;
    fastcgi_pass \$upstream_php;
}
    "
    else
        WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK="
    location / {
        try_files \$uri \$uri/ /index.php?\$args ;
    }

    location ~ \.php\$ {
        try_files \$uri =404;
        include fastcgi_params;
        fastcgi_pass \$upstream_php;
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
        alias /var/www/example.com/htdocs/wp-content/blogs.dir;
        access_log off;
        log_not_found off;
        expires max;
    }
"
    SERVERNAME="${SERVER}.${SUFFIX} *.${SERVER}.${SUFFIX}"
fi

WORDPRESS_BLOCK="
    index index.php;

    ${WORDPRESS_SINGLESITE_BLOCK}
    ${WORDPRESS_MULTISITE_SUBDIR_BLOCK}
    ${WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK}
"


CUSTOM_BLOCK="${WORDPRESS_BLOCK}"
unset WORDPRESS_SUBDIR WORDPRESS_SINGLESITE WORDPRESS_MULTISITE_SUBDIR WORDPRESS_MULTISITE_SUBDOMAIN
unset WORDPRESS_MULTISITE_SUBDIR_BLOCK WORDPPRESS_MULTISITE_SUBDOMAIN_BLOCK WORDPRESS_SINGLESITE_BLOCK
unset WORDPRESS_SUBDIR WORDPRESS_ROOT WORDPRESS_BLOCK