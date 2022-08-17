# SimpleStreamer

SimpleStreamer provides Server and Publisher apps to stream any file from one place to another
Under the hood it uses TCP protocol for signaling and UDP for streaming
Boost is used for networking because of simplicity and variety of additional useful features
Review and contribution are welcomed!

## Signaling

Client side of signaling implemented in simple_publisher/HandshakeClient.cpp, server side in simple_server/HandshakeServer.cpp

## Streaming

Client side of streaming implemented in simple_publisher/Publisher.cpp, server side in simple_server/StreamServer.cpp

## Compilation

- Clone the repository: git clone https://github.com/tmb0208/SimpleStreamer.git
- Open CMakeLists.txt via your IDE, which support cmake or use CLI cmake
- !!! Don't forget to specify boost include path via cmake parameters (Boost_INCLUDE_DIR)
- Build using cmake generator (Makefile or ninja)
- That's it. Applications ready to use

## Usage

Server:
- Run: simple_server [path to file where to stream]

Publisher:
- Set env variable with server's secret key(for now it's 'secret_key': export PUBLISHER_SECRET=secret_key
- Run: simple_publisher [path to file to stream]

## Tech desicions
The applications implemented with performance, extendability in mind
Server and Publisher can be simply configured
For now it works in localhost as it was enough for POC, but it's easy to change hadcoded values or move address and signaling port to command line parameters in future
