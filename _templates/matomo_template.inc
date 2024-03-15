# Source: https://github.com/matomo-org/matomo-nginx/blob/master/sites-available/matomo.conf

# Inherit the generic php template

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

FASTCGI_PASS=${FASTCGI_PASS:-\$upstream_php}

MATOMO_BLOCK="
    index index.php;

    ## uncomment if you want to enable HSTS with 6 months cache
    ## ATTENTION: Be sure you know the implications of this change (you won't be able to disable HTTPS anymore)
    #add_header Strict-Transport-Security max-age=15768000 always;

    add_header Referrer-Policy origin always; # make sure outgoing links don't show the URL to the Matomo instance
    add_header X-Content-Type-Options \"nosniff\" always;
    add_header X-XSS-Protection \"1; mode=block\" always;

    ## only allow accessing the following php files
    location ~ ^/(index|matomo|piwik|js/index|plugins/HeatmapSessionRecording/configs)\.php\$ {
        include snippets/fastcgi-php.conf;
        fastcgi_param HTTP_PROXY ''; # prohibit httpoxy: https://httpoxy.org/
        fastcgi_pass ${FASTCGI_PASS};
    }

    ## deny access to all other .php files
    location ~* ^.+\.php\$ {
        deny all;
        return 403;
    }

    ## serve all other files normally
    location / {
        try_files \$uri \$uri/ =404;
    }

    ## disable all access to the following directories
    location ~ ^/(config|tmp|core|lang) {
        deny all;
        return 403; # replace with 404 to not show these directories exist
    }

    location ~ /\.ht {
        deny  all;
        return 403;
    }

    location ~ js/container_.*_preview\.js\$ {
        expires off;
        add_header Cache-Control 'private, no-cache, no-store';
    }

    location ~ \.(gif|ico|jpg|png|svg|js|css|htm|html|mp3|mp4|wav|ogg|avi|ttf|eot|woff|woff2)$ {
        allow all;
        ## Cache images,CSS,JS and webfonts for an hour
        ## Increasing the duration may improve the load-time, but may cause old files to show after an Matomo upgrade
        expires 1h;
        add_header Pragma public;
        add_header Cache-Control \"public\";
    }

    location ~ ^/(libs|vendor|plugins|misc|node_modules) {
        deny all;
        return 403;
    }

    ## properly display textfiles in root directory
    location ~/(.*\.md|LEGALNOTICE|LICENSE) {
        default_type text/plain;
    }
"

CUSTOM_BLOCK="${MATOMO_BLOCK}"

unset MATOMO_BLOCK FASTCGI_PASS
