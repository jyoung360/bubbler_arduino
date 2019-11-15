#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Particle.h"
#include "MQTT.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

#define INPUT A2
#define LED D7
int val = 0;  
volatile int whattime;
String totalBubbles = "0";
String avgBubbles = "0/min";

void callback(char* topic, byte* payload, unsigned int length);
MQTT client("xxx.xxx.xxx.xxx", 1883, callback);

void callback(char* topic, byte* payload, unsigned int length) {
	char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

	String myPayload = String(p);
	Serial.println(myPayload);
	if(strcmp(topic,"bubbles/signal") == 0) {
		digitalWrite(LED, HIGH);
		whattime = millis();
	} else if(strcmp(topic,"bubbles/count") == 0) {
		totalBubbles = myPayload;
		display.clearDisplay();
		display.setTextSize(2); // Draw 2X-scale text
		display.setTextColor(WHITE);
		display.setCursor(10, 0);
		display.print(F(totalBubbles));
		display.setCursor(10, 20);
		display.print(F(avgBubbles));
		display.display();
	} else if(strcmp(topic,"bubbles/average") == 0) {
		avgBubbles = myPayload;
		display.clearDisplay();
		display.setTextSize(2); // Draw 2X-scale text
		display.setTextColor(WHITE);
		display.setCursor(10, 0);
		display.print(F(totalBubbles));
		display.setCursor(10, 20);
		display.print(F(avgBubbles));
		display.display();
	}

	Serial.println(topic);
	Serial.println(p);
}
// QOS ack callback.
// if application use QOS1 or QOS2, MQTT server sendback ack message id.
void qoscallback(unsigned int messageid) {
    Serial.print("Ack Message Id:");
    Serial.println(messageid);
}

boolean reconnect() {
	client.connect("sparkclient", "xxxxxxxx", "xxxxxxxx");
	client.subscribe("bubbles/signal", MQTT::QOS0);
	client.subscribe("bubbles/count", MQTT::QOS0);
	client.subscribe("bubbles/average", MQTT::QOS0);
}

long lastReconnectAttempt = 0;

void setup() {
	WiFi.on();
	WiFi.connect();
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
	Serial.begin(9600);
	pinMode(LED, OUTPUT);
    client.connect("sparkclient", "xxxxxxxx", "xxxxxxxx");
	client.subscribe("bubbles/signal", MQTT::QOS0);
	client.subscribe("bubbles/count", MQTT::QOS0);
	client.subscribe("bubbles/average", MQTT::QOS0);
}

void loop() {
	val = analogRead(INPUT);
	String value = String(val);
	if (client.isConnected()) {
        uint16_t messageid;
        client.publish("bubbles/voltage", value, MQTT::QOS0, &messageid);
		client.loop();
    }
	else {
		long now = millis();
		if (now - lastReconnectAttempt > 5000) {
			lastReconnectAttempt = now;
			if (reconnect()) {
				lastReconnectAttempt = 0;
			}
		}
	}

	int now = millis();
	if(now - whattime > 500) {
		digitalWrite(LED, LOW);
	}
}
