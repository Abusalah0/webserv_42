#!/bin/bash

PREFIX=$(echo "$PATH_INFO"| cut -c -8)
if [ "$PREFIX" != "/delete/" ]; then
	echo "Status: 405 Method Not Allowed"
	echo ""
	exit
fi

ROUTE=$DOCUMENT_ROOT$(echo "$PATH_INFO"| cut -c 8-)

if [ "${ROUTE: -1}" == "/" ]; then
	echo "Status: 405 Method not allowed"
	echo ""
	exit
fi

if [ -d "$ROUTE" ]; then
	echo "Status: 418 I'm a teapot"
	echo ""
	exit
fi

if [ ! -f "$ROUTE" ]; then
	echo "Status: 404 Not Found"
	echo ""
	exit
fi

if ! rm -f $ROUTE; then
	echo "Status: 403 Forbidden"
	echo ""
	exit
fi

echo "Status: 204 No Content"
echo ""