const int S1 = 33;
const int S2 = 32;
const int S3 = 35;
const int S4 = 34;
const int S5 = 39;
const int NEAR = 4;
const int CLP = 36;

void setup() {
  Serial.begin(115200);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(NEAR, INPUT);
  pinMode(CLP, INPUT);
  
  delay(1000);
  Serial.println("\n=== BFD-1000 Sensor Graph ===\n");
}

void printBar(const char* label, int value) {
  Serial.print(label);
  Serial.print(": ");
  
  if (value == 1) {
    Serial.print("███████████████████ "); // BLACK (HIGH)
  } else {
    Serial.print("░░░░░░░░░░░░░░░░░░░ "); // WHITE (LOW)
  }
  
  Serial.println(value);
}

void loop() {
  Serial.print("\033[2J\033[H");
  
  Serial.println("=== BFD-1000 Sensor Status ===");
  Serial.println();
  
  // Read and display each sensor
  printBar("S1  ", digitalRead(S1));
  printBar("S2  ", digitalRead(S2));
  printBar("S3  ", digitalRead(S3));
  printBar("S4  ", digitalRead(S4));
  printBar("S5  ", digitalRead(S5));
  printBar("NEAR", digitalRead(NEAR));
  printBar("CLP ", digitalRead(CLP));
  
  Serial.println();
  Serial.println("Legend: ███ = HIGH (Black) | ░░░ = LOW (White)");
  Serial.println("--------------------------------");
  
  delay(1000);
}