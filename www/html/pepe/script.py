import sys

def send_headers():
    sys.stdout.write("Status: 200 OK\r\n")
    sys.stdout.write("Content-Type: text/plain\r\n")
    sys.stdout.write("Transfer-Encoding: chunked\r\n")
    sys.stdout.write("\r\n")  # End of headers

def send_chunk(data):
    chunk = data.encode('utf-8')
    size = f"{len(chunk):X}\r\n"
    sys.stdout.write(size)
    sys.stdout.write(data + "\r\n")
def send_chunked_response():
    send_headers()
    send_chunk("hello world!")
    send_chunk("i love me some pussy oh my god!!!")
    sys.stdout.write("0\r\n\r\n")  # Proper ending
    sys.stdout.flush()

send_chunked_response()
