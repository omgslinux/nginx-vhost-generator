#!/bin/bash

if [[ -z $@ ]];then exit 1; fi

. init.inc

for VHOST in $@;do
	if [[ -d ${VHOST} ]];then
		initVars
        . defaults.inc
        for VHOSTFILE in $VHOST/*.inc;do
    		. ${VHOSTFILE}
        done

        processServers

		echo "# Generated http server block for ${VHOST} site
        ${HTTP_BLOCK}

# Generated https server block for ${VHOST} site
		${HTTPS_BLOCK}">../sites-available/${SERVER}
		if [ ! -d ${LOGDIR} ];then
			mkdir -p ${LOGDIR}
		fi

		rm ../sites-enabled/${SERVER} 2>/dev/null
		ln -s ../sites-available/${SERVER} ../sites-enabled/${SERVER}
		echo "Vhost ${VHOST} created, along with ${LOGDIR} and site-enabled symlink"
	fi
done
