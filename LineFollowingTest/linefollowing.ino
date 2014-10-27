/*
  Robot will be set up as such
   -------3-------
  w| 1   2 4   5 |w
   |             |
   |             |
*/

/*
  Uno has the following digital pins

  0 - rx  | not used
  1 - tx  | not used
  2 -     | left motor drive forward
  3 - pwm | left motor drive backward
  4 -     | line following 1
  5 - pwm | right motor drive forward
  6 - pwm | right motor drive backward
  7 -     | line following 2
  8 -     | line following 3
  9 - pwm | not used
  10- pwm | not used
  11- pwm | not used
  12-     | line following 4
  13-     | line following 5
*/


////////// Line Following Variables \\\\\\\\\\

  // pins for the line sensors
  int line_following_pin_1 = 4;
  int line_following_pin_2 = 7;
  int line_following_pin_3 = 8;
  int line_following_pin_4 = 12;
  int line_following_pin_5 = 13;

  // saved values for the line_following_pin inputs
  boolean line_following_val_1 = false;
  boolean line_following_val_2 = false;
  boolean line_following_val_3 = false;
  boolean line_following_val_4 = false;
  boolean line_following_val_5 = false;


////////// Motor Controller Variables \\\\\\\\\\

  //pins for the outputs for the motorcontroller
  int left_motor_drive_forward = 2;
  int left_motor_drive_backward = 3;
  int right_motor_drive_forward = 5;
  int right_motor_drive_backward = 6;

  // drive direction for each motor
  // if >0 then forward, if <0 then backward
  int left_motor_drive = 0;
  int right_motor_drive = 0;

  //time for reading the movement controlling
  control_sleep_time = 100
  turn_forward_iterations = 10
  turn_initial_iterations = 10


////////// Intersection Variables \\\\\\\\\\

  // last intersection lines
  boolean last_turn_left = false;
  boolean last_turn_right = false;
  boolean last_turn_up = false;

  // if we followed the last intersection lines
  boolean last_turn_resolved_left = false;
  boolean last_turn_resolved_right = false;
  boolean last_turn_resolved_up = false;

  // the last intersection line we followed
  boolean last_choice_left = false;
  boolean last_choice_right = false;
  boolean last_choice_up = false;


// function called at the start of an arduino program
void setup() {
  
  // initialize the line sensing pins
  pinMode(line_following_pin_1, INPUT);
  pinMode(line_following_pin_2, INPUT);
  pinMode(line_following_pin_3, INPUT);
  pinMode(line_following_pin_4, INPUT);
  pinMode(line_following_pin_5, INPUT);
  
  // initialize the drive outputs
  pinMode(left_motor_drive_forward, OUTPUT);
  pinMode(left_motor_drive_backward, OUTPUT);
  
  pinMode(right_motor_drive_forward, OUTPUT);
  pinMode(right_motor_drive_backward, OUTPUT);
}

// function to read the line sensor inputs to the values
void readLineSensors() {
  line_following_val_1 = digitalRead(line_following_pin_1);
  line_following_val_2 = digitalRead(line_following_pin_2);
  line_following_val_3 = digitalRead(line_following_pin_3);
  line_following_val_4 = digitalRead(line_following_pin_4);
  line_following_val_5 = digitalRead(line_following_pin_5);
}

void setMotorDrives() {
  if (left_motor_drive > 0) {
    digitalWrite(left_motor_drive_backward, false);
    digitalWrite(left_motor_drive_forward, true);
  }
  else if (left_motor_drive < 0) {
    digitalWrite(left_motor_drive_forward, false);
    digitalWrite(left_motor_drive_forward, true);
  }
  else {
    digitalWrite(left_motor_drive_forward, false);
    digitalWrite(left_motor_drive_forward, false);
  }

  if (right_motor_drive > 0) {
    digitalWrite(right_motor_drive_backward, false);
    digitalWrite(right_motor_drive_forward, true);
  }
  else if (right_motor_drive < 0) {
    digitalWrite(right_motor_drive_forward, false);
    digitalWrite(right_motor_drive_forward, true);
  }
  else {
    digitalWrite(right_motor_drive_forward, false);
    digitalWrite(right_motor_drive_forward, false);
  }
}

void setTurnLeft() {
  left_motor_drive  =  1;
  right_motor_drive = -1;
}

void setTurnRight() {
  left_motor_drive  = -1;
  right_motor_drive =  1;
}

void setTurnLeft1Wheel() {
  left_motor_drive  = 1;
  right_motor_drive = 0;
}

void setTurnRight1Wheel() {
  left_motor_drive  = 0;
  right_motor_drive = 1;
}

void setMoveForward() {
  left_motor_drive  = 1;
  right_motor_drive = 1;
}

// function that determines if we are at in intersection
boolean isAtIntersection() {
  return (line_following_val_1 || line_following_val_5);
}

//functions to check if we are within the line bounds
boolean isVeeringToRight() {
  return line_following_val_2;
}

//functions to check if we are within the line bounds
boolean isVeeringToLeft() {
  return line_following_val_4;
}

// function that prepares us for the intersection logic
void clearIntersectionValues() {
  last_turn_left = false;
  last_turn_right = false;
  last_turn_up = false;

  last_turn_resolved_left = false;
  last_turn_resolved_right = false;
  last_turn_resolved_up = false;

  last_choice_left = false;
  last_choice_right = false;
  last_choice_up = false;  
}

// function that sets the intersection values
void setIntersectionValues() {
  clearIntersectionValues()

  if (line_following_val_1) {last_turn_left = true;}
  else {last_turn_left = false;}

  if (line_following_val_3) {last_turn_up = true;}
  else {last_turn_up = false;}

  if (line_following_val_5) {last_turn_right = true;}
  else {last_turn_right = false;}
}

// function that stays in the straight line 
// state until an intersection is reached
void performLineFollowingStraight() {
  while (true) {
    readLineSensors();

    if (isAtIntersection()) break;

    if (isVeeringToLeft()) setTurnRight1Wheel();
    else if (isVeeringToRight()) setTurnLeft1Wheel();
    else setMoveForward();

    setMotorDrives();
    delay(control_sleep_time);
  }
}

// functions that turns until we are in the line again
void performTurnLeft() {
  // drive up so the center of rotation is on the line
  for (int i = 0; i < turn_forward_iterations; i++) {
    setMoveForward();
    setMotorDrives();
    delay(control_sleep_time);
  }

  //turn left until past the initial line
  for (int i = 0; i < turn_initial_iterations; i++) {
    setTurnLeft();
    setMotorDrives();
    delay(control_sleep_time);
  }

  //keep turning until sensor 4 sees the line
  while(true) {
    readLineSensors();
    if (line_following_val_4) break;
    setTurnLeft();
    setMotorDrives();
    delay(control_sleep_time);
  }
}

// functions that turns until we are in the line again
void performTurnRight() {
  // drive up so the center of rotation is on the line
  for (int i = 0; i < turn_forward_iterations; i++) {
    setMoveForward();
    setMotorDrives();
    delay(control_sleep_time);
  }

  //turn right until past the initial line
  for (int i = 0; i < turn_initial_iterations; i++) {
    setTurnRight();
    setMotorDrives();
    delay(control_sleep_time);
  }

  //keep turning until sensor 2 sees the line
  while(true) {
    readLineSensors();
    if (line_following_val_2) break;
    setTurnRight();
    setMotorDrives();
    delay(control_sleep_time);
  }
}

void performIntersection() {
  //[TODO] - write logic for handling an intersection
}

void loop() {
  //[TODO] - write the loop for the gameboard logic
}
