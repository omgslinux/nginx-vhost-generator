# Template for a nextcloud in a self hosted vhost
VHOST_TYPE="nextcloud"

SERVER="nextcloud" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/nextcloud" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="1" # Set any value to force redirect http to https
HTTPS_PORT="443" # Port for https

# ONLY if nextcloud is installed in a subdirectory of an existing vhost,
# where probably DOCROOT above should be the root value of the existing.
NEXTCLOUD_DIR=""

# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
SSL_VERIFY_CLIENT="optional"

# Set to any value for using fastcgi and client certificates.
# This will allow your app to check the client certificate validation process
SSLCLIENT_FASTCGI=""
