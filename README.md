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

# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${VHOST}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${VHOST}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
SSL_VERIFY_CLIENT="optional"

# Set to any value for using fastcgi and client certificates.
# This will allow your app to check the client certificate validation process
SSLCLIENT_FASTCGI=""
```

* Currently, the most used template is for symfony >4.x (i.e. webroot is public/ directory). There's also a proxy
template to be used in frontend. Last news is a nextcloud template for backend. Other webapps and configs will come.
The templates live in the _templates/ directory. PRs are also welcome.
* DO NOT FORGET to set the correct VHOST_TYPE in your file
* You can set the format for the log directory to something different to ${SERVER}.${SUFFIX}, like ${SUFFIX}/${SERVER}.
You can do this by modifying the LOGDIRFORMAT variable in getLogDirFormat function in defaults.inc,
or define LOGDIRFORMAT in single vhosts. The directory for logs will be /var/log/nginx/$LOGDIRFORMAT.
* If ${SSL_CERTIFICATE} is empty or not set, there will be no https block
* When you are finished, you can generate the config file by running ./mkvhost.sh <dir>, which will:
    * Include any .inc file inside <dir>
    * Create/Overwrite the config file in ../sites-available, for both HTTP and HTTPS in the same file
    * Remove and recreate the symlink in ../sites-enabled
    * Create the specific ${LOGDIR} if it doesn't exist
* You can now manually check the generated config file, together with nginx -t to see if there's something wrong.
