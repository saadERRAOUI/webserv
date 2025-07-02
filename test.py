import socket

def send_chunked():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 8080))
    
    request = "POST /test HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nTransfer-Encoding: chunked\r\nContent-Type: text/plain\r\n\r\n"
    sock.send(request.encode())
    
    chunks = [
        "First chunk of data",
        "Second chunk here", 
        "Third and final chunk"
    ]
    
    for chunk in chunks:
        chunk_size = hex(len(chunk))[2:] + "\r\n"
        sock.send(chunk_size.encode())
        sock.send((chunk + "\r\n").encode())
    
    sock.send("0\r\n\r\n".encode())  # End chunks
    
    response = sock.recv(4096)
    print(response.decode())
    sock.close()

send_chunked()