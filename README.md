# nginx-vhost-generator
Bash scripts for generating nginx vhost config files

These scripts intend to generate vhost config files, along with the links and log stuff
to have a kind of automated vhost generator for nginx.

The scenario is that you may want to have a site and have the chance to access via http and/or https,
and you find somehow hard to write, copy, etc new config from existing (or not) any other config file,
together with copying/pasting and other tasks that are error prone. Also, you may have to go to some website, search the web, etc and deal with
somehow stuff and sometimes difficult (or even wrong) instructions. Instead, you just have to define the necessary
parameters for the config file for your vhost type to be automatically generated for you.

Usage
=====
* Once you have created your vhost definitions, you just have to run <code>./mkvhost.sh &lt;dir&gt;</code> from the main directory.
You can specify multiple <code>&lt;dir&gt;</code> in the command line.

Installation
============
* Clone the repository anywhere (or from <code>/etc/nginx</code>). In any case, you will have to copy or move so the repository directory is <code>/etc/nginx/&lt;repo&gt;</code>.
* Duplicate the <code>defaults.inc_dist</code> file and name it <code>defaults.inc</code>, which will have your common default variables for any vhost.
* Set any defaults for your needs, theorically common variables for most (or all) sites

Templates
=========
* The vhost templates live in the <code>_templates/</code> directory using the formula <code>${VHOST_TYPE}_template.inc</code>. If you find a useful template is missing, create an issue about it. PRs are also welcome.

Examples
========
* Check the <code>_examples</code> directory to find an example for your VHOST_TYPE.
* Let's take a look at some variables in the symfony template:
```
VHOST_TYPE="symfony" # Any of the existing in _templates/ directory

SERVER="symfony" # Vhost used name
SUFFIX="example.org" # Suffix to add to ${SERVER} to have a FQDN
DOCROOT="/var/www/vhosts/${SERVER}" # The vhost document root
HTTP_PORT="80" # Port for http requests
HTTP_REDIRECT="" # Set any value to force redirect http to https
HTTP_ENV="dev" # For symfony, set the value for $APP_ENV when using http
HTTPS_PORT="443" # Port for https requests
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


Creating a vhost
================
* For your vhost definition, create a directory in the repo directory as your vhost name (in fact, it can be any name, but use the same naming conventions for your vhost and the directory with the definitions). Copy the example for your vhost in your vhost definition directory. Customize it
by modifying and/or commenting unused (or inherited from your <code>defaults.inc</code>) values.
* DO NOT FORGET to set the correct <code>VHOST_TYPE</code> in your file.
* Not included in the examples, you can define an <code>EXTRA_BLOCK</code> variable in your vhost definition for adding extra settings. That block will be added at the bottom of the generated config file.
* You can set the format for the log directory to something different to the default <code>${SERVER}.${SUFFIX}</code>, like <code>${SUFFIX}/${SERVER}</code>. You can do this by modifying the <code>LOGDIRFORMAT</code> variable in the <code>getLogDirFormat()</code> function in <code>defaults.inc</code>, or define <code>LOGDIRFORMAT</code> in single vhosts. The directory for logs will be <code>/var/log/nginx/$LOGDIRFORMAT</code>.
* If <code>SSL_CERTIFICATE</code> is empty or not set, there will be no https block
* For php and fastcgi, try to update your nginx installation after copying, so nginx variable <code>$upstream_php</code> points to
the default php fastcgi to be used in fastcgi_pass by default in the php sites.
* When you are finished editing the definition for your vhost, you can generate the config file by running <code>./mkvhost.sh &lt;dir&gt;</code>, which will:
    * Include any <code>*.inc</code> files inside &lt;dir&gt;
    * Some templates need extra config files in the <code>_conf/</code> directory that will be copied to <code>/etc/nginx/conf</code>
directory when necessary. Make sure that the copied files suit your needs and don't break anything. It's suggested that in case of conflict, use the copied definitions for other sites in order to avoid generated files not to work. If the necessary file already exists
in the target directory, no copy will take place.
    * Similarly, files in <code>_snippets/</code> directory will be copied when necessary to <code>/etc/nginx/snippets directory</code>.
    * Create/Overwrite the config file in <code>../sites-available/${SERVER}</code> (with the SERVER variable defined in your vhost), for both HTTP and HTTPS in the same file. If necessary, make sure to backup an existing file with your previous definition.
    * Remove and recreate the symlink <code>../sites-enabled/${SERVER}</code>
    * Create the specific directory for logs if it doesn't exist
* You can now manually check the generated config file, together with running <code>nginx -t</code> to see if there's something wrong.
