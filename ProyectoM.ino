// PINS
//  Motor
int izqA = 9;
int izqB = 10;
int derA = 11;
int derB = 12;

//  Motor A for Suspension
int const ENA_SUSP = 3;
int const IN1 = 5;
int const IN2 = 4;
//  Motor B for Suspension
int const ENB_SUSP = 6;
int const IN3 = 7;
int const IN4 = 8;

// CONSTANTS
int const MIN_SPEED = 10;    // Set to minimum PWM value that will make motors turn
int const ACCEL_DELAY = 50;  // delay between steps when ramping motor speed up or down.
int const inc_dec_amount = 2;

// VARIABLES
char bluetooth;
bool suspension_mode;
// Suspension
char susp_curr_direction_right;
char susp_curr_direction_left;
int susp_curr_speed_right;
int susp_curr_speed_left;
// Wheels
int wheel_speed;

//===============================================================================
//  Initialization
//===============================================================================
void setup() {
  // Wheels
  pinMode(derA, OUTPUT);
  pinMode(derB, OUTPUT);
  pinMode(izqA, OUTPUT);
  pinMode(izqB, OUTPUT);

  // Suspension
  pinMode(ENA_SUSP, OUTPUT);  // set all the motor control pins to outputs
  pinMode(ENB_SUSP, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(derA, LOW);
  digitalWrite(derB, LOW);
  digitalWrite(izqA, LOW);
  digitalWrite(izqB, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Default selections for Suspension
  suspension_mode = false;
  susp_curr_direction_right = 'F';  // forward
  susp_curr_direction_left = 'F';   // forward
  susp_curr_speed_right = 0;        // stop
  susp_curr_speed_left = 0;         // stop

  // Default selections for Wheels
  wheel_speed = 0;

  Serial.begin(38400);  // Set comm speed for serial monitor messages and bluetooth
}
//===============================================================================
//  Main
//===============================================================================
void loop() {
  Serial.println(bluetooth);
  if (Serial.available()) {
    bluetooth = Serial.read();
    Serial.println(bluetooth);
  }

  // mode selection
  if (bluetooth == 'X')
    suspension_mode = true;
  else if (bluetooth == 'x')
    suspension_mode = false;

  if (suspension_mode) {
    switch (bluetooth) {
      case 'F':
        // set preference using bluetooth input
        setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
        setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
        break;
      case 'B':
        // set preference using bluetooth input
        setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
        setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
        break;

      // forward-right & back-right respectively
      case 'I':
        setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
        break;
      case 'J':
        setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
        break;

      // forward-left & back-left respectively
      case 'G':
        setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
        break;
      case 'H':
        setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
        break;

      // stop
      case 'W':
      case 'w':
        susp_curr_direction_right = 'F';  // forward
        susp_curr_speed_right = 0;        // stop
        susp_curr_direction_left = 'F';   // forward
        susp_curr_speed_left = 0;         // stop
        break;

      default:
        break;
    }
    // apply selected state
    DriverMotor('A', susp_curr_direction_right, susp_curr_speed_right);
    DriverMotor('B', susp_curr_direction_left, susp_curr_speed_left);
  }

  else {
    // wheel speed selection
    switch (bluetooth) {
      case '0':
        wheel_speed = 0;
        break;
      case '1':
        wheel_speed = 10;
        break;
      case '2':
        wheel_speed = 20;
        break;
      case '3':
        wheel_speed = 30;
        break;
      case '4':
        wheel_speed = 40;
        break;
      case '5':
        wheel_speed = 50;
        break;
      case '6':
        wheel_speed = 60;
        break;
      case '7':
        wheel_speed = 70;
        break;
      case '8':
        wheel_speed = 80;
        break;
      case '9':
        wheel_speed = 90;
        break;
      case 'q':
        wheel_speed = 100;
        break;
      default:
        break;
    }
    switch (bluetooth) {
      case 'S':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, LOW);
        digitalWrite(izqA, LOW);
        break;
      case 'F':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, HIGH);
        digitalWrite(izqA, HIGH);
        break;
      case 'B':
        digitalWrite(derA, LOW);
        digitalWrite(izqA, LOW);
        digitalWrite(derB, HIGH);
        digitalWrite(izqB, HIGH);
        break;
      case 'R':
        digitalWrite(derB, HIGH);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, LOW);
        digitalWrite(izqA, HIGH);
        break;
      case 'L':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, HIGH);
        digitalWrite(izqA, LOW);
        digitalWrite(derA, HIGH);
        break;
      default:
        break;
    }
  }
  delay(10);
}
/*
 * Motor function does all the heavy lifting of controlling the motors
 * mot = motor to control either 'A' or 'B'.  'C' controls both motors.
 * dir = Direction either 'F'orward or 'R'everse
 * speed = Speed.  Takes in 1-100 percent and maps to 0-255 for PWM control.  
 * Mapping ignores speed values that are too low to make the motor turn.
 * In this case, anything below 27, but 0 still means 0 to stop the motors.
 */
void DriverMotor(char mot, char dir, int speed) {
  // remap the speed from range 0-100 to 0-255
  int newspeed;
  if (speed == 0)
    newspeed = 0;  // Don't remap zero, but remap everything else.
  else
    newspeed = map(speed, 1, 100, MIN_SPEED, 255);

  switch (mot) {
    case 'A':  // Controlling Motor A
      if (dir == 'F') {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      analogWrite(ENA_SUSP, newspeed);
      break;

    case 'B':  // Controlling Motor B
      if (dir == 'F') {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      analogWrite(ENB_SUSP, newspeed);
      break;

    case 'C':  // Controlling Both Motors
      if (dir == 'F') {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      analogWrite(ENA_SUSP, newspeed);
      analogWrite(ENB_SUSP, newspeed);
      break;
  }
}

// void ShieldMotor(char mot, char dir, int speed) {
//   // remap the speed from range 0-100 to 0-255
//   int newspeed;
//   if (speed == 0)
//     newspeed = 0;  // Don't remap zero, but remap everything else.
//   else
//     newspeed = map(speed, 1, 100, MIN_SPEED, 255);

//   switch (mot) {
//     case 'A':  // Controlling Motor A
//       if (dir == 'F') {
//         digitalWrite(INA_WHEEL, HIGH);
//       } else if (dir == 'R') {
//         digitalWrite(INB_WHEEL, LOW);
//       }
//       analogWrite(ENA_WHEEL, newspeed);
//       break;

//     case 'B':  // Controlling Motor B
//       if (dir == 'F') {
//         digitalWrite(INB_WHEEL, HIGH);
//       } else if (dir == 'R') {
//         digitalWrite(INA_WHEEL, LOW);
//       }
//       analogWrite(ENB_WHEEL, newspeed);
//       break;

//     case 'C':  // Controlling Both Motors
//       if (dir == 'F') {
//         digitalWrite(INA_WHEEL, HIGH);
//         digitalWrite(INB_WHEEL, HIGH);
//       } else if (dir == 'R') {
//         digitalWrite(INA_WHEEL, LOW);
//         digitalWrite(INB_WHEEL, LOW);
//       }
//       analogWrite(ENA_WHEEL, newspeed);
//       analogWrite(ENB_WHEEL, newspeed);
//       break;
//   }
// }

void setDirAndSpeed(char requested_dir, char &curr_direction, int &curr_speed) {
  switch (requested_dir) {
    case 'F':
      if (curr_direction == 'F' && 100 - inc_dec_amount < curr_speed)
        curr_speed = 100;

      else if (curr_direction == 'F' && 0 <= curr_speed)
        curr_speed += inc_dec_amount;

      else if (curr_direction == 'R' && curr_speed <= MIN_SPEED + inc_dec_amount) {
        curr_direction = 'F';  // forward
        curr_speed = 0;
      }

      else
        curr_speed -= inc_dec_amount;
      break;

    case 'B':
      if (curr_direction == 'R' && 100 - inc_dec_amount < curr_speed)
        curr_speed = 100;

      else if (curr_direction == 'R' && 0 <= curr_speed)
        curr_speed += inc_dec_amount;

      else if (curr_direction == 'F' && curr_speed <= MIN_SPEED + inc_dec_amount) {
        curr_direction = 'R';  // forward
        curr_speed = 0;
      }
      else
        curr_speed -= inc_dec_amount;
      break;

    default:
      // preserve current state
      break;
  }
}