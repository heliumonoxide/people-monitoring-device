#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include <WiFi.h>  // Include WiFi for ESP32
//#include <Firebase_ESP_Client.h>  // Uncomment when ready for Firebase

// Constants
const int MAX_SPEED_KMH = 40;   // Maximum allowed speed in km/h
const int MIN_DETECTION_DISTANCE_MM = 50;   // Minimum detection distance: 5 cm
const int MAX_DETECTION_DISTANCE_MM = 300;  // Maximum detection distance: 30 cm
const unsigned long AVERAGE_INTERVAL_MS = 5 * 60 * 1000; // Average interval: 5 minutes
const unsigned long LOOP_DELAY_MS = 50; // Main loop delay
const int I2C_SDA = 19;  // SDA pin for TTGO T-OI Plus
const int I2C_SCL = 18;  // SCL pin for TTGO T-OI Plus
const float MEASURED_DISTANCE_MM = 450.0;  // Hypothetical distance for speed calculation (in mm)


// Firebase setup (add project details here)
//FirebaseData fbdo;
//FirebaseConfig config;
//FirebaseAuth auth;

// Global variables
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();
bool objectDetected = false;
unsigned long detectionStartTime;
unsigned long lastAverageTime = 0;
float speedSum = 0;
int speedCount = 0;

void setup() {
  // Serial for debugging
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize ToF sensor
  if (!vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

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
}

void loop() {
  int16_t distance;

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

        // Start the timer if object (hand) is detected
        if (!objectDetected) {
          objectDetected = true;
          detectionStartTime = millis();
          Serial.println("Object detected, timer started.");
        }
      } else {
        // Stop the timer if object (hand) is removed
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

  // Every 5 minutes, send average speed to Firebase
  if (millis() - lastAverageTime >= AVERAGE_INTERVAL_MS) {
    if (speedCount > 0) {
      float averageSpeed = speedSum / speedCount;

      // Connect to WiFi
      if (connectWiFi()) {
        // Print average speed after WiFi connection
        Serial.print("Average speed over 5 minutes: ");
        Serial.println(averageSpeed);

        // Send data to Firebase
        sendSpeedToFirebase(averageSpeed);

        // Reset counters
        speedSum = 0;
        speedCount = 0;
        lastAverageTime = millis();
        
        // Disconnect WiFi for power saving
        WiFi.mode(WIFI_OFF);
      } else {
        Serial.println("Failed to connect to WiFi.");
      }
    }
  }

  delay(LOOP_DELAY_MS);
}

// Function to send data to Firebase
void sendSpeedToFirebase(float avgSpeed) {
  // Uncomment to use Firebase
  // if (Firebase.ready()) {
  //   Firebase.RTDB.setFloat(&fbdo, "/speedData/averageSpeed", avgSpeed);
  //   Serial.println("Data sent to Firebase.");
  // } else {
  //   Serial.println("Failed to connect to Firebase.");
  // }
}

// Connect WiFi function
bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("Pustaka Juanda", "juandapst");

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    return true;
  } else {
    Serial.println("\nWiFi connection failed");
    return false;
  }
}
