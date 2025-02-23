VHOST_TYPE="dokuwiki"

SERVER="dokuwiki" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="1" # Set any value to force redirect http to https
HTTPS_PORT="443" # Port for https

# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
SSL_VERIFY_CLIENT="optional"

# Set to any value for using fastcgi and client certificates.
# This will allow your app to check the client certificate validation process
SSLCLIENT_FASTCGI=""

# IMPORTANT: Before installing, set this to empty. After, set to anything and recreate host
DOKUWIKI_INSTALLED=""

# Set to 0 (default), 1 or 2, according to the $conf['userewrite'] setting in conf/local.php
DOKUWIKI_USEREWRITE="0"

# Recommended defaults for dokuwiki
CLIENT_MAX_BODY_SIZE="4M"
CLIENT_BODY_TIMEOUT="60";
