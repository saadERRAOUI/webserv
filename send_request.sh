#!/bin/bash

# Endpoint URL
URL="http://127.0.0.1:8080/test.html#index"

# Headers
HEADER_CONTENT_TYPE="Content-Type: application/json"
HEADER_CONTENT_TYPE="Transfer-encoding: chuncked"
HEADER_CUSTOM="X-Custom-Header: my-value"
HEADER_CUSTOM2="X-Custom-Header2: my-value2"

# JSON body
BODY='{
  "key1": "value1",
  "key2": "value2",
  "key3": "value3"
}'

# Send the request
curl -X POST "$URL" \
  -H "$HEADER_CONTENT_TYPE" \
  -H "$HEADER_CUSTOM" \
  -H "$HEADER_CUSTOM2" \
  -d "$BODY"
