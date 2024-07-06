import socket

def send_query(query):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('localhost', 8080))
    client_socket.sendall(query.encode())
    response = client_socket.recv(1024).decode()
    client_socket.close()
    return response

if __name__ == "__main__":
    # Create a table
    response = send_query("CREATE_TABLE users")
    print(response)

    # Add columns to the table
    response = send_query("ADD_COLUMN users id")
    print(response)
    response = send_query("ADD_COLUMN users name")
    print(response)
    response = send_query("ADD_COLUMN users email")
    print(response)

    # Insert records into the table
    response = send_query("INSERT users 1 'John Doe' 'john@example.com'")
    print(response)
    response = send_query("INSERT users 2 'Jane Smith' 'jane@example.com'")
    print(response)

    # Query the table
    response = send_query("QUERY users")
    print(response)

