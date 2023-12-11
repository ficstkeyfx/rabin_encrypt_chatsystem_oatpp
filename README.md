# Rabin-Cryptosystem-Chat [![Build Status](https://dev.azure.com/lganzzzo/lganzzzo/_apis/build/status/lganzzzo.canchat?branchName=master)](https://dev.azure.com/lganzzzo/lganzzzo/_build/latest?definitionId=24&branchName=master)

![Rabin-Cryptosystem-Chat](https://upload.wikimedia.org/wikipedia/commons/0/0a/Tr%C3%A5dtelefon-illustration.png)

Feature-complete rooms-based chat for tens of thousands users. Client plus Server. Built with [Oat++ Web Framework](https://oatpp.io/).   
`Single-Node` `In-Memory` `WebSocket` `TLS`.


## Chat

### Rooms

Sử dụng hệ mật mã Rabin để mã hóa tin nhắn đầu cuối

## Build And Run

### Build From Source

#### Pre-Requirements

- Cài đặt `LibreSSL` phiên bản `3.0.0` hoặc mới hơn
- Cài đặt [oatpp](https://github.com/oatpp/oatpp) - Oat++ Web Framework 
- Cài đặt [oatpp-websocket](https://github.com/oatpp/oatpp-websocket) 
- Cài đặt [oatpp-libressl](https://github.com/oatpp/oatpp-libressl) 

*Để cài đặt oatpp modules có thể sử dụng `utility/install-oatpp-modules.sh` script*

#### Build Chat Server

```bash
$ cd <this-repo>/server/
$ mkdir build/ && cd build/
$ cmake ..
$ make
```

Now run chat server

```bash
$ canchat-exe --host localhost --port 8443
```

### Docker

```bash
$ docker build -t canchat .
$ docker run -p 8443:8443 -e EXTERNAL_ADDRESS=localhost -e EXTERNAL_PORT=8443 -it canchat
```

---