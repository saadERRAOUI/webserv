def send_headers():
	# Status should always be the first header
	print("Status: 200 OK")
	
	# Content headers
	# print("Content-Length: 222")
	# print("Content-Type: text/html")
	# print("Content-Disposition: attachment; filename=pic.py")
	# print("Content-Type: page/html")
	# Empty line to separate headers from body
	print("\r\n\r\n")
	print("123 \n hello world!")

# Call the function
send_headers()