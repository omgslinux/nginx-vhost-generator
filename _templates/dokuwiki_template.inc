# Source: https://www.dokuwiki.org/install:nginx

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

if [[ ${DOKUWIKI_USEREWRITE} == "0" ]];then
    ROOT_LOCATION_BLOCK="try_files \$uri \$uri/ =404;"
else
    ROOT_LOCATION_BLOCK="try_files \$uri \$uri/ @dokuwiki;"
    DOKUWIKI_URL_REWRITE="rewrite ^/doku.php/(.*) /doku.php?id=\$1&\$args last;"
    if [[ ${DOKUWIKI_USEREWRITE} == "1" ]];then
        DOKUWIKI_URL_REWRITE="rewrite ^/(.*) /doku.php?id=\$1&\$args last;"
    fi
    DOKUWIKI_URL_REWRITE_BLOCK="
    location @dokuwiki {
    rewrite ^/_media/(.*) /lib/exe/fetch.php?media=\$1 last;
    rewrite ^/_detail/(.*) /lib/exe/detail.php?media=\$1 last;
    rewrite ^/_export/([^/]+)/(.*) /doku.php?do=export_\$1&id=\$2 last;
    ${DOKUWIKI_URL_REWRITE}
    }
    "
fi
unset DOKUWIKI_URL_REWRITE

if [[ ${DOKUWIKI_INSTALLED} ]];then
    DOKUWIKI_INSTALL_BLOCK="
    # Comment out while installing, then uncomment
    location ~ /(install.php) { deny all; }
"
fi

DOKUWIKI_BLOCK="
    index  doku.php;

# HEADERS ######### ######### ######### ######### ######### ######### ##########
    # Information "leaks"
    server_tokens       off;
    fastcgi_hide_header X-Powered_by;

# TWEAKS ######### ######### ######### ######### ######### ######### #######  ##

    client_body_buffer_size 128k;

    location ~ ^/lib.*\.(js|css|gif|png|ico|jpg|jpeg|svg)$ {
        expires 365d;   # browser caching
    }

# RESTRICT ACCESS ######### ######### ######### ######### ######### ######### ##
    # Reference: https://www.dokuwiki.org/security#deny_directory_access_in_nginx
                 # TODO: Compare with this

    ${DOKUWIKI_INSTALL_BLOCK}

    # .ht             - .htaccess, .htpasswd, .htdigest, .htanything
    # .git, .hg, .svn - Git, Mercurial, Subversion.
    # .vs             - Visual Studio (Code)
    # All directories except lib.
    # All "other" files that you don't want to delete, but don't want public.

    location ~ /(\.ht|\.git|\.hg|\.svn|\.vs|data|conf|bin|inc|vendor|README|VERSION|SECURITY.md|COPYING|composer.json|composer.lock) {
        #return 404; # https://www.dokuwiki.org/install:nginx?rev=1734102057#nginx_particulars
        deny all;    # Returns 403
    }

######################
    # (2024-08): Is this actually a default feature or does
    # this belong in a seperate section? Disable by default?

    # Support for X-Accel-Redirect
    location ~ ^/data/ {
       internal;
    }
########################

# REDIRECT & PHP ### ######### ######### ######### ######### ######### #########

    location / {
        ${ROOT_LOCATION_BLOCK}

        # This means; where \$uri is 'path', if 'GET /path' doesnt exist, redirect
        # client to 'GET /path/' directory. If neither, goto @dokuwiki rules.
    }

    ${DOKUWIKI_URL_REWRITE_BLOCK}

    location ~ \.php$ {
        #try_files \$uri \$uri/ /doku.php;

        include       fastcgi_params;
        fastcgi_param SCRIPT_FILENAME \$document_root\$fastcgi_script_name;
        fastcgi_param REDIRECT_STATUS 200;

        fastcgi_pass \$upstream_php;
    }

"

CUSTOM_BLOCK="${DOKUWIKI_BLOCK}"

unset DOKUWIKI_BLOCK DOKUWIKI_INSTALL_BLOCK DOKUWIKI_INSTALLED DOKUWIKI_URL_REWRITE_BLOCK
