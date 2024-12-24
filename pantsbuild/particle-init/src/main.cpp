#include "Particle.h"
#include "HttpClient.h"
#define ANALOG_PIN 	A1 
#define MBP_API_HOST "10.0.0.77"
// #define API_HOST "10.0.0.63"
#define API_PORT 5000
#define DELAY_SECONDS 1000


HttpClient http;
http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};
http_request_t request;
http_response_t response;

SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

int val = 0;

char buf[1024];

void setup() {
    Serial.begin(9600);
}

char* create_payload() {
    memset(buf, 0, sizeof(buf));
    val = analogRead(ANALOG_PIN);
    JSONBufferWriter writer(buf, sizeof(buf));
    writer.beginObject();
    writer.name("value").value(val);
    writer.endObject();
    return writer.buffer();
}


void loop() {
    request.hostname = MBP_API_HOST;
    request.port = API_PORT;
    request.body = create_payload();

    http.post(request, response, headers);
    delay( DELAY_SECONDS );
}