# References
https://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_form
https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers
https://httpwg.org/specs/rfc9110.html
https://httpwg.org/specs/rfc9112.html

# Request Line
```
request-line   = method SP request-target SP HTTP-version
```
# Request Header
`field-name     = token`

# Field Syntax
```field-line   = field-name ":" OWS field-value OWS```

# Field Values
```
field-value    = *field-content
field-content  = field-vchar
                   [ 1*( SP / HTAB / field-vchar ) field-vchar ]
field-vchar    = VCHAR / obs-text
obs-text       = %x80-FF
```
# Token
```
token          = 1*tchar

tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
                 / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
                 / DIGIT / ALPHA
                 ; any VCHAR, except delimiters
```

# Whitespaces
```
OWS            = *( SP / HTAB )
                 ; optional whitespace
RWS            = 1*( SP / HTAB )
                 ; required whitespace
BWS            = OWS
                 ; "bad" whitespace
```

# HTTP Version
```
HTTP-version  = HTTP-name "/" DIGIT "." DIGIT
HTTP-name     = %s"HTTP"
```

# Request Target
```
request-target = origin-form
                 / absolute-form
                 / authority-form
                 / asterisk-form
```

# Standard Request Header Fields
## Content-Length
```
Content-Length    = "Content-Length" ":" 1*DIGIT
```
## Connection
```
Connection = "Connection" ":" 1#(connection-token)
connection-token  = token
```
## Host
```
Host = "Host" ":" host [ ":" port ]
```
## Transfer-Encoding
```
Transfer-Encoding       = "Transfer-Encoding" ":" 1#transfer-coding
```
## Cookie
```
Cookie: <cookie-list>
Cookie: name=value
Cookie: name=value; name2=value2; name3=value3
```