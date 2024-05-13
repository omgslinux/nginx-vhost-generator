# Taken from https://docs.nextcloud.com/server/28/admin_manual/installation/nginx.html

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

read -r -d '' GZIP_BLOCK <<EOB
    # Enable gzip but do not remove ETag headers
    gzip on;
    gzip_vary on;
    gzip_comp_level 4;
    gzip_min_length 256;
    gzip_proxied expired no-cache no-store private no_last_modified no_etag auth;
    gzip_types application/atom+xml application/javascript application/json application/ld+json application/manifest+json application/rss+xml application/vnd.geo+\$
    son application/vnd.ms-fontobject application/x-font-ttf application/x-web-app-manifest+json application/xhtml+xml application/xml font/opentype image/bmp image/s\$
    g+xml image/x-icon text/cache-manifest text/css text/plain text/vcard text/vnd.rim.location.xloc text/vtt text/x-component text/x-cross-domain-policy;
EOB

read -r -d '' HEADERS_BLOCK << EOB
        # HTTP response headers borrowed from Nextcloud '.htaccess'
        add_header Cache-Control                     "public, max-age=15778463\$asset_immutable";
        add_header Referrer-Policy                   "no-referrer"       always;
        add_header X-Content-Type-Options            "nosniff"           always;
        add_header X-Frame-Options                   "SAMEORIGIN"        always;
        add_header X-Permitted-Cross-Domain-Policies "none"              always;
        add_header X-Robots-Tag                      "noindex, nofollow" always;
        add_header X-XSS-Protection                  "1; mode=block"     always;
EOB

MIME_BLOCK="
    # Set .mjs and .wasm MIME types
    # Either include it in the default mime.types list
    # and include that list explicitly or add the file extension
    # only for Nextcloud like below:
    include mime.types;
    types {
        text/javascript mjs;
    #    application/wasm wasm;
    }

"

ROBOTS_BLOCK="
    location = /robots.txt {
        allow all;
        log_not_found off;
        access_log off;
    }
"
read -r -d '' WELL_KNOWN_BLOCK << EOB
location ^~ /.well-known {
    # The rules in this block are an adaptation of the rules
    # in the Nextcloud '.htaccess' that concern '/.well-known'.

    location = /.well-known/carddav { return 301 ${NEXTCLOUD_DIR}/remote.php/dav/; }
    location = /.well-known/caldav  { return 301 ${NEXTCLOUD_DIR}/remote.php/dav/; }

    location /.well-known/acme-challenge    { try_files \$uri \$uri/ =404; }
    location /.well-known/pki-validation    { try_files \$uri \$uri/ =404; }

    # Let Nextcloud's API for '/.well-known' URIs handle all other
    # requests by passing them to the front-end controller.
    return 301 ${NEXTCLOUD_DIR}/index.php\$request_uri;
}
EOB


read -r -d '' WEBFINGER_BLOCK << EOB
    # The following 2 rules are only needed for the user_webfinger app.
    # Uncomment it if you are planning to use this app.
    #rewrite ^/.well-known/host-meta /public.php?service=host-meta last;
    #rewrite ^/.well-known/host-meta.json /public.php?service=host-meta-json last;
EOB

read -r -d '' COMMON_BLOCK << EOB

    # Rule borrowed from '.htaccess'
    location ${NEXTCLOUD_DIR}/remote {
        return 301 ${NEXTCLOUD_DIR}/remote.php\$request_uri;
    }

    # Uncomment if your server is build with the ngx_pagespeed module
    # This module is currently not supported.
    #pagespeed off;

    # Serve static files. This block must be AFTER the php handler block
    location ~ \.(?:css|js|mjs|svg|gif|png|jpg|ico|wasm|tflite|map|ogg|flac)\$ {
        try_files \$uri ${NEXTCLOUD_DIR}/index.php\$request_uri;
        ${HEADERS_BLOCK}
        access_log off;     # Optional: Don't log access to assets
    }

    location ~ \.woff2?\$ {
        try_files \$uri ${NEXTCLOUD_DIR}/index.php\$request_uri;
        expires 7d;         # Cache-Control policy borrowed from '.htaccess'
        access_log off;     # Optional: Don't log access to assets
    }

EOB

read -r -d '' NEXTCLOUD_BLOCK << EOB

    ${HEADERS_BLOCK}
    ${MIME_BLOCK}
    ${ROBOTS_BLOCK}
    ${GZIP_BLOCK}
    ${WEBFINGER_BLOCK}
    ${WELL_KNOWN_BLOCK}


    location / {
        rewrite ^ /index.php\$uri;
    }

    location ~ ^/(?:build|tests|config|lib|3rdparty|templates|data)/ {
        deny all;
    }
    location ~ ^/(?:\.|autotest|occ|issue|indie|db_|console) {
        deny all;
    }

    location ~ ^/(?:index|remote|public|cron|core/ajax/update|status|ocs/v[12]|updater/.+|ocs-provider/.+)\.php(?:\$|/) {
        fastcgi_split_path_info ^(.+\.php)(/.*)\$;
        include fastcgi_params;
        fastcgi_param SCRIPT_FILENAME \$document_root\$fastcgi_script_name;
        fastcgi_param PATH_INFO \$fastcgi_path_info;
        fastcgi_param HTTPS on;
        #Avoid sending the security headers twice
        fastcgi_param modHeadersAvailable true;
        fastcgi_param front_controller_active true;
        fastcgi_pass \$php_nextcloud;
        fastcgi_intercept_errors on;
        fastcgi_request_buffering off;
        fastcgi_read_timeout 300;
    }

    location ~ ^/(?:updater|ocs-provider)(?:\$|/) {
        try_files \$uri/ =404;
        index index.php;
    }

    ${COMMON_BLOCK}

EOB

read -r -d '' LOCAL_BLOCK <<EOB

    ${MIME_BLOCK}
    ${ROBOTS_BLOCK}

    location ^~ ${NEXTCLOUD_DIR} {

        ${GZIP_BLOCK}
        # Pagespeed is not supported by Nextcloud, so if your server is built
        # with the 'ngx_pagespeed' module, uncomment this line to disable it.
        #pagespeed off;
       # The settings allows you to optimize the HTTP2 bandwidth.
        # See https://blog.cloudflare.com/delivering-http-2-upload-speed-improvements/
        # for tuning hints
        client_body_buffer_size 512k;

        # HSTS settings
        # WARNING: Only add the preload option once you read about
        # the consequences in https://hstspreload.org/. This option
        # will add the domain to a hardcoded list that is shipped
        # in all major browsers and getting removed from this list
        # could take several months.
        #add_header Strict-Transport-Security "max-age=15768000; includeSubDomains; preload;" always;

        ${HEADERS_BLOCK}

        # Remove X-Powered-By, which is an information leak
        fastcgi_hide_header X-Powered-By;

        # Specify how to handle directories -- specifying '${NEXTCLOUD_DIR}/index.php\$request_uri'
        # here as the fallback means that Nginx always exhibits the desired behaviour
        # when a client requests a path that corresponds to a directory that exists
        # on the server. In particular, if that directory contains an index.php file,
        # that file is correctly served; if it doesn't, then the request is passed to
        # the front-end controller. This consistent behaviour means that we don't need
        # to specify custom rules for certain paths (e.g. images and other assets,
        # '/updater', '/ocs-provider'), and thus
        # 'try_files \$uri \$uri/ ${NEXTCLOUD_DIR}/index.php\$request_uri'
        # always provides the desired behaviour.
        index index.php index.html ${NEXTCLOUD_DIR}/index.php\$request_uri;

        # Rule borrowed from '.htaccess' to handle Microsoft DAV clients
        location = ${NEXTCLOUD_DIR} {
            if ( \$http_user_agent ~ ^DavClnt ) {
                return 302 ${NEXTCLOUD_DIR}/remote.php/webdav/\$is_args\$args;
            }
        }

        # Rules borrowed from '.htaccess' to hide certain paths from clients
        location ~ ^${NEXTCLOUD_DIR}/(?:build|tests|config|lib|3rdparty|templates|data)(?:\$|/)    { return 404; }
        location ~ ^${NEXTCLOUD_DIR}/(?:\.|autotest|occ|issue|indie|db_|console)                  { return 404; }

        # Ensure this block, which passes PHP files to the PHP process, is above the blocks
        # which handle static assets (as seen below). If this block is not declared first,
        # then Nginx will encounter an infinite rewriting loop when it prepends
        # '${NEXTCLOUD_DIR}/index.php' to the URI, resulting in a HTTP 500 error response.
        location ~ \.php(?:\$|/) {
            # Required for legacy support
            rewrite ^${NEXTCLOUD_DIR}/(?!index|remote|public|cron|core\/ajax\/update|status|ocs\/v[12]|updater\/.+|ocs-provider\/.+|.+\/richdocumentscode(_arm64)?\/proxy) /nextcloud/index.php\$request_uri;

            fastcgi_split_path_info ^(.+?\.php)(/.*)\$;
            set \$path_info \$fastcgi_path_info;

            try_files \$fastcgi_script_name =404;

            include fastcgi_params;
            fastcgi_param SCRIPT_FILENAME \$document_root\$fastcgi_script_name;
            fastcgi_param PATH_INFO \$path_info;
	        ${SSL_CERTIFICATE:+fastcgi_param HTTPS on;}

            fastcgi_param modHeadersAvailable true;         # Avoid sending the security headers twice
            fastcgi_param front_controller_active true;     # Enable pretty urls
            fastcgi_pass \$php_nextcloud;

            fastcgi_intercept_errors on;
            fastcgi_request_buffering off;

            fastcgi_max_temp_file_size 0;
        }

        location ${NEXTCLOUD_DIR} {
            try_files \$uri \$uri/ ${NEXTCLOUD_DIR}/index.php$request_uri;
        }
        ${COMMON_BLOCK}

    }


EOB

if [[ ${NEXTCLOUD_DIR} ]];then
    CUSTOM_BLOCK="${LOCAL_BLOCK}"
else
    CUSTOM_BLOCK="${NEXTCLOUD_BLOCK}"
fi

unset NEXTCLOUD_DIR LOCAL_BLOCK NEXTCLOUD_BLOCK
unset GZIP_BLOCK HEADERS_BLOCK MIME_BLOCK ROBOTS_BLOCK STATICFILES_BLOCK WOFF_BLOCK
