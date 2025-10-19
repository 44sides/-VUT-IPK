#!/usr/bin/env python3
import socket
import sys

FORMAT = 'utf-8'
AGENT = "xtverd01"


def ADDR_str_to_pair(string):
    colon_index = string.index(':')

    return string[:colon_index], int(string[colon_index + 1:])


def NSP_REQUEST(addr, name):
    client_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_UDP.connect(addr)

    client_UDP.sendall(f"WHEREIS {name}".encode(FORMAT))

    recv_data = addr = client_UDP.recv(1024).decode(FORMAT)
    client_UDP.close()

    print(f" DNS RESPONSE:\n{recv_data}\n")

    addr = addr[3:]

    return ADDR_str_to_pair(addr)


def fsp_response_STATUS_DATA_extraction(response):
    status = response.split(b"\r\n\r\n")[0][8:(response.split(b"\r\n\r\n")[0].index(b'\r'))]
    data = response.split(b"\r\n\r\n")[1]

    return status, data


def FSP_REQUEST(filename, hostaddr, fspdomain, agent):
    client_TCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_TCP.connect(hostaddr)

    client_TCP.sendall(f"GET {filename} FSP/1.0\r\nHostname: {fspdomain}\r\nAgent: {agent}\r\n\r\n".encode(FORMAT))

    recv_response_l = [client_TCP.recv(512), client_TCP.recv(512)]
    response = b''.join(recv_response_l)
    client_TCP.close()

    print(f" HOST RESPONSE:\n{response}")

    Status_Data = fsp_response_STATUS_DATA_extraction(response)

    for c in filename:
        if c == '/':
            parts = filename.split("/")
            filename = parts[len(parts) - 1]

    if Status_Data[0] == b"Success" and filename != "index":
        f = open(f'{filename}', 'wb')
        f.write(Status_Data[1])
        f.close()

    return Status_Data[1]


def GET_ALL():
    index = FSP_REQUEST("index", FSP_ADDR, FSP_DOMAIN, AGENT)
    index = index.split(b"\r\n")
    index.pop(len(index) - 1)
    print(index)

    for file in index:
        FSP_REQUEST(file.decode(FORMAT), FSP_ADDR, FSP_DOMAIN, AGENT)


SERVER_ADDR = ADDR_str_to_pair(sys.argv[2])
Host_File = sys.argv[4][6:]

FSP_DOMAIN = Host_File[:Host_File.index('/')]
HOST_FILE = Host_File[Host_File.index('/') + 1:]

FSP_ADDR = NSP_REQUEST(SERVER_ADDR, FSP_DOMAIN)

if HOST_FILE == '*':
    GET_ALL()
else:
    FSP_REQUEST(HOST_FILE, FSP_ADDR, FSP_DOMAIN, AGENT)
