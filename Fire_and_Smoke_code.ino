/*
 * ============================================================
 *  Fire & Gas Safety System — WebServer Version
 *  Original: Blynk  →  Converted to: Local WebServer (no Firebase, no Blynk)
 *
 *  SAME PINS AS YOUR ORIGINAL CODE:
 *  ─────────────────────────────────────────────────────────
 *  MQ2 Sensor:
 *    AO  → GPIO 36   (Analog)
 *    DO  → GPIO 34   (Digital)
 *
 *  Flame Sensor:
 *    AO  → GPIO 39   (Analog)
 *    DO  → GPIO 35   (Digital)
 *
 *  Buzzer      → GPIO 25
 *  LED         → GPIO 26
 *  Servo Motor → GPIO 13
 *
 *  HOW TO USE:
 *  1. Upload this code to ESP32
 *  2. Open Serial Monitor at 115200 baud
 *  3. Wait for "Connected! Open: http://192.168.x.x"
 *  4. Open that IP in any browser on the same WiFi
 *  5. Dashboard loads automatically!
 *
 *  LIBRARIES NEEDED (Arduino Library Manager):
 *    • ESP32Servo  by Kevin Harrington
 *    (WiFi and WebServer are built-in with ESP32 board)
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ─────────────────────────────────────────────────────────
//  YOUR WIFI (same as original)
// ─────────────────────────────────────────────────────────
char ssid[] = "Oppo";
char pass[] = "55555555";

// ─────────────────────────────────────────────────────────
//  PINS (exact same as your original .ino)
// ─────────────────────────────────────────────────────────
#define MQ2_A      36    // MQ2 Analog Out
#define MQ2_D      34    // MQ2 Digital Out
#define FLAME_A    39    // Flame Sensor Analog Out
#define FLAME_D    35    // Flame Sensor Digital Out
#define BUZZER     25    // Active Buzzer
#define LED_PIN    26    // Alert LED
#define SERVO_PIN  13    // Servo Motor Signal

// ─────────────────────────────────────────────────────────
//  THRESHOLDS (same as your original)
// ─────────────────────────────────────────────────────────
int smokeLimit = 500;    // MQ2 analog threshold
int flameLimit = 500;    // Flame sensor analog threshold

// ─────────────────────────────────────────────────────────
//  WARMUP (same as original — 3 minutes)
// ─────────────────────────────────────────────────────────
unsigned long warmupTime = 180000;   // 3 minutes in ms
unsigned long startTime;

// ─────────────────────────────────────────────────────────
//  ALERT TIMING (same as original — 3 seconds)
// ─────────────────────────────────────────────────────────
bool alertActive = false;
unsigned long alertStartTime = 0;
const int alertDuration = 3000;

// ─────────────────────────────────────────────────────────
//  GLOBAL SENSOR STATE (read by web server)
// ─────────────────────────────────────────────────────────
int    smokeAnalog  = 0;
int    smokeDigital = 0;
int    flameAnalog  = 0;
int    flameDigital = 0;
String sysStatus    = "SAFE";
String alertType    = "NONE";

// ─────────────────────────────────────────────────────────
WebServer server(80);
Servo     doorServo;

// ─────────────────────────────────────────────────────────
//  TRIGGER ALERT (same logic as original)
// ─────────────────────────────────────────────────────────
void triggerAlert(String type) {
  if (alertActive) return;

  alertActive    = true;
  alertStartTime = millis();
  alertType      = type;

  digitalWrite(BUZZER,  HIGH);
  digitalWrite(LED_PIN, HIGH);
  doorServo.write(90);   // Open door/vent on alert

  Serial.print("[ALERT] Triggered: ");
  Serial.println(type);
}

// ─────────────────────────────────────────────────────────
//  RESET ALERT after alertDuration (same as original)
// ─────────────────────────────────────────────────────────
void resetAlert() {
  if (alertActive && millis() - alertStartTime >= alertDuration) {
    digitalWrite(BUZZER,  LOW);
    digitalWrite(LED_PIN, LOW);
    doorServo.write(0);   // Close door/vent
    alertActive = false;
    alertType   = "NONE";
    Serial.println("[ALERT] Reset — returning to SAFE");
  }
}

// ─────────────────────────────────────────────────────────
//  CHECK SENSORS (same logic as original)
// ─────────────────────────────────────────────────────────
void checkSensors() {
  // Skip during warmup
  if (millis() - startTime < warmupTime) {
    Serial.println("[WARMUP] MQ2 Sensor Warming Up...");
    return;
  }

  // Read all 4 sensor pins
  smokeAnalog  = analogRead(MQ2_A);
  smokeDigital = digitalRead(MQ2_D);
  flameAnalog  = analogRead(FLAME_A);
  flameDigital = digitalRead(FLAME_D);

  // Determine status
  bool gasAlert   = (smokeAnalog > smokeLimit);
  bool flameAlert = (flameAnalog < flameLimit || flameDigital == LOW);

  if (gasAlert && flameAlert) {
    sysStatus = "CRITICAL";
    triggerAlert("GAS+FIRE");
  } else if (flameAlert) {
    sysStatus = "CRITICAL";
    triggerAlert("FIRE");
  } else if (gasAlert) {
    sysStatus = "WARNING";
    triggerAlert("GAS");
  } else {
    sysStatus = "SAFE";
  }

  // Serial print (same as original)
  Serial.print("Smoke AO: ");   Serial.print(smokeAnalog);
  Serial.print(" | Smoke DO: "); Serial.print(smokeDigital);
  Serial.print(" || Flame AO: "); Serial.print(flameAnalog);
  Serial.print(" | Flame DO: "); Serial.print(flameDigital);
  Serial.print(" | Status: ");   Serial.println(sysStatus);
}

// ─────────────────────────────────────────────────────────
//  /data  →  Returns JSON to dashboard
// ─────────────────────────────────────────────────────────
void handleData() {
  String json = "{";
  json += "\"smokeAnalog\":"  + String(smokeAnalog)   + ",";
  json += "\"smokeDigital\":" + String(smokeDigital)  + ",";
  json += "\"flameAnalog\":"  + String(flameAnalog)   + ",";
  json += "\"flameDigital\":" + String(flameDigital)  + ",";
  json += "\"status\":\""     + sysStatus             + "\",";
  json += "\"alertType\":\""  + alertType             + "\",";
  json += "\"alertActive\":"  + String(alertActive ? "true" : "false") + ",";
  json += "\"smokeLimit\":"   + String(smokeLimit)    + ",";
  json += "\"flameLimit\":"   + String(flameLimit)    + ",";
  json += "\"uptime\":"       + String(millis()/1000) + ",";
  json += "\"warmingUp\":"    + String((millis()-startTime < warmupTime) ? "true" : "false") + ",";
  json += "\"ip\":\""         + WiFi.localIP().toString() + "\"";
  json += "}";

  // Allow browser to fetch (CORS)
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

// ─────────────────────────────────────────────────────────
//  /  →  Redirect to dashboard
//  (dashboard is opened as a separate .html file)
// ─────────────────────────────────────────────────────────
void handleRoot() {
  String html = "<html><body style='background:#04080e;color:#ff6a00;font-family:monospace;text-align:center;padding:40px;'>";
  html += "<h2>PYROS — Fire & Gas Monitor</h2>";
  html += "<p>ESP32 is running!</p>";
  html += "<p>Open the <b>pyros_dashboard.html</b> file in your browser.</p>";
  html += "<p>Make sure <b>ESP32_IP</b> in the dashboard is set to: <b>" + WiFi.localIP().toString() + "</b></p>";
  html += "<hr/><p>API endpoint: <a href='/data' style='color:#ffaa00'>/data</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// ─────────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Fire & Gas Safety System ===");

  startTime = millis();

  // Pin modes (same as original)
  pinMode(MQ2_D,   INPUT);
  pinMode(FLAME_D, INPUT);
  pinMode(BUZZER,  OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER,  LOW);
  digitalWrite(LED_PIN, LOW);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0);   // Start closed

  // Connect WiFi
  Serial.printf("[WiFi] Connecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[WiFi] Connected!");
  Serial.print("[WiFi] ESP32 IP Address: http://");
  Serial.println(WiFi.localIP());
  Serial.println("[WiFi] Open the above IP in your browser!");
  Serial.println("─────────────────────────────────────");
  Serial.println("[INFO] Open pyros_dashboard.html in browser");
  Serial.print("[INFO] Set ESP32_IP = \"");
  Serial.print(WiFi.localIP());
  Serial.println("\" in the dashboard file");
  Serial.println("─────────────────────────────────────");

  // Register web server routes
  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("[Server] HTTP server started on port 80");
  Serial.println("[INFO] Waiting 3 minutes for MQ2 warm-up...");
}

// ─────────────────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────────────────
void loop() {
  server.handleClient();   // Handle dashboard requests

  // Check sensors every 2 seconds (same interval as original timer)
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck >= 2000) {
    lastCheck = millis();
    checkSensors();
  }

  resetAlert();   // Same as original
}
