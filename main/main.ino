#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi Credentials
// what did you think i was going to put there hehe 

// Define states
enum State { JUNBICHU, KITAKU, SAGYOCHU };
State currentState = JUNBICHU;

// Pixel Art Data (Placeholder - You need to define your own bitmaps)
const uint16_t junbichu[64 * 64]; // 準備中
const uint16_t kitaku[64 * 64];    // 帰宅
const uint16_t sagyochu[64 * 64];  // 作業中

void setup() {
    M5.begin();
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    
    M5.Lcd.println("\nConnected to Wi-Fi!");
    fetchState();
    updateDisplay();
}

void loop() {
    M5.update();

    // Button A - 準備中
    if (M5.BtnA.wasPressed()) {
        currentState = JUNBICHU;
        updateServer();
    }
    // Button B - 帰宅
    if (M5.BtnB.wasPressed()) {
        currentState = KITAKU;
        updateServer();
    }
    // Button C - 作業中
    if (M5.BtnC.wasPressed()) {
        currentState = SAGYOCHU;
        updateServer();
    }

    // Fetch remote state every 10 seconds
    static unsigned long lastFetch = 0;
    if (millis() - lastFetch > 10000) {
        lastFetch = millis();
        fetchState();
    }

    delay(100);
}

// Update display based on current state
void updateDisplay() {
    M5.Lcd.fillScreen(BLACK);
    
    switch (currentState) {
        case JUNBICHU:
            M5.Lcd.drawBitmap(32, 32, 64, 64, junbichu);
            break;
        case KITAKU:
            M5.Lcd.drawBitmap(32, 32, 64, 64, kitaku);
            break;
        case SAGYOCHU:
            M5.Lcd.drawBitmap(32, 32, 64, 64, sagyochu);
            break;
    }
}

// Send updated state to the web server
void updateServer() {
    HTTPClient http;
    http.begin(server_url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"state\":\"";
    if (currentState == JUNBICHU) payload += "junbichu";
    else if (currentState == KITAKU) payload += "kitaku";
    else if (currentState == SAGYOCHU) payload += "sagyochu";
    payload += "\"}";

    int httpResponseCode = http.POST(payload);
    http.end();
    
    updateDisplay();
}

// Fetch current state from web server
void fetchState() {
    HTTPClient http;
    http.begin(server_url);
    
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        String response = http.getString();
        if (response.indexOf("junbichu") != -1) currentState = JUNBICHU;
        else if (response.indexOf("kitaku") != -1) currentState = KITAKU;
        else if (response.indexOf("sagyochu") != -1) currentState = SAGYOCHU;
    }
    
    http.end();
    updateDisplay();
}
