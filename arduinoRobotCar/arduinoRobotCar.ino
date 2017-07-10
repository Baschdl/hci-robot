
/*
  Robot car controlled by 4 light sensors
 */

//-------------------------------------------------------------------------------------------------

// set colors
int const white = 0;
int const grey = 1;
int const black = 2;


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
int initial_mean_white = 0;
int initial_mean_grey = 128;
int initial_mean_black = 255;

int whiteA[history_size];
int greyA[history_size];
int blackA[history_size];

//state
int lastColor;
int currentColor;
boolean lastGreyWhite = false;

//-------------------------------------------------------------------------------------------------

//------------------------------------------initialization-----------------------------------------
void setup() {
  //init color history
  for(int i = 0; i < history_size; i++){
    whiteA[i] = initial_mean_white;
    greyA[i] = initial_mean_grey;
    blackA[i] = initial_mean_black;
  }

  Serial.begin(9600);
  pinMode(forwardPin, INPUT);
  pinMode(backwardPin, INPUT);
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
  readSensors();
  while(colorOfStrip(leftPinValue) != colorOfStrip(rightPinValue)){
    Forward();
    delay(50);
    Stop();
  }
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
  //check if one sensor is already in another strip -> we might drive schräg
  if(colorOfStrip(leftPinValue) != colorOfStrip(rightPinValue)){
    alterDirection();
  }
  //else??

  //actualisation of state
  lastColor = currentColor;
  currentColor = colorOfStrip(leftPinValue);

	driveToBrightness();
	
	// print values
	printLightValues(forwardPinValue, backwardPinValue, leftPinValue, rightPinValue);    

	delay(TIMER); 
	Stop();
}

//

//gives back the color to the given sensorvalue
int colorOfStrip(int sensorvalue){
  int distWhite = abs (sensorvalue - mean(white));
  int distGrey = abs (sensorvalue - mean(grey));
  int distBlack = abs (sensorvalue - mean(black));
  //refactor
  if(distWhite < distGrey){
    if(distWhite < distBlack){
      return white;
    }
    else{
      return black;
    }
  }
  else{
    if(distGrey < distBlack){
      return grey;
    }
    else{
      return black;
    }
  }
  
}
//give back mean of color history array
int mean(int color){
  //TODO pu new value in
  int sum = 0;
  switch(color) {
    case white:
      for(int i = 0; i < history_size; i++){
        sum += whiteA[i];
      }
      return sum/history_size;
      break;
    case grey:
      for(int i = 0; i < history_size; i++){
        sum += greyA[i];
      }
      return sum/history_size;
      break;
    case black:
      for(int i = 0; i < history_size; i++){
        sum += blackA[i];
      }
      return sum/history_size;
      break;
    }
}

//alter Direction
void alterDirection(){
//TODO implement

}

//----------------------------------readSensors-------------------------------------------------------
void readSensors()
{
	forwardPinValue = analogRead(forwardPin);
	backwardPinValue = analogRead(backwardPin);
	leftPinValue = analogRead(leftPin);
	rightPinValue = analogRead(rightPin);
}

//
//----------------------------------determine direction-----------------------------------------------
void driveToBrightness()
{
	 if(lastColor = currentColor){
    //color is the same we are in the same strip we drive straight
    Forward();
   //stop
   }
   else{
    if((lastColor = black && currentColor = white) || (lastColor = grey && currentColor = black) || (lastColor = white && currentColor = grey)){
      //umdrehen
    }
    else{
      if(lastColor = grey && currentColor = white){
        lastGreyWhite
        }  
    }
   }
  
  
  
 
	
	
	//
	// 1.) use the values read from the light sensors to determine the direction
	//
	
	
	
	
	//
	// 2.) drive in this direction
	//
	
	
	
}

//
//----------------------------------debug functions --------------------------------------------------
void printLightValues(int forward, int backward, int left, int right)
{
    Serial.print("light sensors: (");    
    Serial.print(forward);
    Serial.print(backward);
    Serial.print(left);
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
