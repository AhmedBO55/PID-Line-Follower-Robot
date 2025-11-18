#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Ooredoo-ALHN-05DD";
const char* password = "tEnLxF2D6r";

WebServer server(80);

const int S1 = 33, S2 = 32, S3 = 35, S4 = 34, S5 = 39;
const int PWMA = 18, AIN1 = 16, AIN2 = 17;
const int PWMB = 19, BIN1 = 25, BIN2 = 23, STBY = 5;

const int freqPWM = 1000, resolutionPWM = 8;

int baseSpeed = 180;
int maxSpeed = 240;
int searchSpeed = 60;
float Kp = 18.0, Ki = 0.05, Kd = 22.0;

float lastError = 0, integral = 0;
const float integralLimit = 30.0;
unsigned long lastTime = 0;

const int positions[5] = {-2, -1, 0, 1, 2};
int lastDirection = 0;

volatile bool robotStopped = false;
volatile bool resetRequested = false;

int stuckCounter = 0;
const int stuckThreshold = 8;

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Line Control</title>
<style>
body{font-family:Arial;background:#667eea;padding:20px;margin:0}
.container{max-width:500px;margin:0 auto;background:#fff;border-radius:10px;padding:25px}
h1{color:#667eea;text-align:center;margin-bottom:20px}
.section{background:#f5f5f5;padding:15px;border-radius:8px;margin-bottom:15px}
.section h2{font-size:16px;margin:0 0 10px 0;color:#333}
.group{margin:10px 0}
label{display:block;font-weight:bold;margin-bottom:3px;font-size:14px}
input{width:100%;padding:8px;border:1px solid #ddd;border-radius:4px;font-size:14px}
.btn{width:100%;padding:12px;border:none;border-radius:6px;font-size:16px;font-weight:bold;cursor:pointer;margin-top:8px}
.btn-primary{background:#667eea;color:#fff}
.btn-danger{background:#e74c3c;color:#fff}
</style>
</head>
<body>
<div class="container">
<h1>Line Follower</h1>
<div class="section">
<h2>Speed</h2>
<div class="group">
<label>Base:</label>
<input type="number" id="baseSpeed" value="85" min="0" max="255">
</div>
<div class="group">
<label>Max:</label>
<input type="number" id="maxSpeed" value="110" min="0" max="255">
</div>
<div class="group">
<label>Search:</label>
<input type="number" id="searchSpeed" value="60" min="0" max="255">
</div>
</div>
<div class="section">
<h2>PID</h2>
<div class="group">
<label>Kp:</label>
<input type="number" id="kp" value="18.0" step="0.1">
</div>
<div class="group">
<label>Ki:</label>
<input type="number" id="ki" value="0.05" step="0.01">
</div>
<div class="group">
<label>Kd:</label>
<input type="number" id="kd" value="22.0" step="0.1">
</div>
</div>
<button class="btn btn-primary" onclick="apply()">Apply</button>
<button class="btn btn-danger" onclick="reset()">Reset</button>
</div>
<script>
function apply(){
const p=new URLSearchParams({
baseSpeed:document.getElementById('baseSpeed').value,
maxSpeed:document.getElementById('maxSpeed').value,
searchSpeed:document.getElementById('searchSpeed').value,
kp:document.getElementById('kp').value,
ki:document.getElementById('ki').value,
kd:document.getElementById('kd').value
});
fetch('/update?'+p.toString()).then(r=>r.text()).then(()=>alert('Applied'));
}
function reset(){
if(confirm('Reset?')){
fetch('/reset').then(()=>{alert('Resetting...');setTimeout(()=>location.reload(),2000);});
}
}
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\nStarting...");
  
  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT);
  pinMode(S4, INPUT); pinMode(S5, INPUT);
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(STBY, OUTPUT);
  
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  digitalWrite(STBY, LOW);
  
  ledcAttach(PWMA, freqPWM, resolutionPWM);
  ledcAttach(PWMB, freqPWM, resolutionPWM);
  ledcWrite(PWMA, 0); ledcWrite(PWMB, 0);
  
  digitalWrite(STBY, HIGH);
  
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi failed!");
  }
  
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", webpage);
  });
  
  server.on("/update", HTTP_GET, handleUpdate);
  server.on("/reset", HTTP_GET, handleReset);
  
  server.begin();
  Serial.println("Server started");
  
  delay(2000);
}

void loop() {
  server.handleClient();
  
  if (resetRequested) {
    ESP.restart();
  }
  
  if (!robotStopped) {
    followLine();
  }
  
  delay(10);
}

void followLine() {
  int s1 = !digitalRead(S1), s2 = !digitalRead(S2), s3 = !digitalRead(S3);
  int s4 = !digitalRead(S4), s5 = !digitalRead(S5);
  
  if (s1 && s2 && s3 && s4 && s5) {
    stopRobot();
    return;
  }
  
  if (!s1 && !s2 && !s3 && !s4 && !s5) {
    searchForLine();
    stuckCounter++;
    if (stuckCounter > stuckThreshold) {
      setMotorSpeed(baseSpeed, baseSpeed);
      stuckCounter = 0;
    }
    return;
  }
  
  stuckCounter = 0;
  
  float error = calculateError(s1, s2, s3, s4, s5);
  
  if (error < -0.5) lastDirection = -1;
  else if (error > 0.5) lastDirection = 1;
  
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  
  if (lastTime == 0 || dt > 1.0) {
    lastTime = currentTime;
    dt = 0.01;
  }
  
  if (dt > 0) {
    float P = Kp * error;
    
    integral += error * dt;
    integral = constrain(integral, -integralLimit, integralLimit);
    float I = Ki * integral;
    
    float derivative = (error - lastError) / dt;
    float D = Kd * derivative;
    
    float correction = P + I + D;
    
    if (s3 == 1 && abs(error) < 0.5) {
      correction *= 0.2;
      integral *= 0.5;
    }
    
    int leftSpeed = constrain(baseSpeed + correction, -maxSpeed, maxSpeed);
    int rightSpeed = constrain(baseSpeed - correction, -maxSpeed, maxSpeed);
    
    setMotorSpeed(leftSpeed, rightSpeed);
    
    lastError = error;
    lastTime = currentTime;
  }
}

void searchForLine() {
  int leftSpeed = (lastDirection <= 0) ? -searchSpeed : searchSpeed;
  int rightSpeed = -leftSpeed;
  setMotorSpeed(leftSpeed, rightSpeed);
}

void stopRobot() {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  ledcWrite(PWMA, 0); ledcWrite(PWMB, 0);
  robotStopped = true;
}

float calculateError(int s1, int s2, int s3, int s4, int s5) {
  int sensors[5] = {s1, s2, s3, s4, s5};
  float weightedSum = 0;
  int sensorCount = 0;
  
  for (int i = 0; i < 5; i++) {
    if (sensors[i] == 1) {
      weightedSum += positions[i];
      sensorCount++;
    }
  }
  
  return (sensorCount == 0) ? lastError : weightedSum / sensorCount;
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  if (leftSpeed > 0) {
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
  }
  ledcWrite(PWMA, abs(leftSpeed));
  
  if (rightSpeed > 0) {
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  } else if (rightSpeed < 0) {
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  }
  ledcWrite(PWMB, abs(rightSpeed));
}

void handleUpdate() {
  if (server.hasArg("baseSpeed")) baseSpeed = server.arg("baseSpeed").toInt();
  if (server.hasArg("maxSpeed")) maxSpeed = server.arg("maxSpeed").toInt();
  if (server.hasArg("searchSpeed")) searchSpeed = server.arg("searchSpeed").toInt();
  if (server.hasArg("kp")) Kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) Ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) Kd = server.arg("kd").toFloat();
  
  server.send(200, "text/plain", "OK");
}

void handleReset() {
  server.send(200, "text/plain", "Resetting");
  delay(100);
  resetRequested = true;
}