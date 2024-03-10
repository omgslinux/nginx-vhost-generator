SERVER="example" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="" # Set any value to force redirect http to https
HTTP_ENV="dev" # For symfony, set the value for $APP_ENV when using http
HTTPS_PORT="8080" # Port for https
HTTPS_ENV="prod" # For symfony, set the value for $APP_ENV when using https
SSLCLIENT_FASTCGI="" # Set to any value for using fastcgi and client certificates
SSLCLIENT_VERIFY="
    ssl_client_certificate /etc/ssl/private/MyCA/CA.pem;
    ssl_verify_client optional;
" # The specific lines for CA certificate and client verification level in config file.
#SSLCLIENT_VERIFY ="" # Uncomment this line if you don't use CA verification




# Here just include the template
. symfony_common.conf_template

# Final assign the result to $CUSTOM_BLOCK variable
CUSTOM_BLOCK="${SYMFONY_BLOCK}"
