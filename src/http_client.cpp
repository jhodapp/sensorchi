#include "http_client.h"


HttpClient::HttpClient(IPAddress ip, uint16_t port)
 : ip(ip), port(port), use_host(false),
   buf_idx(0), received_full_response(false)
{
}

HttpClient::HttpClient(const String &host, uint16_t port)
 : host(host), port(port), use_host(true),
   buf_idx(0), received_full_response(false)
{
}

HttpClient::HttpClient()
 : port(80), use_host(false),
   buf_idx(0), received_full_response(false)
{
}

HttpClient::~HttpClient()
{
    client.stop();

#if 0
    if (response_buf != nullptr)
        free(response_buf);
#endif
}

bool HttpClient::connect()
{
    Log.info("HttpClient::connect()");
    // If already connected, don't try to connect again
    if (client.connected()) {
        Log.warn("Connect called after client already connected to endpoint.");
        return true;
    }

    if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 && host.length() == 0) {
        Log.error("Can't connect, ip nor host are set.");
        return false;
    }

    if (use_host) {
        Log.info("Connecting to: %s:%d", host, port);
        return client.connect(host, port);
    } else if (ip[0] > 0) {
        Log.info("Connecting to: %d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], port);
        return client.connect(ip, port);
    } else {
        Log.warn("Not connecting, host or ip not specified.");
        return false;
    }

#if 0
    // Allocate a buffer to hold HTTP responses from the server
    if (response_buf == nullptr)
        response_buf = static_cast<char *>(malloc(RESPONSE_BUFFER_SIZE));

    if (!response_buf)
        Log.error("** Failed to allocate HTTP response_buf **");
    else
        Log.info("** Successfully allocated %d bytes of the heap **", RESPONSE_BUFFER_SIZE);
#endif
}

bool HttpClient::connected()
{
    return client.connected();
}

bool HttpClient::available()
{
    return client.available();
}

const char HttpClient::read()
{
    const char c = client.read();
    if (buf_idx < RESPONSE_BUFFER_SIZE) {
        response_buf[buf_idx] = c;
        Log.info("response_buf[%d]: %c", buf_idx, response_buf[buf_idx]);

        // TODO: process the HTTP response
        if (httpResponseEnd()) {
            Log.info("Received HTTP response end");
            received_full_response = true;
        }

        if (buf_idx < RESPONSE_BUFFER_SIZE)
            buf_idx++;
        else {
            std::memset(response_buf, 0, RESPONSE_BUFFER_SIZE);
            buf_idx = 0;
        }
    }
    return c;
}

void HttpClient::setEnpoint(const String &endpoint)
{
    this->endpoint = endpoint;
}

bool HttpClient::sendJson(const JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> &json)
{
    if (!connected()) {
        Log.warn("Failed to send JSON payload, not connected to endpoint.");
        return false;
    }

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

bool HttpClient::httpResponseEnd()
{
    if (buf_idx < 3)
        return false;
    
    if (response_buf[buf_idx] == '\r')
        Log.info("--- Received '\\r'");

    if (response_buf[buf_idx] == '\n')
        Log.info("--- Received '\\n'");

    // A "\r\n\r\n" signifies the end of an HTTP response
    if (response_buf[buf_idx-3] == '\r' && response_buf[buf_idx-2] == '\n' &&
         response_buf[buf_idx-1] == '\r' && response_buf[buf_idx] == '\n') {
             return true;
    } else {
        return false;
    }
}