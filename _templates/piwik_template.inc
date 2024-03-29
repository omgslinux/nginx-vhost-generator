# Source: https://www.nginx.com/resources/wiki/start/topics/recipes/piwik/

# Inherit the generic php template for single point to check files

EXTRA_INDEX_LOCATION_BLOCK=""

. ${TEMPLATES_DIR}/php_template.inc

FASTCGI_PASS=${FASTCGI_PASS:-\$upstream_php}


PIWIK_BLOCK="
    limit_conn arbeit 10;

    # Disable all methods besides HEAD, GET and POST.
    if (\$request_method !~ ^(GET|HEAD|POST)\$ ) {
        return 444;
    }

    index index.php index.html;

    # Support for favicon. Return a 204 (No Content) if the favicon
    # doesn't exist.
    location = /favicon.ico {
             try_files /favicon.ico =204;
    }

    # Try all locations and relay to index.php as a fallback.
    location / {
             try_files \$uri /index.php;
    }

    # Relay all index.php requests to fastcgi.
    location ~* ^/(?:index|piwik)\.php\$ {
            fastcgi_pass ${FASTCGI_PASS};
    }

    # Return a 404 for protected directories
    location ~ /(?:config|tmp|vendor)/ {
              return 404;
    }

    # Any other attempt to access PHP files returns a 404.
    location ~* ^.*\.php\$ {
              return 404;
    }

    # Return a 404 for files and directories starting with a period. This includes directories used by version control systems
    location ~ /\. {
             return 404;
    }

    # Return a 404 for package manager config files
    location ~ (?:composer.json|composer.lock|bower.json)\$ {
             return 404;
    }

    # Return a 404 for all text files.
    location ~* (?:README|LICENSE|LEGALNOTICE|\.txt|\.md)\$ {
             return 404;
    }

    # # The 404 is signaled through a static page.
    # error_page  404  /404.html;

    # ## All server error pages go to 50x.html at the document root.
    # error_page 500 502 503 504  /50x.html;
    # location = /50x.html {
    #       root   /var/www/nginx-default;
    # }
"


CUSTOM_BLOCK="${PIWIK_BLOCK}"

unset PIWIK_BLOCK FASTCGI_PASS
