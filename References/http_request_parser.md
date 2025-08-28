# References
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