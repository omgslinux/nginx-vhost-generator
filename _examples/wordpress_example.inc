# Template for a wordpress site in a backend
VHOST_TYPE="wordpress"

SERVER="wordpress" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN

# Basic vhost settings
DOCROOT="/var/www/wordpress" # The vhost document root
HTTP_PORT="80" # Port for http request

HTTP_REDIRECT="" # Set any value to force redirect http to https
HTTPS_PORT="443" # Port for https

# If not using https, comment the following lines
# Usual setup for letsencrypt
SSL_CERTIFICATE="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem"
SSL_CERTIFICATE_KEY="/etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem"

# The specific lines for CA certificate and client verification level in config file.
SSL_CLIENT_CERTIFICATE="/etc/ssl/private/CA.pem"
SSL_VERIFY_CLIENT="optional"

# End of basic vhost settings


## WORDPRESS SPECIFIC SETTINGS
# Set ONLY ONE OF THE FOLLOWING. Defaults to single site, which takes precedence when processing
WORDPRESS_SINGLESITE="1"
WORDPRESS_MULTISITE_SUBDIR=""
WORDPRESS_MULTISITE_SUBDOMAIN=""

# For single sites, set the subdir. Default is ""
WORDPRESS_SUBDIR=""

## END OF WORDPRESS SPECIFIC SETTINGS
