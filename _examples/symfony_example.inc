# First, define the vhost type. It has to be one of _templates/*_template.inc
VHOST_TYPE="symfony"

# Then, customize the vhost
SERVER="symfony" # Vhost used name
# Suffix to add to ${SERVER} to have a FQDN. Uncomment if different from defaults.inc
SUFFIX="example.org"

DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="" # Set any value to force redirect http to https
HTTP_ENV="dev" # For symfony, set the value for $APP_ENV when using http
HTTPS_PORT="443" # Port for https
HTTPS_ENV="prod" # For symfony, set the value for $APP_ENV when using https

# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
SSL_VERIFY_CLIENT="optional"

# Set to any value for using fastcgi and client certificates.
# This will allow your app to check the client certificate validation process
SSLCLIENT_FASTCGI=""

# Index setting. If empty, defaults to index.php. For old symfony versions, set to app.php
INDEX_BLOCK=""
