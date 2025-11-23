#include <Arduino.h>
#include <WiFi.h>  // For ESP32 / ESP32-S3
#include <WiFiClientSecure.h>     // for TLS
#include <PubSubClient.h>         // MQTT client

#define RGB_BUILTIN 48

// ---- WiFi config ----
const char* WIFI_SSID     = "KPND39256";
const char* WIFI_PASSWORD = "RPd4CxzvxWspVjLt";

// ---- MQTT broker ----
const char* MQTT_HOST = "morninglow.nl";  // or server IP for testing
const int   MQTT_PORT = 9883;              // 8883 = MQTT over TLS (common)
const char* MQTT_USER = "esp32user";       // set in mosquitto later
const char* MQTT_PASS = "esp32password";

// Topic to control LED
const char* LED_TOPIC = "esp32/led";

const char* MQTT_CA_CERT = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Forward declarations
void setLed(bool on);

void connectToWifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for connection
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) { // ~20 seconds
    delay(500);
    Serial.print(".");
    retries++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
     neopixelWrite(RGB_BUILTIN, 0, 50, 50);  // blauw aan
  } else {
    Serial.println("❌ Failed to connect to WiFi");
     neopixelWrite(RGB_BUILTIN, 50, 0, 0);  // red
  }
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT msg on topic ");
  Serial.println(topic);

  // Copy payload into a string
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();
  msg.toLowerCase();

  if (String(topic) == LED_TOPIC) {
    if (msg == "on" || msg == "1" || msg == "true") {
      setLed(true);
    } else {
      setLed(false);
    }
  }
}



void connectToMqtt() {
  // For real security:
  wifiClient.setCACert(MQTT_CA_CERT);

  // For quick testing only (NOT secure):
  // wifiClient.setInsecure();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("esp32-client-1", MQTT_USER, MQTT_PASS)) {
      Serial.println("connected!");
      mqttClient.subscribe(LED_TOPIC);
      Serial.print("Subscribed to ");
      Serial.println(LED_TOPIC);
    } else {
      Serial.print("failed, state=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void setLed(bool on) {
  if (on) {
    neopixelWrite(RGB_BUILTIN, 0, 0, 50); // blue-ish
  } else {
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);  // off
  }
}



void setup() {
  neopixelWrite(RGB_BUILTIN, 0, 50, 0);  // green

    Serial.begin(115200);
  delay(1000);

  connectToWifi();
  connectToMqtt();
   
}

void loop() {
    if (!mqttClient.connected()) {
    connectToMqtt();
  }
  mqttClient.loop(); // keep MQTT alive
}