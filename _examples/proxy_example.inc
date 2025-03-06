# Template for a proxy. Use this when you have a frontend and just need to use a proxy_pass to an upstream backend
VHOST_TYPE="proxy"

SERVER="proxy" # Vhost used name
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

# Parameter for proxy_pass in / location. ONLY IN FRONTEND
PROXY_PASS="http://192.168.55.105:8080"

# Settings for several general options, to be overriden when necessary
CLIENT_MAX_BODY_SIZE="512M" #   client_max_body_size 512M;
CLIENT_BODY_TIMEOUT="300s"  #   client_body_timeout 300s;
FASTCGI_BUFFERS="64 4K"     #   fastcgi_buffers 64 4K;

# Set to any value if using websockets. Nextcloud recommends setting this
USE_WEBSOCKET=""

# Location of .well-known directory for certbot. Setting it will create a specific location entry
#WELLKNOWN_DIR="/var/www/html"
