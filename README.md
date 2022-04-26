# 3 Way HTTP Server

This is a lightweight HTTP server on Ubuntu operating system, that provides some host system information: 

- Host name
- CPU name
- CPU load. 

The client sends one of three HTTP requests depending on the endpoint of the URL. The client can use GET, wget or curl commands, as well as a browser, to send the request. 

## Getting Started 

You can deploy the server on your device. To do this, you just need to have Ubuntu. 

Also use it for testing and development purposes, having full source code.

### Prerequisites
- Linux Ubuntu 20.04 LTS (Not tested on other distributions)
- IDE For C and C++ (For use as open-source)

## Usage
### To launch the server: 
```bash
./hinfoscv 'port'
```
Specify the port on which the server will listen.

### Urls for the client to make requests:
```bash
http://servername:port/hostname
```
```bash
http://servername:port/cpu-name
```
```bash
http://servername:port/load
```
A few examples:
```c
1. Server side:
./hinfoscv 8080
2. Client side:
GET http://localhost:8080/load
3. Server side:
...received HTTP header...

Message sent! (load)
4. Client side:
8.25%
```
```c
1. Server side:
./hinfoscv 12345
2. Client side:
browser -> http://localhost:12345/cpu-name
3. Server side:
...received HTTP header...

Message sent! (cpu-name)
4. Client side: 
Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz 

5. Client side:
curl http://localhost:12345/time
6. Server side:
...received HTTP header...

400 BAD REQUEST sent!
7. Client side: 
Bad Request 
```


### Author
Vladyslav Tverdokhlib
(xtverd01@vutbr.cz)
