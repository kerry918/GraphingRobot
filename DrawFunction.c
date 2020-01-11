//This is for RobotC

//Sources
//https://overiq.com/c-programming-101/array-of-structures-in-c/
//https://www.geeksforgeeks.org/structures-c/


//Note that the paper is being restricted to 20cm by 20cm even though
//it is A size (22cm by 28cm) to account for any errors
//This also makes the calculations easier since it is 1:1 scale for the
//actual graph itself

//since the movement range for the pen is 8cm horizontally only,
//it is 0.4cm between each x value. The y range is adjusted to 8cm as well
//to make the graph has a proportional view

//Motor D is the motor that controls the movement of the arm
//Motor B is the motor to control the movement of the paper slider
//Motor C is the motor to control the pen rotation
//sensor 1 is the touch sensor
//sensor 2 is the color sensor

//The side that has pen holder is the front side of the robot
//the starting position of the robot: pen down
//the direction paper moves is x-axis, the direction pen moves is y-axis

#include "PC_FileIO.c"

//strucs
//xy coordinates
typedef struct{
	float x;
	float y;
}xyCoord;

//motor speed
typedef struct{
	tMotor port;
	int speed;
}motorInfo;

const int NUM_OF_COORDS=20;
const int MAX_ARM_COUNTS=8*(180/(PI*2.75)); //prevents arm from going off page
//the max_arm_counts is 9 since the maximimum horiozntal movement of the arm wheel is 9cm
const int MIN_ARM_COUNTS=0;

//Function Prototypes
void motorSpeed(motorInfo & m , float newSpeed);
void readFile(xyCoord* coords);
void pressAndReleaseButton();
void moveToOrigin(motorInfo&arm,motorInfo&paper,motorInfo&pen);
void drawCartesianPlane (motorInfo&arm, motorInfo&paper, motorInfo&pen);
void rotatePenUpAndDown (int upOrDown,motorInfo&pen);
void graph(xyCoord* coords,motorInfo&arm,motorInfo&paper,motorInfo&pen);
void moveToNextCoord(float numX,float numY,int yOld,motorInfo&paper,motorInfo&arm);
void driveToInfinity(float numX,float numY, int yOld, int xOld,motorInfo&paper,motorInfo&arm);
bool undefined(float num);
bool toInfinity(float num, int motorEncoderValue);
bool colorTest();
void end(int time,motorInfo&arm,motorInfo&paper,motorInfo&pen);

//Controls calling of other functions
task main() {
	//naming motors
	motorInfo pen;
	pen.port = motorC;
	pen.speed = 0;

	motorInfo paper;
	paper.port = motorB;
	paper.speed = 0;

	motorInfo arm;
	arm.port = motorD;
	arm.speed = 0;

	//sensor type and sensor mode
	SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);

	//Start a timer to see how long it takes to graph
	time1[T1]=0;

	//Variables
	xyCoord coords[NUM_OF_COORDS];

	//initialize variables
	for(int row=0; row<NUM_OF_COORDS;row++){
		coords[row].x=0;
		coords[row].y=0;
	}
	int count=0;
		while( colorTest() &&count < 1){
		//function call
		//starting robot
		readFile(coords);
		displayString(3,"Please press the down button to start the robot");
		pressAndReleaseButton();
		moveToOrigin(arm,paper,pen);
		drawCartesianPlane(arm,paper,pen);
		//actual graphing
		graph(coords,arm,paper,pen);
		count++;
	}

	//ending robot
	end(time1[T1],arm,paper,pen);
	rotatePenUpAndDown(2,pen); //rotate pen down as the ending position, get ready for the next grpah
}//end of main

//set robot speed
void motorSpeed (motorInfo & m , float newSpeed){
	m.speed = newSpeed;
	motor[m.port] = m.speed;
}

//Reads the data from the file
void readFile(xyCoord* coords){
	TFileHandle fin;
	int f = 0;
	int f2 = 0;
	//Check if file can open
	if(!openReadPC(fin,"roboCoords.txt")) {
		displayString(3,"Error with the file");
	}
	else{
		//store all coordinates in an array
		for(int numCoords=0;numCoords<NUM_OF_COORDS;numCoords++){
		readIntPC(fin,f);
		readIntPC(fin,f2);
			coords[numCoords].x=f;
			coords[numCoords].y=f2;
		}
	}
	closeFilePC(fin);
}

//Waits for user to push button and release
void pressAndReleaseButton(){
	while(!getButtonPress(buttonAny)){}
	while(getButtonPress(buttonAny)){}
	displayString(3,"");
}

//move the robot to the origin of the cartesian plane
void moveToOrigin(motorInfo&arm, motorInfo&paper, motorInfo&pen){
	//rotate pen up before moving the robot to the starting position
	rotatePenUpAndDown(1,pen);
	//move the robot arm to the RIGHT end
	nMotorEncoder[arm.port] = 0;
	wait1Msec(500);
	motorSpeed(arm ,50);
	while(nMotorEncoder[arm.port] < MAX_ARM_COUNTS){}
	motorSpeed(arm,0);
	wait1Msec(2000);
	//move the robot arm to the CENTER of the horizontal graphing range
	motorSpeed(arm,-30);
	while(nMotorEncoder[arm.port] > MAX_ARM_COUNTS/2){}

	motorSpeed(arm ,0);
}

//rotate pen holder up and down
void rotatePenUpAndDown (int upOrDown, motorInfo&pen){
	//starts a timer to record the time taken to rotate the pen up and down
	time1[T2] = 0;
	//when the motor power is set to be 5, it takes 5 seconds to rotate a full cycle
	//rotate pen up 90 degrees
	if (upOrDown == 1){
		motorSpeed(pen,5);
	}
	//rotate pen down
	else if (upOrDown == 2){
		motorSpeed(pen,-5);
	}
	while(time1[T2]<1250){}
	motorSpeed(pen,0);
	wait1Msec(1000);
}

//robot draws the cartesian plane
void drawCartesianPlane (motorInfo&arm, motorInfo&paper, motorInfo&pen){
	//rotate pen down when drawing cartesian plane
	rotatePenUpAndDown (2,pen);

	//variable
	const int ARM_COUNT = 4*(180/(PI*2.75)); //half of the horizontal movement range

	nMotorEncoder[arm.port] = 0;
	nMotorEncoder[paper.port] = 0;

	for(int count=0; count<2; count++){
		if(count==0){	//draw +y axis
			motorSpeed(arm,20);
			while(nMotorEncoder[arm.port] < ARM_COUNT){}
		}

		else if(count==1){	//draw -y axis
			motorSpeed(arm,-30);
			while(nMotorEncoder[arm.port] > 0) {} //back to the origin
			motorSpeed(arm,0);
			rotatePenUpAndDown(2,pen); //pen down
			motorSpeed(arm,-20);
			while(nMotorEncoder[arm.port] > -ARM_COUNT){}
		}
		motorSpeed(arm,0);
		rotatePenUpAndDown(1,pen); //pen up
		wait1Msec(5);
	}

	for(int count=0; count<2; count++){
		if(count==0){//draw +x axis
			motorSpeed(arm,30);
			while(nMotorEncoder[arm.port] < 0){} //back to the origin
			motorSpeed(arm,0);
			rotatePenUpAndDown(2,pen); //pen down
			motorSpeed(paper,20);
			while(nMotorEncoder[paper.port] < ARM_COUNT){}
		}
		else if(count==1){//draw -x axis
			motorSpeed(paper,-30);
			while(nMotorEncoder[paper.port] > 0){} //back to the origin
			motorSpeed(paper,0);
			rotatePenUpAndDown(2,pen); //pen down
			motorSpeed(paper,-20);
			while(nMotorEncoder[paper.port] > -ARM_COUNT){}
		}
		motorSpeed(paper,0);
		rotatePenUpAndDown(1,pen); //pen up
		wait1Msec(5);
	}
	motorSpeed(paper,30);
	while(nMotorEncoder[paper.port] < 0){} //back to the origin
	motorSpeed(paper,0);
	wait1Msec(5);
}

//Function to graph everything
void graph(xyCoord* coords,motorInfo&arm,motorInfo&paper,motorInfo&pen) {
	nMotorEncoder[arm.port]=0;

	//variables
	int yOld = 0;
	int xOld = 0;
	int startPoint = 11;
	int endPoint = NUM_OF_COORDS;

	rotatePenUpAndDown(2,pen); //pen down
	if(coords[0].x > 0){ //if log function
		startPoint = 0;
		endPoint = 10;
	}

	//draw the coordinates in +x

	for(int numCoords=startPoint;numCoords<=endPoint;numCoords++){
		yOld = coords[numCoords-1].y;
		xOld = coords[numCoords-1].x;
		if(!undefined(coords[numCoords].y)){
			moveToNextCoord(coords[numCoords].x,coords[numCoords].y,yOld,paper,arm);
			//stops the robot briefly to reset
			motorSpeed(arm ,0);
			motorSpeed(pen ,0);
			motorSpeed(paper ,0);
			wait1Msec(1000);
		}
		else if(toInfinity(coords[numCoords].y,nMotorEncoder[arm.port])){
			driveToInfinity(coords[numCoords].x,coords[numCoords].y,yOld,xOld,paper,arm);
			//exit the for loop when it goes to first coordinate that has y value out of range
			numCoords = NUM_OF_COORDS+1;
		}
	}


	if(coords[0].x<=0){ //log function will not be included
		//return to origin before drawing the coordnates in -x
		moveToOrigin(arm,paper,pen);

		rotatePenUpAndDown(2,pen); //pen down
		//draw the coordinates in -x
		for(int numCoords = 9;numCoords>=0;numCoords--){

			yOld = coords[numCoords+1].y;
			xOld = coords[numCoords+1].x;
			if(!undefined(coords[numCoords].y)){
				moveToNextCoord(coords[numCoords].x,coords[numCoords].y,yOld,paper,arm);
			motorSpeed(arm ,0);
			motorSpeed(pen ,0);
			motorSpeed(paper ,0);
			wait1Msec(1000);
			}

			else if(toInfinity(coords[numCoords].y,nMotorEncoder[arm.port])){
				driveToInfinity(coords[numCoords].x,coords[numCoords].y,yOld,xOld,paper,arm);
				//exit the for loop when it goes to first coordinate that has y value out of range
				numCoords = -1;
			}
		}
		rotatePenUpAndDown(1,pen); //pen up
	}
}

//Checks if a coordinate is viable in range
bool undefined(float num){
	return(num>10 || num < -10);
}

//Checks if the next coordinate goes to infinity
bool toInfinity(float num, int motorEncoderValue) {
	return(num>10 && motorEncoderValue < MAX_ARM_COUNTS ||
	num<0 && motorEncoderValue > MIN_ARM_COUNTS);
}

//drive to next coordinates
void moveToNextCoord(float numX, float numY,int yOld,motorInfo&paper,motorInfo&arm){

	// check what is the y-value for when x=0, and move to the position
	// rotate down the pen to start
	time1[T2] = 0;
	// Draw connection between two points
	while( time1[T2] < 200)
	{
		motorSpeed(paper, 5.18711*numX);
		motorSpeed(arm , ((2*((numY -yOld)) + 0.4701) /0.4762));
		wait1Msec(500);
	}
	// The distance for every x-value is 0.4 cm
	// The distance foe every y-value is (y-value new - y-value old)*0.4 cm
	// Speed for the arm and the paper should be uniform
}

//drive to infinity
void driveToInfinity(float numX, float numY, int yOld, int xOld,motorInfo&paper,motorInfo&arm){

	float m = 0, b = 0, xNew = 0, speedX = 0, speedY = 0;
	// Find a linear equation between two points
	// The out of range point and the point before it
	m = (numX - xOld)/(numY - yOld) ;
	b = numY - m*numX;

	// Find the corresponding x value when y = 8
	// Calculate the proper speed for the arm and the paper to move
	if (numY > 8)
	{
		xNew = (8-b)/m;
		speedX = (xNew - xOld)*0.4/0.2;
		speedY = ((8-yOld) * speedX) /(xNew - xOld);
	}

	// Find the corresponding x value when y = -8
	// Calculate the proper speed for the arm and the paper to move
	if (numY < -8)
	{
		xNew = (-8-b)/m;
		speedX = (xNew - xOld)*0.4/0.2;
		speedY = (fabs(-8-yOld) * speedX) /(xNew - xOld);
	}

	time1[T2] = 0;
	// Draw connection between two points
	while (time1[T2] < 200)
	{
		motorSpeed(arm , (speedY + 0.4701)/0.4762);
		motorSpeed(paper , (speedX + 0.4701)/0.4762);
	}
}

//check for proper paper loaded
bool colorTest (){
	return SensorValue[S1] == (int)colorWhite;
}

//display message when the graph is finished
void end(int time,motorInfo&arm,motorInfo&paper,motorInfo&pen) {
	motorSpeed(arm ,0);
	motorSpeed(pen ,0);
	motorSpeed(paper ,0);
	displayString(3, "Finished graphing");
	displayString(5,"The program ran for %d seconds",time);
	displayString(7,"Please press any button to end the program");
	pressAndReleaseButton();
}
