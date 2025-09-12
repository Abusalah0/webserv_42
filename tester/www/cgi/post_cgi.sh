#!/bin/bash

PREFIX=$(echo "$PATH_INFO"| cut -c -6)
if [ "$PREFIX" != "/post/" ]; then
	echo "Status: 405 Method Not Allowed"
	echo ""
	exit
fi

ROUTE=$DOCUMENT_ROOT$(echo "$PATH_INFO"| cut -c 6-)
if [ -d "$ROUTE" ]; then
	echo "Status: 405 Method Not Allowed"
	echo ""
	exit
fi

read -r -n $CONTENT_LENGTH POST_DATA
if ! echo -n $POST_DATA > $ROUTE; then
	echo "Status: 403 Forbidden"
	echo ""
	exit
fi

echo "Status: 201 Created"
echo "Transfer-Encoding: chunked"
echo "Content-Type: text/plain"
echo "Location: $PATH_INFO"
echo ""

echo "File was written to $ROUTE"