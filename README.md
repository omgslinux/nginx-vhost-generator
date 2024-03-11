# nginx-vhost-generator
Bash scripts for generating config files for vhosts for nginx

These scripts intend to generate vhost config files, along with the links and log stuff
to have a kind of automated vhost generator for nginx.

The scenario is that you may want to have a site and have the chance to access via http and/or https,
and you find somehow hard to write, copy, etc new config from existing (or not) any other config file,
together with copying/pasting and other tasks that are error prone. So, you just have to define the necessary
parameters for the config files to be automatically generated for you.

Installation
============

* Clone the repository anywhere and copy the skeleton at the same level that /etc/nginx,
so there's a new directory in that tree.
* Copy the sslclient-fastcgi.conf file into the snippets dir. Optionally, take a look at the supplied php.conf and symfony.conf
files, and copy them to conf.d for the symfony scripts to work out-of-the box. You can then suit them to your needs.
* Duplicate the defaults.inc_dist file and name it defaults.inc. Set any defaults for your needs, theorically
common variables for most (or all) sites
* Check the _examples directory. You can use any of the files as template for your vhosts, modifying the values, like these:

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

* DO NOT FORGET to set the correct VHOST_TYPE in your file
* Currently, the most used template is for symfony >4.x (i.e. webroot is public/ directory). Other webapps and configs
(nextcloud, wordpress, etc) will come. PRs are also welcome.
* When you are finished, you can generate the config file by running ./mkvhost.sh <dir>, which will:
    * Include any .inc file inside <dir>
    * Create/Overwrite the config file in ../sites-available, for both HTTP and HTTPS in the same file
    * Remove and recreate the symlink in ../sites-enabled
    * Create the specific ${LOGDIR} if it doesn't exist
* You can now manually check the generated config file, together with nginx -t to see if there's something wrong.
