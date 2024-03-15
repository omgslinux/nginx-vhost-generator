# Source https://www.nginx.com/resources/wiki/start/topics/recipes/redmine/

# Lets make sure that there's a redmine upstream definition to be used below
# You can modify the copied file for your needs
if [[ ! -f ../conf.d/redmine.conf ]]; then
    cp -p ${CONFS_DIR}/redmine.conf ../conf.d
fi

PROXY_PASS=${PROXY_PASS:-http://redmine}

REDMINE_BLOCK="
        location / {
                try_files \$uri @redmine;
        }

        location @redmine {
                proxy_set_header X-Forwarded-For \$remote_addr;
                proxy_pass ${PROXY_PASS};
        }
"

CUSTOM_BLOCK="${REDMINE_BLOCK}"

unset REDMINE_BLOCK PROXY_PASS
