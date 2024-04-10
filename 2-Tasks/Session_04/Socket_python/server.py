import socket
import threading

def receive(sock):
    while True:
        try:
            data = sock.recv(1024)
            if data:
                print("\nClient:", data.decode())
            else:
                break
        except:
            break

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('127.0.0.1', 8080))  # Replace with the IP address and port number of your choice
sock.listen(5)

conn, addr = sock.accept()

# Start a thread to receive messages from the client
t = threading.Thread(target=receive, args=(conn,))
t.start()

while True:
    message = input()
    conn.send(message.encode())
    if message == "exit":
        break

sock.close()
conn.close()
t.join()
