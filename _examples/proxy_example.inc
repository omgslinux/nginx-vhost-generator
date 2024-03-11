# Template for a proxy. Use this when you have a frontend and just need to use a proxy_pass to an upstream backend
VHOST_TYPE="proxy"
#. proxy_common.conf_template

SERVER="proxy" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http request
HTTP_REDIRECT="1" # Set any value to force redirect http to https
HTTPS_PORT="443" # Port for https

# Usual setup for letsencrypt certificates.
CERTS="
    # Server certificate and key
    ssl_certificate /etc/letsencrypt/live/${SERVER}.${SUFFIX}/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/${SERVER}.${SUFFIX}/privkey.pem;
"
# The specific lines for CA certificate and client verification level in config file.
SSLCLIENT_VERIFY="
    ssl_client_certificate /etc/ssl/private/CA.pem;
    ssl_verify_client optional;
"
#SSLCLIENT_VERIFY ="" # Uncomment this line to overwrite the above if you don't use CA verification

# Parameter for proxy_pass in / location. ONLY IN FRONTEND
PROXY_PASS="192.168.55.105:8080"
