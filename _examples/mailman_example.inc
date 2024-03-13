# Template for a proxy. Use this when you have a frontend and just need to use a proxy_pass to an upstream backend
VHOST_TYPE="mailman"

SERVER="mailman" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/usr/lib" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="1" # Set any value to force redirect http to https
HTTPS_PORT="443" # Port for https

# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
#SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
#SSL_VERIFY_CLIENT="optional"

# Mailman specific variables
MAILMAN_DATA_DIR="/var/lib/mailman"
MAILMAN_INTERNAL_URL="http://127.0.0.1/"
