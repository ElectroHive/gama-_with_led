#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD address (could be 0x3F)

// ---------- Pins ----------
int leds[] = {2, 3, 4, 5, 6, 7}; // 6 LEDs (yellow in the middle)
int numLeds = 6;
int yellowLed1 = 3; // First yellow LED
int yellowLed2 = 4; // Second yellow LED

int btn1 = 8;
int btn2 = 9;
int buzzer = 10;

// ---------- Scores ----------
int score1 = 0;
int score2 = 0;
int winScore = 5; // Winning score

// ---------- Ready States ----------
bool ready1 = false;
bool ready2 = false;

void setup() {
  lcd.init();
  lcd.backlight();
  
  for (int i = 0; i < numLeds; i++) pinMode(leds[i], OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  welcomeScreen();
  rulesScreen();
  waitForReady();
}

void loop() {
  gameRound();
  if (score1 >= winScore || score2 >= winScore) {
    showWinner();
    while (1); // Game ends
  }
}

// ---------- Functions ----------

void welcomeScreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome to Game!");
  delay(2000);
}

void rulesScreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press when LED");
  lcd.setCursor(0,1);
  lcd.print("is YELLOW!");
  delay(2500);
}

void waitForReady() {
  lcd.clear();
  ready1 = false; ready2 = false;
  while (!(ready1 && ready2)) {
    lcd.setCursor(0,0);
    lcd.print("Press if READY");

    if (digitalRead(btn1) == LOW) ready1 = true;
    if (digitalRead(btn2) == LOW) ready2 = true;

    lcd.setCursor(0,1);
    lcd.print("P1:");
    lcd.print(ready1 ? "OK " : "..."); 
    lcd.print(" P2:");
    lcd.print(ready2 ? "OK " : "...");
    delay(200);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Game Start!");
  delay(2000);
}

void gameRound() {
  int baseDelay = 300;              // Base LED speed
  int speedUp = (score1 + score2) * 30; // Speed increases with score
  int delayTime = max(50, baseDelay - speedUp); // Minimum delay = 50ms

  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], HIGH);
    
    // Short beep with buzzer synced with LEDs
    tone(buzzer, 800);        
    delay(delayTime / 2);     
    noTone(buzzer);           
    delay(delayTime / 2);     

    if (digitalRead(btn1) == LOW || digitalRead(btn2) == LOW) {
      checkWinner(i);
      return; // End round after button press
    }

    digitalWrite(leds[i], LOW);
  }
}

void checkWinner(int ledIndex) {
  lcd.clear();
  bool yellow = (leds[ledIndex] == yellowLed1 || leds[ledIndex] == yellowLed2);

  if (digitalRead(btn1) == LOW) {
    if (yellow) { score1++; lcd.print("P1 Correct!"); tone(buzzer,1200,150); }
    else { lcd.print("P1 Wrong!"); tone(buzzer,300,300); }
  }
  if (digitalRead(btn2) == LOW) {
    if (yellow) { score2++; lcd.setCursor(0,1); lcd.print("P2 Correct!"); tone(buzzer,1200,150); }
    else { lcd.setCursor(0,1); lcd.print("P2 Wrong!"); tone(buzzer,300,300); }
  }

  // 🎉 Flashing LED effect
  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < numLeds; k++) digitalWrite(leds[k], HIGH);
    delay(400);
    for (int k = 0; k < numLeds; k++) digitalWrite(leds[k], LOW);
    delay(200);
  }

  showScore();
}

void showScore() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P1:");
  lcd.print(score1);
  lcd.setCursor(8,0);
  lcd.print("P2:");
  lcd.print(score2);
  delay(1500);
}

void showWinner() {
  lcd.clear();
  if (score1 >= winScore) lcd.print("Winner: P1!");
  else lcd.print("Winner: P2!");
  
  // Flashing LCD effect for winner
  for (int j=0;j<6;j++){
    lcd.noBacklight(); delay(300);
    lcd.backlight(); delay(300);
  }
}
