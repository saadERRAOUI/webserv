#!/bin/bash

# Endpoint URL
URL="http://127.0.0.1:8080/api/v1/tree.jpg"

# Headers
HEADER_CONNECTION="Connection: close"
HEADER_ACCEPT_ENCODING="accept-encoding: gzip, deflate, br"
HEADER_HOST="Host: example.com"
# JSON body
BODY=''

# Send the request
curl -X GET "$URL" \
  -H "$HEADER_CONNECTION" \
  -H "$HEADER_ACCEPT_ENCODING" \
  -H "$HEADER_HOST" \
  > image.jpg
