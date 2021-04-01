
const int waitTime=2500; // 2.5 seconds between button presses
int sequence[100];           
int sequenceLength = 0;              
int inputCount = 0;         
int lastInput = 0;           
int expectedInput = 0;               
bool isButtonPressed = false;          
bool wait = false;            
bool isLost = false;    
byte noPins = 3;                                 
byte pins[] = {2, 10, 8};                   
long inputTime = 0;         
int knobPin = A1;
const int ledPinR = 3;
bool ledStateR = LOW;

void setup() {
  Serial.begin(9600);  
  pinMode(ledPinR, OUTPUT);       
  Reset();
}

/// Sets all the pins as either INPUT or OUTPUT based on the value of 'dir'
void setPinDirection(byte dir){
  for(byte i = 0; i < noPins; i++){
    pinMode(pins[i], dir); 
  }
}
void writeAllPins(byte val){
  for(byte i = 0; i < noPins; i++){
    digitalWrite(pins[i], val); 
  }
}

void flashLEDs(int freq){  //flashing all LEDs
  setPinDirection(OUTPUT); 
  for(int i = 0; i < 3; i++){
    writeAllPins(HIGH);
    delay(freq);
    writeAllPins(LOW);
    delay(freq);
  }
}

void Reset(){   //resetting to default
  flashLEDs(500);
  sequenceLength = 0;
  inputCount = 0;
  lastInput = 0;
  expectedInput = 0;
  isButtonPressed = false;
  wait = false;
  isLost = false;
}

void gameLost(){
  flashLEDs(50); 
  Reset();    
}


void playGame(){
  for(int i = 0; i < sequenceLength; i++){
      digitalWrite(sequence[i], HIGH);
      delay(500);
      digitalWrite(sequence[i], LOW);
      delay(250);
    } 
}

void loop() {  

  int knobValue = analogRead(knobPin);
  Serial.println(knobValue);
  int mappedValue = map(knobValue, 495, 1023, 0, 255);
  analogWrite(ledPinR, mappedValue);
  
  if(!wait && mappedValue>200){      
    setPinDirection(OUTPUT);                      
    
    randomSeed(analogRead(A0));                   // https://www.arduino.cc/en/Reference/RandomSeed
    sequence[sequenceLength] = pins[random(0,noPins)]; 
    sequenceLength++;                                     
    
    playGame();                              
    
    wait = true;                                  
    inputTime = millis();                        
  }else{ 
           //Player's turn 
    setPinDirection(INPUT);                       
    if(millis() - inputTime > waitTime){  
      gameLost();                           
      return;
    }      
        
    if(!isButtonPressed){                                   
      expectedInput = sequence[inputCount];               
      
      for(int i = 0; i < noPins; i++){           
        if(pins[i]==expectedInput)                        
          continue;                               
        if(digitalRead(pins[i]) == HIGH){         
          lastInput = pins[i];
          isLost = true;                       
          isButtonPressed = true;                          
        }
      }      
    }

    if(digitalRead(expectedInput) == 1 && !isButtonPressed)      // The player pressed the right button
    {
      inputTime = millis();                       
      lastInput = expectedInput;
      inputCount++;                               
      isButtonPressed = true;                                              
    }else{
      if(isButtonPressed && digitalRead(lastInput) == LOW){  
        isButtonPressed = false;
        delay(20);
        if(isLost){                              
          gameLost();                         
        }
        else{
          if(inputCount == sequenceLength){            
            wait = false;                           
            inputCount = 0;                         
            delay(1500);
          }
        }
      }
    }    
  }
}
