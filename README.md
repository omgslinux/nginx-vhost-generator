# nginx-vhost-generator
Bash scripts for generating config files for vhosts for nginx

These scripts intend to generate vhost config files, along with the links and log stuff
to have a kind of automated vhost generator for nginx.

Installation
============

* Clone the repository anywhere and copy the skeleton at the same level that /etc/nginx,
so there's a new directory.
* Copy the sslclient-fastcgi.conf file to the snippets dir
* Duplicate the defaults.inc_dist and name it defaults.inc. Set any defaults for your needs, theorically
common variables for most (or all) sites
* Check the example directory with its .inc file. Use it as template for your vhosts, modifying the values
for your host:

```
SERVER="example" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="" # Set any value to force redirect http to https
HTTP_ENV="dev" # For symfony, set the value for $APP_ENV when using http
HTTPS_PORT="443" # Port for https
HTTPS_ENV="prod" # For symfony, set the value for $APP_ENV when using https
CERTS="
    # Server certificate and key
    ssl_certificate /etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem;
" # Usual setup for letsencrypt
SSLCLIENT_FASTCGI="" # Set to any value for using fastcgi and client certificates
SSLCLIENT_VERIFY="
    ssl_client_certificate /etc/ssl/private/MyCA/CA.pem;
    ssl_verify_client optional;
" # The specific lines for CA certificate and client verification level in config file.
#SSLCLIENT_VERIFY ="" # Uncomment this line if you don't use CA verification
```

* Currently, the most used template is for symfony >4.x (i.e. root is public/ directory). Other webapps (nextcloud, etc)
will come, but PRs are welcome.
* When you are finished, you can run ./mkvhost.sh <dir>, which will:
    * Include any .inc file inside <dir>
    * Create/update the config file in ../sites-available, for both HTTP and HTTPS in the same file
    * Remove and recreate the symlink in ../sites-enabled
    * Create the specific ${LOGDIR} if it doesn't exist
* You can manually check the file, together with nginx -t to see if there's something wrong.
