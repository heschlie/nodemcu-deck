#include <main.h>

FASTLED_USING_NAMESPACE

void setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.begin(SSID, WIFI_PASSWD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
}

void setup_ota() {
    // Port defaults to 8266
    ArduinoOTA.setPort(otaPort);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(HOSTNAME);

    // No authentication by default
    ArduinoOTA.setPassword(OTA_PASSWD);

    ArduinoOTA.onStart([]() {
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating.");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("Ready to rock");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    Serial.println("Booting");

    setup_wifi();
    setup_ota();

    // FastLED stuff
    delay(3000); // 3 second delay for recovery

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    animations = createMap();

    // MQTT setup
    client.setServer(MQTT_SERVER, 1883);
    client.subscribe(SUB_TOPIC);
    client.setCallback(mqtt_callback);
}

void mqtt_reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("outTopic", "hello world");
            // ... and resubscribe
            client.subscribe(SUB_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(payload);
    Serial.println("Received:");
    Serial.println(*payload);

    const char* state = root["state"];
    currentState = state;
    if (state == "OFF") {
        currentAnimation = animations["Off"];
        return;
    }

    if (root.containsKey("color")) {
        uint8_t r = root["color"]["r"];
        uint8_t g = root["color"]["g"];
        uint8_t b = root["color"]["b"];
        current_color = CRGB(r, g, b);
        currentAnimation = animations["Solid"];
    } else if (root.containsKey("effect")) {
        const char* effect = root["effect"];
        currentAnimation = animations[std::string(effect)];
    }
}

void loop() {
    ArduinoOTA.handle();

    if (!client.connected()) {
        mqtt_reconnect();
    }
    client.loop();

    currentAnimation();
//    rainbow();
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND);

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { baseHue++; } // slowly cycle the "base color" through the rainbow
    std::string payload = "{ \"state\": \"" + currentState + "\"}\"";
    EVERY_N_SECONDS(5) {client.publish(STATE_TOPIC, payload.c_str());}
}