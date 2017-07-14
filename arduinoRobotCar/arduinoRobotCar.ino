
/*
  Robot car controlled by 4 light sensors
 */

//-------------------------------------------------------------------------------------------------

// set colors
int const white = 0;
int const grey = 1;
int const black = 2;

int led1 = 13;
int led2 = 12;
int led3 = 8;
int led4 = 7;
int led5 = 2;

int TIMER =  100; // init to 1 second
int forwardPinValue = 0;
int backwardPinValue = 0;
int leftPinValue = 0;
int rightPinValue = 0;

/* analog pins for the 4 light sensors*/
int forwardPin = A0;
int backwardPin = A2;
int leftPin = A1;
int rightPin = A3;

/* digital pins for the Hbridge controlling the 2 motors */
int motor1PinA = 3;
int motor1PinB = 4;
int enablePinMotor1 = 9; 

int motor2PinA = 5;
int motor2PinB = 6;
int enablePinMotor2 = 10; 

int hbridePin = 11;

/*parameters for drivin algo*/
int const history_size = 30;
int initial_mean_white = 250;
int initial_mean_grey = 500;
int initial_mean_black = 1000;

int whiteA[history_size];
int whiteArrayCounter = 0;

int greyA[history_size];
int greyArrayCounter = 0;

int blackA[history_size];
int blackArrayCounter = 0;

//state
int lastColor;
int currentColor;
boolean lastGreyWhite = false;
boolean forward = true;
int correctDirectionCount = 0;
boolean won = false;
//-------------------------------------------------------------------------------------------------

//------------------------------------------initialization-----------------------------------------
void setup() {
  //init color history
  for(int i = 0; i < history_size; i++){
    whiteA[i] = initial_mean_white;
    greyA[i] = initial_mean_grey;
    blackA[i] = initial_mean_black;
  }

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);

  Serial.begin(9600);
  /*pinMode(forwardPin, INPUT);
  pinMode(backwardPin, INPUT);*/
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);

  // if necessary enable the pull up resistor
  //digitalWrite(forwardPin, HIGH); 
  //digitalWrite(backwardPin, HIGH); 
  //digitalWrite(leftPin, HIGH);
  //digitalWrite(leftPin, HIGH);  
   
  pinMode(motor1PinA, OUTPUT);
  pinMode(motor1PinB, OUTPUT);
  pinMode(motor2PinA, OUTPUT);
  pinMode(motor2PinB, OUTPUT);

  // set enablePin high so that motors can turn on:
  digitalWrite(hbridePin, HIGH); 
  digitalWrite(enablePinMotor1, HIGH); 
  digitalWrite(enablePinMotor2, HIGH); 

  //init so we start with both sensor in the same strip
  /*readSensors();
  while(colorOfStrip(leftPinValue) != colorOfStrip(rightPinValue)){
    //forward();
    delay(50);
    //Stop();
  }*/
  //init state
  lastColor = colorOfStrip(leftPinValue);
  currentColor = colorOfStrip(leftPinValue);
  
  Serial.println("Car initialized, ready to go!");
}

//
//-----------------------------------main loop-----------------------------------------------------------
void loop() {
  // retrieve values of lightness
  readSensors();
  delay(1000);
  printLightValues(leftPinValue, rightPinValue, colorOfStrip(leftPinValue), colorOfStrip(rightPinValue));
  //check if one sensor is already in another strip -> we might drive schräg
  if(!checkDirection()){
    correctDirectionCount = 0;
    //actualisation of state
    stateUpdate(colorOfStrip(leftPinValue)); 
    driveToBrightness();
    delay(100);
    Stop();
  }

  if(won){
    digitalWrite(led1, HIGH);
    delay(50);
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    delay(50);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
    delay(50);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
    delay(50);
    digitalWrite(led4, LOW);
    digitalWrite(led5, HIGH);
    delay(50);
    digitalWrite(led5, LOW);
  }
}


//----------------------------------drive to aim-----------------------------------------------
void driveToBrightness()
{
   if(lastColor == currentColor){
    //color is the same we are in the same strip we drive straight
    driveForward();
   }
   else{
    if((lastColor == black && currentColor == white) || (lastColor == grey && currentColor == black) || (lastColor == white && currentColor == grey)){
      if(lastColor == white && currentColor == grey && lastGreyWhite){
        Serial.print("whuuup whuup we won");
        won = true;
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        driveBackward();
        delay(20);
        Stop();
      }else{
        changeDirection();
        driveForward(); 
      }
      lastGreyWhite = false;
    }
    else{
      if(lastColor == grey && currentColor == white){
        lastGreyWhite = true;
      }else{
        lastGreyWhite = false;
      }
      driveForward(); 
    }
   }
}


//gives back the color to the given sensorvalue
int colorOfStrip(int sensorvalue){
  int distWhite = abs(sensorvalue - mean(whiteA));
  int distGrey = abs(sensorvalue - mean(greyA));
  int distBlack = abs(sensorvalue - mean(blackA));
  if(distWhite < distGrey){
    pushArrayValue(whiteA, sensorvalue, &whiteArrayCounter);
    return white;
  }
  else{
    if(distGrey < distBlack){
      pushArrayValue(greyA, sensorvalue, &greyArrayCounter);
      return grey;
    }
    else{
      pushArrayValue(blackA, sensorvalue, &blackArrayCounter);
      return black;
    }
  }
  
}
//give back mean of color history array
int mean(int* colorArray){
  int sum = 0;
  for(int i = 0; i < history_size; i++){
    sum += colorArray[i];
  }
  return sum/history_size;
}

void pushArrayValue(int* colorArray, int value, int* counter){
  colorArray[*counter] = value;
  incCounter(counter);
}

void incCounter(int* counter){
  if(*counter < (history_size - 1)){
    *counter++;
  }
  else{
    *counter = 0;
  }
}



boolean checkDirection(){
  if(correctDirectionCount > 200){
    possibleErrorPrevention();
  }
  int leftColor = colorOfStrip(leftPinValue);
  int rightColor = colorOfStrip(rightPinValue);
  if(leftColor != rightColor){
    correctDirectionCount++;
    correctDirection(leftColor, rightColor);
    return true;
  }
  else{
    return false;
  }
}

//TODO

void possibleErrorPrevention(){
  Serial.println("Error, now driving around like crazy");
  int leftColor = colorOfStrip(leftPinValue);
  int rightColor = colorOfStrip(rightPinValue);
  while(leftColor != rightColor){
    Forward();
    delay(100);
  }
}

void correctDirection(int leftColor, int rightColor){
  int rotationTime = 20;
  int forwardTime = 10;
  if(currentColor != leftColor){
    digitalWrite(led4, HIGH);
    if(forward){
      RotateRight();
      delay(rotationTime);
      Forward();
      delay(forwardTime);
      Stop();
    }
    else{
      RotateLeft();
      delay(rotationTime);
      Backward();
      delay(forwardTime);
      Stop();
    }
    digitalWrite(led4, LOW);
  }
  else{
    digitalWrite(led5, HIGH);
    if(forward){
      RotateLeft();
      delay(rotationTime);
      Backward();
      delay(forwardTime);
      Stop();
    }
    else{
      RotateRight();
      delay(rotationTime);
      Forward();
      delay(forwardTime);
      Stop();
    }
    digitalWrite(led5, LOW);  
  }
}

void stateUpdate(int colorOfCurrentStrip){
  lastColor = currentColor;
  currentColor = colorOfCurrentStrip;
}

//----------------------------------readSensors-------------------------------------------------------
void readSensors()
{
	/*forwardPinValue = analogRead(forwardPin);
	backwardPinValue = analogRead(backwardPin);*/
	leftPinValue = analogRead(leftPin);
	rightPinValue = analogRead(rightPin);
  Serial.print("read sensors");
}

void changeDirection(){
  forward = !forward;
}
void driveForward(){
  if(forward){
    Forward();
  }
  else{
    Backward();
  }
}

void driveBackward(){
  if(forward){
    Backward();
  }
  else{
    Forward();
  }
}
//
//----------------------------------debug functions --------------------------------------------------
void printLightValues(int forward, int backward, int left, int right)
{
    Serial.println("light sensors: (");    
    Serial.print(forward);
    Serial.print(" ! ");  
    Serial.print(backward);
    Serial.print(" ! "); 
    Serial.print(left);
    Serial.print(" ! "); 
    Serial.println(right);
}


//
//----------------------------------motor functions --------------------------------------------------
void Forward()
{
  Serial.println("move forward");
  digitalWrite(motor1PinA, HIGH);
  digitalWrite(motor1PinB, LOW);
  digitalWrite(motor2PinA, HIGH);
  digitalWrite(motor2PinB, LOW);
}

void Backward() // inverse voltage of both motors
{
  Serial.println("move backward");
  digitalWrite(motor1PinA, LOW);
  digitalWrite(motor1PinB, HIGH);
  digitalWrite(motor2PinA, LOW);
  digitalWrite(motor2PinB, HIGH);
}

void RotateLeft() // inverse voltage of only one motor
{
  Serial.println("rotate left");
  digitalWrite(motor1PinA, LOW);
  digitalWrite(motor1PinB, HIGH);
  digitalWrite(motor2PinA, HIGH);
  digitalWrite(motor2PinB, LOW);
}

void RotateRight() // inverse voltage of only one motor
{
  Serial.println("rotate right");
  digitalWrite(motor1PinA, HIGH);
  digitalWrite(motor1PinB, LOW);
  digitalWrite(motor2PinA, LOW);
  digitalWrite(motor2PinB, HIGH);
}

void Stop() 
{
  Serial.println("stop");
  digitalWrite(motor1PinA, LOW);
  digitalWrite(motor1PinB, LOW);
  digitalWrite(motor2PinA, LOW);
  digitalWrite(motor2PinB, LOW);
}
