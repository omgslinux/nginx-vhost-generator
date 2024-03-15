# Inherit the generic php template

EXTRA_INDEX_LOCATION_BLOCK="    fastcgi_param APP_ENV \$app_env;"

. ${TEMPLATES_DIR}/php_template.inc

SYMFONY_BLOCK="
${CUSTOM_BLOCK}
"

CUSTOM_BLOCK="${SYMFONY_BLOCK}"

unset SYMFONY_BLOCK
