import socket
import threading

def receive(sock):
    while True:
        try:
            data = sock.recv(1024)
            if data:
                print("\nServer:", data.decode())
            else:
                break
        except:
            break

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 8080))  # Replace with the IP address and port number of the server

# Start a thread to receive messages from the server
t = threading.Thread(target=receive, args=(sock,))
t.start()

while True:
    message = input()
    sock.send(message.encode())
    if message == "exit":
        break

sock.close()
t.join()
