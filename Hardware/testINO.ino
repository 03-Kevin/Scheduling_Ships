const int ledPinsRight[] = {13, 12, 11, 10, 9}; // LEDs for the right ocean
const int ledPinsLeft[] = {6, 5, 4, 3, 2}; // LEDs for the left ocean
const int crossingLedPin = 8; // LED to indicate a boat is crossing
const int specialLedPin = 7;  // LED that blinks rapidly, slowly or stays constant

int boatQueueRight[5] = {0, 0, 0, 0, 0}; // State of the right ocean queue
int boatQueueLeft[5] = {0, 0, 0, 0, 0}; // State of the left ocean queue
int queueSizeRight = 0; // Current size of the right ocean queue
int queueSizeLeft = 0; // Current size of the left ocean queue
bool isCrossing = false; // State to indicate if a boat is crossing

unsigned long previousMillis = 0; // For blinking the crossing LED
const long interval = 200; // Blinking interval in milliseconds

unsigned long specialLedPreviousMillis = 0; // Timer for special LED
int specialLedState = LOW; // State of the special LED
int specialLedMode = 0; // 0: constant, 1: slow blink, 2: fast blink
unsigned long specialLedInterval = 0; // Interval for special LED blinking

void setup() {
  Serial.begin(9600);

  // Setup pins for right ocean LEDs
  for (int i = 0; i < 5; i++) {
    pinMode(ledPinsRight[i], OUTPUT);
    digitalWrite(ledPinsRight[i], LOW); // Turn off all right ocean LEDs initially
  }
  
  // Setup pins for left ocean LEDs
  for (int i = 0; i < 5; i++) {
    pinMode(ledPinsLeft[i], OUTPUT);
    digitalWrite(ledPinsLeft[i], LOW); // Turn off all left ocean LEDs initially
  }
  
  // Setup pin for crossing LED
  pinMode(crossingLedPin, OUTPUT);
  digitalWrite(crossingLedPin, LOW); // Ensure it's off initially

  // Setup pin for special LED (pin 7)
  pinMode(specialLedPin, OUTPUT);
  digitalWrite(specialLedPin, LOW); // Ensure it's off initially
}

void loop() {
  // Blink the crossing LED if a boat is crossing
  if (isCrossing) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      int state = digitalRead(crossingLedPin);
      digitalWrite(crossingLedPin, !state); // Toggle the crossing LED
    }
  }

  // Handle inputs for manual operations or initial data
  if (Serial.available() > 0) {
    char input = Serial.read(); // Read the received character
    Serial.print("Carácter recibido: ");
    Serial.println(input);
    // If input starts with 'L' or 'R', it's initial data
    if (input == 'L') {
      int barcosIzquierda = Serial.parseInt(); // Read the number of boats for the left ocean
      queueSizeLeft = barcosIzquierda;
      updateOceanQueue(boatQueueLeft, ledPinsLeft, queueSizeLeft);
      Serial.print("Carga inicial de barcos en el océano izquierdo: ");
      Serial.println(queueSizeLeft);
    }
    else if (input == 'R') {
      int barcosDerecha = Serial.parseInt(); // Read the number of boats for the right ocean
      queueSizeRight = barcosDerecha;
      updateOceanQueue(boatQueueRight, ledPinsRight, queueSizeRight);
      Serial.print("Carga inicial de barcos en el océano derecho: ");
      Serial.println(queueSizeRight);
    }

    // Handle right ocean inputs (q, w, e)
    else if (input == 'q' || input == 'w' || input == 'e') {
      if (queueSizeRight < 5) {
        boatQueueRight[queueSizeRight] = 1;
        queueSizeRight++;
        Serial.print("Barco agregado al océano derecho. Tamaño de la cola: ");
        Serial.println(queueSizeRight);
      } else {
        Serial.println("Cola del océano derecho llena.");
      }
    }
    // Handle left ocean inputs (r, t, y)
    else if (input == 'r' || input == 't' || input == 'y') {
      if (queueSizeLeft < 5) {
        boatQueueLeft[queueSizeLeft] = 1;
        queueSizeLeft++;
        Serial.print("Barco agregado al océano izquierdo. Tamaño de la cola: ");
        Serial.println(queueSizeLeft);
      } else {
        Serial.println("Cola del océano izquierdo llena.");
      }
    }
    // Remove boat from right ocean
    else if (input == 'd') {
      if (queueSizeRight > 0) {
        removeBoatFromQueue(boatQueueRight, queueSizeRight);
        queueSizeRight--;
      } else {
        Serial.println("No hay barcos en la cola del océano derecho.");
      }
    }
    // Remove boat from left ocean
    else if (input == 'i') {
      if (queueSizeLeft > 0) {
        removeBoatFromQueue(boatQueueLeft, queueSizeLeft);
        queueSizeLeft--;
      } else {
        Serial.println("No hay barcos en la cola del océano izquierdo.");
      }
    }
    // Start crossing (blink LED)
    else if (input == 'c') {
      isCrossing = true;
      Serial.println("Barco cruzando, LED de cruce activado.");
    }
    // Stop crossing (stop blinking LED)
    else if (input == 'o') {
      isCrossing = false;
      digitalWrite(crossingLedPin, LOW);
      Serial.println("Barco ha terminado de cruzar, LED de cruce apagado.");
    }

    else if (input == '7') { // Constant light
      specialLedMode = 0;
      digitalWrite(specialLedPin, HIGH);
    }
    else if (input == '8') { // Slow blink
      specialLedMode = 1;
      specialLedInterval = 1000; // 1 second interval for slow blink
    }
    else if (input == '9') { // Fast blink
      specialLedMode = 2;
      specialLedInterval = 50; // 0.1 second interval for fast blink
    }
    
    if (specialLedMode == 1 || specialLedMode == 2) {
    unsigned long currentMillis = millis();
    if (currentMillis - specialLedPreviousMillis >= specialLedInterval) {
      specialLedPreviousMillis = currentMillis;
      specialLedState = !specialLedState;
      digitalWrite(specialLedPin, specialLedState);
    }
  }

    // Update both oceans after each change
    updateOceanQueue(boatQueueRight, ledPinsRight, queueSizeRight);
    updateOceanQueue(boatQueueLeft, ledPinsLeft, queueSizeLeft);
  }
}

// Function to remove the first boat from the queue and shift the others
void removeBoatFromQueue(int boatQueue[], int queueSize) {
  for (int i = 0; i < 4; i++) {
    boatQueue[i] = boatQueue[i + 1]; // Shift the queue
  }
  boatQueue[4] = 0; // Empty the last position
  Serial.println("Barco quitado de la cola.");
}

// Function to update the state of the ocean queue LEDs
void updateOceanQueue(int boatQueue[], const int ledPins[], int queueSize) {
  for (int i = 0; i < 5; i++) {
    if (i < queueSize) {
      digitalWrite(ledPins[i], HIGH); // Turn on the LED for the current boat
      boatQueue[i] = 1; // Set the boat in the queue
    } else {
      digitalWrite(ledPins[i], LOW); // Turn off the LED if no boat
      boatQueue[i] = 0;
    }
  }
}
