# Taken from https://docs.nextcloud.com/server/28/admin_manual/installation/nginx.html

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

GZIP_BLOCK="
    # Enable gzip but do not remove ETag headers
    gzip on;
    gzip_vary on;
    gzip_comp_level 4;
    gzip_min_length 256;
    gzip_proxied expired no-cache no-store private no_last_modified no_etag auth;
    gzip_types application/atom+xml application/javascript application/json application/ld+json application/manifest+json application/rss+xml application/vnd.geo+\$
    son application/vnd.ms-fontobject application/x-font-ttf application/x-web-app-manifest+json application/xhtml+xml application/xml font/opentype image/bmp image/s\$
    g+xml image/x-icon text/cache-manifest text/css text/plain text/vcard text/vnd.rim.location.xloc text/vtt text/x-component text/x-cross-domain-policy;
"

HEADERS_BLOCK='
    add_header Referrer-Policy                   "no-referrer"       always;
    add_header X-Content-Type-Options            "nosniff"           always;
    add_header X-Frame-Options                   "SAMEORIGIN"        always;
    add_header X-Permitted-Cross-Domain-Policies "none"              always;
    add_header X-Robots-Tag                      "noindex, nofollow" always;
    add_header X-XSS-Protection                  "1; mode=block"     always;
'

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

STATICFILES_BLOCK="
    # Serve static files. This block must be AFTER the php handler block
    location ~ \.(?:css|js|mjs|svg|gif|png|jpg|ico|wasm|tflite|map|ogg|flac)\$ {
        try_files \$uri /index.php\$request_uri;
        ${HEADERS_BLOCK}
        access_log off;     # Optional: Don't log access to assets
    }

"

WEBFINGER_BLOCK="
    # The following 2 rules are only needed for the user_webfinger app.
    # Uncomment it if you are planning to use this app.
    #rewrite ^/.well-known/host-meta /public.php?service=host-meta last;
    #rewrite ^/.well-known/host-meta.json /public.php?service=host-meta-json
    # last;
"

WOFF_BLOCK="
    location ~ \.woff2?\$ {
        try_files \$uri /index.php\$request_uri;
        expires 7d;         # Cache-Control policy borrowed from '.htaccess'
        access_log off;     # Optional: Don't log access to assets
    }
"

NEXTCLOUD_BLOCK="

    ${HEADERS_BLOCK}
    ${MIME_BLOCK}
    ${ROBOTS_BLOCK}
    ${GZIP_BLOCK}
    ${WEBFINGER_BLOCK}

    location = /.well-known/carddav {
      return 301 \$scheme://\$host/remote.php/dav;
    }

    location = /.well-known/caldav {
      return 301 \$scheme://\$host/remote.php/dav;
    }

    # set max upload size
    client_max_body_size 512M;
    fastcgi_buffers 64 4K;


    # Uncomment if your server is build with the ngx_pagespeed module
    # This module is currently not supported.
    #pagespeed off;

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
        fastcgi_pass \$upstream_php;
        fastcgi_intercept_errors on;
        fastcgi_request_buffering off;
        fastcgi_read_timeout 300;
    }

    location ~ ^/(?:updater|ocs-provider)(?:\$|/) {
        try_files \$uri/ =404;
        index index.php;
    }

    # Rule borrowed from .htaccess
    location /remote {
        return 301 /remote.php\$request_uri;
    }

    ${STATICFILES_BLOCK}
    ${WOFF_BLOCK}
"

CUSTOM_BLOCK="${NEXTCLOUD_BLOCK}"

unset GZIP_BLOCK HEADERS_BLOCK MIME_BLOCK ROBOTS_BLOCK STATICFILES_BLOCK WOFF_BLOCK
