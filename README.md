# Rabin-Cryptosystem-Chat
Xây dựng dựa trên [Oat++ Web Framework](https://oatpp.io/).   
`Single-Node` `In-Memory` `WebSocket` `TLS`.


## Chat

### Rooms

Sử dụng hệ mật mã Rabin để mã hóa tin nhắn đầu cuối tin nhắn mã hóa sẽ được hiển thị trong console của server.

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
*Hoặc có thể tải bản release trong repo để vào thư mục /server/build/

### Docker

```bash
$ docker build -t canchat .
$ docker run -p 8443:8443 -e EXTERNAL_ADDRESS=localhost -e EXTERNAL_PORT=8443 -it canchat
```

---
