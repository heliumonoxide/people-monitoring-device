#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <sys/time.h> // Include this header

// Wi-Fi and Firebase details
#define WIFI_SSID "Pustaka Juanda"
#define WIFI_PASSWORD "juandapst"
#define FIREBASE_PROJECT_ID "despro-project-monitoring"
#define FIREBASE_API_KEY "AIzaSyB0wy1cc1fyewLy0ABbzadSo--lULY9vnw"
#define USER_EMAIL "thesnyo@gmail.com"
#define USER_PASSWORD "despro1234"

// Firebase setup
WiFiClientSecure ssl_client;
DefaultNetwork network;
AsyncClientClass asyncClient(ssl_client, getNetwork(network));
UserAuth user_auth(FIREBASE_API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
Firestore::Documents Docs;
AsyncResult result;

// ToF sensor setup
const int I2C_SDA = 19;  // SDA pin for TTGO T-OI Plus
const int I2C_SCL = 18;  // SCL pin for TTGO T-OI Plus
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

void setup() {
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println("\nConnected to WiFi.");

    // Initialize time via NTP
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Waiting for time synchronization");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        Serial.print(".");
        delay(500);
        now = time(nullptr);
    }
    Serial.println("\nTime synchronized");
    Serial.print("Current time: ");
    Serial.println(ctime(&now));

    ssl_client.setInsecure();

    // Initialize Firebase with user authentication
    initializeApp(asyncClient, app, getAuth(user_auth), result);

    // Call the authHandler to handle authentication
    authHandler();

    // Bind the FirebaseApp for authentication handler
    app.getApp<Firestore::Documents>(Docs);
    asyncClient.setAsyncResult(result);

    // Initialize I2C and ToF sensor
    Wire.begin(I2C_SDA, I2C_SCL);
    if (!vl53.begin(0x29, &Wire)) {
        Serial.print("Error initializing VL53L1X sensor. Status: ");
        Serial.println(vl53.vl_status);
        while (1) delay(10);
    }
    Serial.println("VL53L1X sensor initialized successfully!");

    // Start ranging with a 50 ms timing budget
    if (!vl53.startRanging()) {
        Serial.print("Couldn't start ranging. Status: ");
        Serial.println(vl53.vl_status);
        while (1) delay(10);
    }
    vl53.setTimingBudget(50);
}

void loop() {
    // Handle authentication loop
    JWT.loop(app.getAuth());

    // Check if data is ready and read distance
    if (vl53.dataReady()) {
        int16_t distance = vl53.distance();

        if (distance != -1) { // Valid measurement
            Serial.print("Measured Distance (mm): ");
            Serial.println(distance);

            // Send measured distance to Firestore
            sendDistanceToFirestore(distance);
        } else {
            Serial.print("Error reading distance. Status: ");
            Serial.println(vl53.vl_status);
        }

        vl53.clearInterrupt(); // Clear the interrupt after reading
    }

    delay(30000); // Wait 1 second before the next reading
}

void sendDistanceToFirestore(int16_t distance) {
    if (app.ready()) {
        String collectionId = "ethical-count";
        String documentId = "distance_" + String(millis());

        Document<Values::Value> doc("distance", Values::Value(Values::IntegerValue(distance)));

        // Get current time
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t seconds = tv.tv_sec;
        uint32_t nanos = tv.tv_usec * 1000; // Convert microseconds to nanoseconds

        // Create timestamp string
        String timestampStr = getTimestampString(seconds, nanos);

        // Create timestamp value
        Values::TimestampValue tsV(timestampStr);
        doc.add("timeAdded", Values::Value(tsV));

        String payload = Docs.createDocument(asyncClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, documentId, DocumentMask(), doc);
        if (asyncClient.lastError().code() == 0) {
            Serial.println("Distance data sent to Firestore successfully.");
        } else {
            Serial.print("Failed to send distance data to Firestore. Error: ");
            Serial.println(asyncClient.lastError().message());
        }
    } else {
        Serial.println("Firebase app not ready for authentication.");
    }
}

String getTimestampString(uint64_t sec, uint32_t nano)
{
    if (sec > 0x3afff4417f)
        sec = 0x3afff4417f;

    if (nano > 0x3b9ac9ff)
        nano = 0x3b9ac9ff;

    time_t now = sec;
    struct tm ts;
    gmtime_r(&now, &ts); // Use gmtime_r for thread safety

    char buf[40];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &ts);

    // Format fractional seconds
    char fractional[10];
    snprintf(fractional, sizeof(fractional), ".%09lu", (unsigned long)nano);

    String timestampStr = String(buf) + String(fractional) + "Z";
    return timestampStr;
}

void authHandler() {
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000) {
        JWT.loop(app.getAuth());
        printResult(result);
    }
}

void printResult(AsyncResult &aResult) {
    if (aResult.isEvent()) {
        Serial.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug()) {
        Serial.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError()) {
        Serial.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available()) {
        Serial.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
    }
}

void printError(int code, const String &msg) {
    Serial.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}