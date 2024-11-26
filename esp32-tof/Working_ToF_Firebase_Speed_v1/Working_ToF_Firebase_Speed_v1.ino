/*
 * Project: Object Motion Monitoring and Speed Calculation
 * Description:
 * This project uses a VL53L1X Time-of-Flight (ToF) sensor to detect objects and calculate their speed. 
 * The system uploads speed data to Firebase every 5 minutes and incorporates power-saving features 
 * for efficient operation.
 *
 * Libraries Used:
 * - Wire.h: For I2C communication.
 * - Adafruit_VL53L1X.h: To manage the VL53L1X sensor.
 * - WiFi.h: For Wi-Fi connectivity.
 * - FirebaseClient.h: To interact with Firebase services.
 * - sys/time.h: For time synchronization via NTP.
 *
 * Core Components:
 * 1. VL53L1X ToF Sensor:
 *    - Measures the distance of objects within the range 50 mm to 2000 mm.
 * 2. Wi-Fi and Firebase Integration:
 *    - Uploads average speed data to Firestore every 5 minutes.
 * 3. LED Indicator:
 *    - Blinks to confirm successful operations (disabled for power saving).
 * 4. Power-Saving Features:
 *    - Reduces CPU frequency and disables Bluetooth and Wi-Fi when idle.
 *
 * Key Constants:
 * - `MIN_DETECTION_DISTANCE_MM`: Minimum object detection distance (50 mm).
 * - `MAX_DETECTION_DISTANCE_MM`: Maximum object detection distance (2000 mm).
 * - `MEASURED_DISTANCE_MM`: Hypothetical distance for speed calculation (450 mm).
 * - `MAX_SPEED_KMH`: Maximum allowed speed (40 km/h).
 * - `AVERAGE_INTERVAL_MS`: Interval to calculate average speed (5 minutes).
 *
 * Main Workflow:
 * 1. Setup:
 *    - Initializes the VL53L1X sensor, I2C communication, and Firebase client.
 * 2. Loop:
 *    - Continuously measures object distance and detects motion.
 *    - Calculates speed based on elapsed time when an object is detected.
 *    - Sends average speed to Firebase at regular intervals.
 *    - Turns off Wi-Fi for power saving between uploads.
 *
 * Functions Overview:
 * - `connectWiFi()`: Connects to the Wi-Fi network.
 * - `synchronizeTime()`: Synchronizes time with NTP servers.
 * - `sendSpeedToFirestore(float avgSpeed)`: Sends average speed data to Firebase Firestore.
 * - `authHandler()`: Manages Firebase authentication.
 * - `getTimestampString()`: Generates a timestamp for Firebase entries.
 * - `printResult()` and `printError()`: Debugging tools for Firebase operations.
 * - `blinkLEDIndicator()`: Provides a visual confirmation of successful operations.
 *
 * Setup Instructions:
 * 1. Replace placeholder credentials (`WIFI_SSID`, `FIREBASE_API_KEY`, etc.) with actual values.
 * 2. Connect the VL53L1X sensor to the defined I2C pins.
 * 3. Upload this code to your microcontroller and monitor the serial output.
 *
 * Power-Saving Notes:
 * - Wi-Fi is disabled between data uploads to conserve power.
 * - CPU frequency is reduced to 80 MHz for efficiency.
 *
 * Error Handling:
 * - Errors in sensor readings or Firebase operations are logged via the serial console.
 */

#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <sys/time.h> // For time functions

// Wi-Fi and Firebase details
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASS"
#define FIREBASE_PROJECT_ID "FIREBASE_ID"
#define FIREBASE_API_KEY "FIREBASE_API_KEY"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASS"

// Firebase setup
WiFiClientSecure ssl_client;
DefaultNetwork network;
AsyncClientClass asyncClient(ssl_client, getNetwork(network));
UserAuth user_auth(FIREBASE_API_KEY, USER_EMAIL, USER_PASSWORD);
FirebaseApp app;
Firestore::Documents Docs;
AsyncResult result;

//  LED Indicator on GPIO3
const int ledPin = 3;  

// ToF sensor setup
const int I2C_SDA = 19;  // SDA pin for TTGO T-OI Plus
const int I2C_SCL = 18;  // SCL pin for TTGO T-OI Plus
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

// Constants
const int MAX_SPEED_KMH = 40;   // Maximum allowed speed in km/h
const int MIN_DETECTION_DISTANCE_MM = 50;   // Minimum detection distance: 5 cm
const int MAX_DETECTION_DISTANCE_MM = 2000;  // Maximum detection distance: 200 cm
const unsigned long AVERAGE_INTERVAL_MS = 5 * 60 * 1000; // Average interval: 5 minutes
const unsigned long LOOP_DELAY_MS = 50; // Main loop delay
const float MEASURED_DISTANCE_MM = 450.0;  // Hypothetical distance for speed calculation (in mm)

// Global variables
bool objectDetected = false;
unsigned long detectionStartTime;
unsigned long lastAverageTime = 0;
float speedSum = 0;
int speedCount = 0;
bool wifiConnected = false;

void setup() {
    Serial.begin(115200);

    // Initialize the LED pin
    pinMode(ledPin, OUTPUT);

    // Initialize I2C communication
    Wire.begin(I2C_SDA, I2C_SCL);

    // Initialize ToF sensor
    if (!vl53.begin(0x29, &Wire)) {
        Serial.print(F("Error on init of VL sensor: "));
        Serial.println(vl53.vl_status);
        while (1) delay(10);
    }
    Serial.println(F("VL53L1X sensor initialized successfully!"));

    // Start ranging and set timing budget
    if (!vl53.startRanging()) {
        Serial.print(F("Couldn't start ranging: "));
        Serial.println(vl53.vl_status);
        while (1) delay(10);
    }
    vl53.setTimingBudget(50);  // Set timing budget to 50 ms

    // Power-saving setup
    WiFi.mode(WIFI_OFF);
    btStop();  // Disable Bluetooth to save power
    setCpuFrequencyMhz(80);  // Lower CPU frequency to save power

    // Initialize Firebase (but don't authenticate yet)
    ssl_client.setInsecure();
    initializeApp(asyncClient, app, getAuth(user_auth), result);

    // Bind the FirebaseApp for authentication handler
    app.getApp<Firestore::Documents>(Docs);
    asyncClient.setAsyncResult(result);
}

void loop() {
    int16_t distance;

    // Handle authentication loop if Wi-Fi is connected
    if (wifiConnected) {
        JWT.loop(app.getAuth());
    }

    // Check if data is ready
    if (vl53.dataReady()) {
        distance = vl53.distance();

        // Handle distance measurement error
        if (distance == -1) {
            Serial.print(F("Couldn't get distance: "));
            Serial.println(vl53.vl_status);
        } else {
            // Check if distance is within the specified range (5-30 cm)
            if (distance >= MIN_DETECTION_DISTANCE_MM && distance <= MAX_DETECTION_DISTANCE_MM) {
                Serial.print("Distance (mm): ");
                Serial.println(distance);  // Print the current distance

                // Start the timer if object is detected
                if (!objectDetected) {
                    objectDetected = true;
                    detectionStartTime = millis();
                    Serial.println("Object detected, timer started.");
                }
            } else {
                // Stop the timer if object is removed
                if (objectDetected) {
                    objectDetected = false;
                    unsigned long detectionEndTime = millis();
                    float elapsedTimeSec = (detectionEndTime - detectionStartTime) / 1000.0;
                    float speedKMH = (MEASURED_DISTANCE_MM / 1000.0) / elapsedTimeSec * 3.6;

                    // Filter out-of-range speeds
                    if (speedKMH >= 0 && speedKMH <= MAX_SPEED_KMH) {
                        speedSum += speedKMH;
                        speedCount++;
                        Serial.print("Measured Speed (km/h): ");
                        Serial.println(speedKMH);  // Print the measured speed
                    } else {
                        Serial.println("Measured speed is out of range.");
                    }
                }
            }
        }
        vl53.clearInterrupt();  // Clear the interrupt after reading
    }

    // Every 5 minutes, send average speed to Firestore
    if (millis() - lastAverageTime >= AVERAGE_INTERVAL_MS) {
        if (speedCount > 0) {
            float averageSpeed = speedSum / speedCount;

            // Connect to Wi-Fi
            if (connectWiFi()) {
                // Authenticate with Firebase
                authHandler();

                // Synchronize time via NTP
                synchronizeTime();

                // Print average speed after Wi-Fi connection
                Serial.print("Average speed over 5 minutes: ");
                Serial.println(averageSpeed);

                // Send data to Firestore
                sendSpeedToFirestore(averageSpeed);

                // Reset counters
                speedSum = 0;
                speedCount = 0;
                lastAverageTime = millis();

                // Disconnect Wi-Fi for power saving
                WiFi.disconnect(true, true);
                WiFi.mode(WIFI_OFF);
                wifiConnected = false;
                Serial.println("Wi-Fi disconnected for power saving.");
            } else {
                Serial.println("Failed to connect to Wi-Fi.");
            }
        } else {
            Serial.println("No speed data collected in the last interval.");
            lastAverageTime = millis();
        }
    }

    delay(LOOP_DELAY_MS);
}

// Function to send data to Firestore
void sendSpeedToFirestore(float avgSpeed) {
    if (app.ready()) {
        String collectionId = "ethical-count";
        String documentId = "speed_" + String(millis()); // Or omit this line to auto-generate ID

        Document<Values::Value> doc("speed", Values::Value(Values::DoubleValue(avgSpeed)));

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

        // Create the document with specified document ID
        String payload = Docs.createDocument(asyncClient, Firestore::Parent(FIREBASE_PROJECT_ID), collectionId, documentId, DocumentMask(), doc);

        if (asyncClient.lastError().code() == 0) {
            Serial.println("Average speed data sent to Firestore successfully.");
            //blinkLEDIndicator(); //disable to save power
        } else {
            Serial.print("Failed to send average speed data to Firestore. Error: ");
            Serial.println(asyncClient.lastError().message());
        }
    } else {
        Serial.println("Firebase app not ready for authentication.");
    }
}

// Connect Wi-Fi function
bool connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi connected");
        wifiConnected = true;
        return true;
    } else {
        Serial.println("\nWi-Fi connection failed");
        wifiConnected = false;
        return false;
    }
}

// Time synchronization function
void synchronizeTime() {
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
}

// Authentication handler
void authHandler() {
    // Blocking authentication handler with timeout
    unsigned long ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000) {
        JWT.loop(app.getAuth());
        printResult(result);
    }
}

// Function to generate timestamp string
String getTimestampString(uint64_t sec, uint32_t nanos)
{
    if (sec > 0x3afff4417f)
        sec = 0x3afff4417f;

    if (nanos > 0x3b9ac9ff)
        nanos = 0x3b9ac9ff;

    time_t now = sec;
    struct tm ts;
    gmtime_r(&now, &ts); // Use gmtime_r for thread safety

    char buf[40];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &ts);

    // Format fractional seconds
    char fractional[10];
    snprintf(fractional, sizeof(fractional), ".%09lu", (unsigned long)nanos);

    String timestampStr = String(buf) + String(fractional) + "Z";
    return timestampStr;
}

// Function to print authentication results
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

// Function to print errors
void printError(int code, const String &msg) {
    Serial.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void blinkLEDIndicator() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(200);  // LED on for 200ms
        digitalWrite(ledPin, LOW);
        delay(200);  // LED off for 200ms
    }
}