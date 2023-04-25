int latchPin = 10;
int dataPin = 11;
int clockPin = 12;
int COLS = 8;
int ROWS = 8;

int sustain = 0;
int sustainPin = A0;

int base = 48;

int mask[8] = {
  B11111110,
  B11111101,
  B11111011,
  B11110111,
  B11101111,
  B11011111,
  B10111111,
  B01111111
};

int rowPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

boolean keyPressed[8][8];

void setup() {
  Serial.begin(31250);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(sustainPin, INPUT);
  for(int i = 0; i < COLS; i++) {
    pinMode(rowPins[i], INPUT);
  }
}
void loop() {
  if(digitalRead(sustainPin) == 0 && sustain == 0) {
    sustain = 1;
    noteOn(0xB0, 0x40, 0x40);
  }
  if(digitalRead(sustainPin) == 1 && sustain == 1) {
    sustain = 0;
    noteOn(0xB0, 0x40, 0x00);
  }
  
  for(int i = 0; i < COLS; i++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, mask[i]);
    digitalWrite(latchPin, HIGH);

    int rowValue[8];
    for(int j = 0; j < ROWS; j++) {
      rowValue[j] = digitalRead(rowPins[j]);
    }
    
    // process keys pressed
    for(int row = 0; row < ROWS; row++)
    {
      if(rowValue[row] == 0 && !keyPressed[row][i])
      {
        keyPressed[row][i] = true;
        int note = row * 8 + i + base;
        noteOn(0x90, note, 0x45);
      }
    }

    // process keys released
    for(int row = 0; row < ROWS; row++)
    {
      if(rowValue[row] == 1 && keyPressed[row][i])
      {
        keyPressed[row][i] = false;
        int note = row * 8 + i + base;
        noteOn(0x80, note, 0x00);
      }
    }
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
