#define TOUCH_PORT S1
#define GYRO_PORT S2
#define ULTRA_PORT S3
#define COLOR_PORT S4

void orientSensor(int orientation);
void drive(int leftSpeed, int rightSpeed);
void followLine();
void getBall();
void turnAngle(int motorPower, int angle);
void encoderDrive(int motorPower, int distance);
int get_colour();
void stop_then_proceed();
void dropoff(int colour);

task main()
{
	SensorType[COLOR_PORT] = sensorEV3_Color;
	SensorMode[COLOR_PORT] = modeEV3Color_Color;
	SensorType[TOUCH_PORT] = sensorEV3_Touch;
	wait1Msec(50);
	const int NUM_COLORS = 3;
	int currentColor = 0;
	string display = "", redTotal = "", blueTotal = "", yellowTotal = "";
	int colorCount[NUM_COLORS]={0, 0, 0};

	const int SENSE_BALL_POS = 0;
	const int SENSE_GROUND_POS = 1;

	const int STOP_SPEED = 0;
	const int SLOW_SPEED = 10;
	const int NORM_SPEED = 15;

	const int RED = 1;
	const int BLUE = 2;
	const int YELLOW = 3;

	while(!getButtonPress(buttonAny))
	{}

	while(getButtonPress(buttonAny))
	{}

	while(getTouchValue(TOUCH_PORT) == 0)
	{
		followLine();
	}
	drive(STOP_SPEED,STOP_SPEED);
	orientSensor(SENSE_BALL_POS);
	wait1Msec(100);
	currentColor = get_colour();

	while(currentColor != 0)
	{
		if (currentColor == RED)
		{
			display = "Red";
			colorCount[0] += 1;
		}
		else if (currentColor == BLUE)
		{
			display = "Blue";
			colorCount[1] += 1;
		}
		else if (currentColor == YELLOW)
		{
			display = "Yellow";
			colorCount[2] += 1;
		}
		displayString(5, display);
		encoderDrive(-SLOW_SPEED, -5);
		getBall();
		orientSensor(SENSE_GROUND_POS);
		while(getColorName(S4) != colorRed)
		{
			stop_then_proceed();
			drive(NORM_SPEED,NORM_SPEED);
		}
		dropoff(currentColor);
		encoderDrive(NORM_SPEED, 10);
		while(getTouchValue(TOUCH_PORT) == 0)
		{
			followLine();
		}
		drive(STOP_SPEED,STOP_SPEED);
		orientSensor(SENSE_BALL_POS);
		wait1Msec(100);
		currentColor = get_colour();
	}
	redTotal = colorCount[0];
	blueTotal = colorCount[1];
	yellowTotal = colorCount[2];
	displayString(3, "Total number of red balls");
	displayString(4, redTotal);
	displayString(5, "Total number of blue balls");
	displayString(6, blueTotal);
	displayString(7, "Total number of yellow balls");
	displayString(8, yellowTotal);
	while(!getButtonPress(buttonAny))
	{}

	while(getButtonPress(buttonAny))
	{}
}

/*
This is the orient sensor function, it moves the motor that
moves the color sensor. It takes in the desired orientation
of the sensor and moves it to that preset value.
*/
void orientSensor(int orientation)
{
	nMotorEncoder[motorB] = 0;
	int motorAngle = 90;
	int motorSpeed = 5;
	if (orientation == 1)
	{
		motor[motorB] = -motorSpeed;
		while(nMotorEncoder[motorB] > -motorAngle)
		{}
	}
	else
	{
		motor[motorB] = motorSpeed;
		while(nMotorEncoder[motorB] < motorAngle)
		{}
	}

	motor[motorB] = 0;
}

/*
This is the drive function. It takes in the left and right
motor speeds seperately and sets the motor speed accordingly
*/
void drive(int leftSpeed, int rightSpeed)
{
	motor[motorD] = leftSpeed;
	motor[motorA] = rightSpeed;
}

/*
This is the follow line function. It follows
a white line
*/
void followLine()
{
	const int TURN_SPEED = 10;
	stop_then_proceed();
	if(getColorName(COLOR_PORT) == colorWhite)
	{
		drive(TURN_SPEED,2*TURN_SPEED);
	}
	else
	{
		drive(2*TURN_SPEED,TURN_SPEED);
	}
}

/*
This is the get ball function. It reorientates the
robot so that it aligns with the pick up ramp and grabs
the ball. Then it backs away and turns.
*/
void getBall()
{
		const int SPEED = 10;
    turnAngle(SPEED, 45);
    encoderDrive(-SPEED, -10);
    turnAngle(SPEED, -40);
    encoderDrive(SPEED, 21);
    motor[motorC] = -SPEED;
    time1[T1] = 0;
    while(time1[T1] < 1000)
    {}
    motor[motorC] = 0;
    encoderDrive(-SPEED, -20);
    turnAngle(SPEED, 175);
}

/*
This function is the turn function.
It turns the robot for a specific angle.
It takes in the motor power and angle.
*/
void turnAngle(int motorPower, int angle)
{
    if(angle > 0)
    {
        resetGyro(GYRO_PORT);
        drive(motorPower, -1 * motorPower);
        while(getGyroDegrees(GYRO_PORT) < angle)
        {}
        drive(0,0);
    }
    else
    {
        resetGyro(GYRO_PORT);
        drive(-1 * motorPower, motorPower);
        while(getGyroDegrees(GYRO_PORT) > angle)
        {}
        drive(0,0);
    }
}

/*
This function is the motor encoder function.
It drives the robot for a specific distance.
It takes in the motor power and distance.
*/
void encoderDrive(int motorPower, int distance)
{
    nMotorEncoder[motorA] = 0;
    drive(motorPower, motorPower);
    int encLimit = distance * 180 / (PI*2.75);
    if(distance > 0)
    {
    	while(nMotorEncoder[motorA] < encLimit)
    	{}
    }
    else
    {
    	while(nMotorEncoder[motorA] > encLimit)
    	{}
    }

    drive(0,0);
}

/*
This function reads in the color from the colour sensor
and returns it
*/
int get_colour()
{
		int colour = 0;
    if(getColorName(S4) == colorRed)
        colour = 1;

    else if(getColorName(S4) == colorYellow)
        colour = 3;

    else if(getColorName(S4) == colorNone)
    		colour = 0;
    else
    	colour = 2;
   return colour;
}

/*
This function stops the robot all long as there is
an obstacle in the way
*/
void stop_then_proceed()
{
	while(getUSDistance(S3) < 30)
	{
		drive(0,0);
	}
}

/*
Drop off function, this is called when the robot reaches
the drop off zone which is the red tape area.
This function takes in the color of the ball to figure out
which bin to drop the ball off in
*/
void dropoff(int colour)
{
	const int SPEED = 15;
	const int STOP_SPEED = 0;
	int angle = 0;
	encoderDrive(SPEED, 5);
	if(colour == 2)
		angle = 85;
	else if(colour == 1)
		angle = -85;

	turnAngle(SPEED, angle);

	while(getColorName(S4) != colorRed)
	{
		drive(SPEED,SPEED);
	}
	encoderDrive(SPEED, 5);
	turnAngle(SPEED,-angle);
	while(getUSDistance(S3) > 40)
	{
		drive(SPEED,SPEED);
	}
	drive(STOP_SPEED,STOP_SPEED);
	motor[motorC] = 10;
  time1[T1] = 0;
  while(time1[T1] < 180)
  {}
  motor[motorC] = STOP_SPEED;
  wait1Msec(500);

  encoderDrive(-SPEED, -10);
  turnAngle(SPEED, 175);

	if(colour != 3)
	{
		while(getColorName(S4) != colorRed)
		{
			drive(SPEED,SPEED);
		}
		encoderDrive(SPEED, 10);
		turnAngle(SPEED, angle);
		while(getColorName(S4) != colorRed)
		{
			drive(SPEED,SPEED);
		}
		encoderDrive(SPEED, SPEED);
		turnAngle(SPEED, -angle);
	}
	else
	{
		while(getColorName(S4) != colorRed)
		{
			drive(SPEED,SPEED);
		}
	}
}
