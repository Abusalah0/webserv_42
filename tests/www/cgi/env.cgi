#!/bin/sh

echo "Status: 200 OK"
echo "Content-Type: text/html"
echo ""

cat << EOF
<html>
<head>
<title>ENV CGI</title>
</head>
<body>
<pre>
EOF

env

cat << EOF
</pre>
</body>
</html>
EOF