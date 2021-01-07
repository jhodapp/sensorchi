#include "http_client.h"


HttpClient::HttpClient(IPAddress ip, uint16_t port)
 : ip(ip), port(port), use_host(false)
{
}

HttpClient::HttpClient(const String &host, uint16_t port)
 : host(host), port(port), use_host(true)
{
}

HttpClient::HttpClient()
 : port(80), use_host(false)
{
}

HttpClient::~HttpClient()
{
    client.stop();
}

bool HttpClient::connect()
{
    // If already connected, don't try to connect again
    if (client.connected())
        return true;

    if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 && host.length() == 0) {
        Log.error("Can't connect, ip nor host are set.");
        return false;
    }

    if (use_host) {
        return client.connect(host, port);
    } else if (ip[0] > 0) {
        return client.connect(ip, port);
    } else {
        return false;
    }
}

bool HttpClient::available()
{
    return client.available();
}

const char HttpClient::read()
{
    return client.read();
}

void HttpClient::setEnpoint(const String &endpoint)
{
    this->endpoint = endpoint;
}

bool HttpClient::sendJson(const JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> &json)
{
    Log.info("Sending POST request:");
    Log.info("POST %s HTTP/1.1", endpoint.c_str());
    Log.info("Host: %s:%d", (use_host ? host.c_str() : ip.toString().c_str()), port);
    Log.info("User-Agent: argon/0.0.1");
    Log.info("Accept: */*");
    Log.info("Content-Type: application/json");
    Log.info("Content-Length: %d", payloadLength(json));
    Log.info(json.getBuffer());

    client.print("POST ");
    client.print(endpoint);
    client.print(" HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(use_host ? host : ip.toString());
    client.print(port);
    client.print("\r\n");
    client.print("User-Agent: argon/0.0.1\r\n");
    client.print("Accept: */*\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.print(payloadLength(json));
    client.print("\r\n");
    client.print("\r\n");
    client.print(json.getBuffer());
    client.print("\r\n");

    return true;
}

uint16_t HttpClient::payloadLength(const JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> &json)
{
    const char *buf = json.getBuffer();
    for (uint16_t i=0; i<JSON_WRITER_BUFFER_SIZE; i++) {
        if (buf[i] == '}')
            return i+1;
    }
}