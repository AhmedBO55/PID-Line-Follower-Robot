#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it
#endif

BluetoothSerial SerialBT;

const int PWMA = 18;
const int AIN1 = 16;
const int AIN2 = 17;

const int PWMB = 19;
const int BIN1 = 25;
const int BIN2 = 23;
const int STBY = 5;

const int freqPWM = 1000;
const int resolutionPWM = 8;

int currentSpeed = 200; 
char currentCommand = 'S'; 

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Car Ready ===");
  
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  digitalWrite(STBY, LOW);
  
  delay(100);
  
  ledcAttach(PWMA, freqPWM, resolutionPWM);
  ledcAttach(PWMB, freqPWM, resolutionPWM);
  
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  
  digitalWrite(STBY, HIGH);
  
  SerialBT.begin("ESP32_Car");
  Serial.println("BT: ESP32_Car");
  Serial.println("Waiting for connection...\n");
}

void loop() {
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    handleCommand(cmd);
  }
  
  if (Serial.available()) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }
}

void handleCommand(char cmd) {
  cmd = toupper(cmd);
  
  if (cmd >= '0' && cmd <= '9') {
    int level = cmd - '0';
    currentSpeed = map(level, 0, 9, 0, 255);
    Serial.print("Speed: ");
    Serial.print(level);
    Serial.print(" (");
    Serial.print(currentSpeed);
    Serial.println("/255)");
    SerialBT.print("Speed: ");
    SerialBT.println(level);
    return;
  }
  
  currentCommand = cmd;
  
  switch(cmd) {
    case 'F':
      moveForward();
      Serial.println(">> Forward");
      SerialBT.println("Forward");
      break;
      
    case 'B':
      moveBackward();
      Serial.println(">> Back");
      SerialBT.println("Back");
      break;
      
    case 'L':
      turnLeft();
      Serial.println(">> Left");
      SerialBT.println("Left");
      break;
      
    case 'R':
      turnRight();
      Serial.println(">> Right");
      SerialBT.println("Right");
      break;
      
    case 'G':
      forwardLeft();
      Serial.println(">> Fwd-Left");
      SerialBT.println("Fwd-Left");
      break;
      
    case 'I':
      forwardRight();
      Serial.println(">> Fwd-Right");
      SerialBT.println("Fwd-Right");
      break;
      
    case 'H':
      backwardLeft();
      Serial.println(">> Back-Left");
      SerialBT.println("Back-Left");
      break;
      
    case 'J':
      backwardRight();
      Serial.println(">> Back-Right");
      SerialBT.println("Back-Right");
      break;
      
    case 'S':
      stopMotors();
      Serial.println(">> Stop");
      SerialBT.println("Stop");
      break;
      
    default:
      Serial.print("Unknown: ");
      Serial.println(cmd);
      break;
  }
}

void moveForward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(PWMB, currentSpeed);
}

void moveBackward() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMB, currentSpeed);
}

void turnLeft() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(PWMB, currentSpeed);
}

void turnRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMB, currentSpeed);
}

void forwardLeft() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, currentSpeed / 2);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(PWMB, currentSpeed);
}

void forwardRight() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  ledcWrite(PWMB, currentSpeed / 2);
}

void backwardLeft() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, currentSpeed / 2);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMB, currentSpeed);
}

void backwardRight() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  ledcWrite(PWMA, currentSpeed);
  
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMB, currentSpeed / 2);
}

void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
}