# Media Types Task

## Function Prototype
`const char* get_media_type(const std::string& file_path)`
## Function Description
The function will get a file path for example:
`/root/file.html`
It should extract the extension and get the media type for the provided extension, There are hundreds of media types, only the ones mentioned in the following document must be handled: [Common media types](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/MIME_types/Common_types)
If extension type is not in the handled range then the function will return `application/octet-stream`.