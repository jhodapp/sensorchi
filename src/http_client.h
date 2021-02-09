#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "JsonParserGeneratorRK.h"

#include "Particle.h"

#define JSON_WRITER_BUFFER_SIZE 256
#define RESPONSE_BUFFER_SIZE 500

// FIXME: consider inheriting from TCPClient instead of containing one
class HttpClient
{
    public:
        HttpClient(IPAddress ip, uint16_t port);
        HttpClient(const String &host, uint16_t port);
        ~HttpClient();

        bool connect();
        bool connected();
        bool available();
        const char read();

        // Sets the API endpoint to POST payload to
        void setEnpoint(const String &endpoint);

        // POSTs a JSON payload to endpoint
        bool sendJson(const JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> &json);

    protected:
        HttpClient();

        uint16_t payloadLength(const JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> &json);
        bool httpResponseEnd();

    private:
        TCPClient client;
        IPAddress ip;
        String host;
        uint16_t port;
        bool use_host;
        String endpoint;

        //char *response_buf;
        char response_buf[RESPONSE_BUFFER_SIZE];
        uint16_t buf_idx;
        bool received_full_response;
};

#endif