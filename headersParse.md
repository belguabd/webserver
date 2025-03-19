##### Request Line:

    Format: Method SP Request-Target SP HTTP-Version CRLF
    Example: GET /index.html HTTP/1.1\r\n
    Method: Token like GET, POST, PUT, etc.
    Request-Target: URI or authority-form or asterisk-form
    HTTP-Version: Must be "HTTP/1.1" for HTTP/1.1 requests

##### Headers:

    Format: Field-Name: Field-Value CRLF
    Multiple headers allowed
    Header section ends with empty line (CRLF)
    Field names are case-insensitive
    Leading/trailing whitespace in values should be stripped
    Field values may be split across multiple lines with a space/tab on continuation lines

###### Header Field Name Restrictions:

    Must be a "token"
        Cannot contain spaces, control characters, or separators like :()/<>@,;:\"{}
        Case-insensitive (though conventionally written with initial capitals)


| Header | Reason |
| ------ | ------ |

| `Host` | Must appear**exactly once**in HTTP/1.1 requests (RFC 9110, Section 7.2.1). |
| -------- | -------------------------------------------------------------------------------- |

| `Content-Length` | Must appear**at most once**(RFC 9110, Section 8.6.1). Multiple values create ambiguity. |
| ------------------ | --------------------------------------------------------------------------------------------- |

| `Transfer-Encoding` | Must appear**at most once**(RFC 9112, Section 6.1). |
| --------------------- | --------------------------------------------------------- |

| `Authorization` | Should only appear once (RFC 9110, Section 11.6.2). |
| ----------------- | --------------------------------------------------- |

| `Connection` | Multiple values cause contradictions (RFC 9110, Section 7.6.1). |
| -------------- | --------------------------------------------------------------- |

| `Date` | Must be unique because it represents a single timestamp (RFC 9110, Section 10.1.1). |
| -------- | ----------------------------------------------------------------------------------- |

| `Upgrade` | Must not be repeated as it defines a single protocol transition (RFC 9110, Section 7.8). |
| ----------- | ---------------------------------------------------------------------------------------- |

###### Header Field Value Restrictions:

    Can contain field content (any visible ASCII characters) plus spaces and horizontal tabs
        Control characters (including CR and LF) are not allowed except in specific circumstances:

    CR/LF may be present only when followed by space/tab (obsolete line folding)
        Horizontal tab (HT) is allowed

    Non-ASCII characters (like UTF-8) technically aren't allowed in RFC 9112 but are addressed in RFC 8941/6266 with special encoding

    Example of Valid Header:
            CopyContent-Type: text/html; charset=utf-8

###### Status Codes for Invalid Headers:

    400 Bad Request: When header field names or values contain illegal characters
            501 Not Implemented: When a header uses syntax that the server doesn't support

##### Message Body:

    Present if:
        Content-Length header exists
        Transfer-Encoding header exists
        Request method (like POST) semantically requires a body

##### Key Parsing Rules:

    Line terminators must be CRLF (carriage return + line feed)
    Headers end with an empty line (CRLF)
    Implementations must handle unexpected whitespace flexibly
    Request-line limited to 8000 octets
    Header fields should be limited (suggested 8000 octets)
    Headers like Content-Length and Transfer-Encoding determine how to read the body
