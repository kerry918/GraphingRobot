/*
This progarm is created to accept basic functions and interpret them for a 
LEGO EV3 Robot to plot out on a graph. This robot will draw the curve
on either a whiteboard or a piece of paper.

Note:
There will be many times the word function and constant may be used
These terms do not refer to the programming side of things but rather
the mathematical terms

Sources:
http://www.cplusplus.com/doc/tutorial/exceptions/
http://www.cplusplus.com/doc/tutorial/arrays/
https://stackoverflow.com/questions/3473438/return-array-in-a-function
*/

//Libraries
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <fstream>

//Function prototypes
void getFunction();
int degreeInput();
int polynomialDegree(int *coefficients, int degree);
void processPolynomial(int *coefficients, int constant, int degree);
void processRational(int *coefficientsNumerator, int degreeNumerator,
					 int constantNumerator, int *coefficientsDenominator,
					 int degreeDenominator, int constantDenominator);
void trigonometric();
bool wantFunctionOrNah();
void trigFunctionOutput(int choice, int coefficient, int innerCoefficient, int constant);
void processLogarithmic();
void processExponential();
bool endOfProgram();

using namespace std;

//main, calls the other functions
int main()
{
	//Main Menu
	cout << "Welcome to the robot graphing calculator" << endl;
	cout << "This program accepts basic functions and graphs the robot" << endl;
	cout << "These functions will generally be in the form of y=af(kx)+c" << endl;

	getFunction();
	system("PAUSE");
	return 0;
}

void getFunction()
{

	int userInput = 0;
	do
	{
		//Continually checks for proper input
		do
		{
			cout << "Please pick an option" << endl;
			cout << "\t1. Polynomial function" << endl;
			cout << "\t2. Rational function" << endl;
			cout << "\t3. Trigonometric function" << endl;
			cout << "\t4. Logarithmic function" << endl;
			cout << "\t5. Exponential function" << endl;
			cin >> userInput;
		} while (userInput < 1 || userInput > 5);

		//checks the option the user entered
		if (userInput == 1)
		{
			//Find degree of the polynomial
			const int DEGREE = degreeInput();
			int coefficients[DEGREE];
			processPolynomial(coefficients, DEGREE, polynomialDegree(coefficients, DEGREE));
		}

		else if (userInput == 2)
		{
			//rational function is in the form of f(x)/g(x)
			//Find degree of f(x)
			cout << "Please enter the information for the numerator of the rational function" << endl;
			const int DEGREENUMERATOR = degreeInput();
			int coefficientsNumerator[DEGREENUMERATOR];
			int constantNumerator = polynomialDegree(coefficientsNumerator, DEGREENUMERATOR);

			//Find degree of g(x)
			cout << "Please enter the information for the denominator of the rational function" << endl;
			const int DEGREEDENOMINATOR = degreeInput();
			int coefficientsDenominator[DEGREEDENOMINATOR];
			int constantDenominator = polynomialDegree(coefficientsDenominator, DEGREEDENOMINATOR);

			processRational(coefficientsNumerator, DEGREENUMERATOR, constantNumerator,
							coefficientsDenominator, DEGREEDENOMINATOR, constantDenominator);
		}
		else if (userInput == 3)
		{
			trigonometric();
		}

		else if (userInput == 4)
		{
			processLogarithmic();
		}
		else if (userInput == 5)
		{
			processExponential();
		}
	} while (endOfProgram());
	cout << "Thank you for using the program" << endl;
	cout << "Remember to press any button on the robot for it to start graphing" << endl;
}

//Allows the user to choose a degree of their function(s)
int degreeInput()
{
	int degree = 0;
	cout << " What degree would you like your function to be?" << endl;
	do
	{
		cout << " Please enter a number between 1 and 5 inclusive?" << endl;
		cin >> degree;
	} while (degree < 0 || degree > 6);
	cout << " You have chosen to make a " << degree << "th degree function" << endl;
	return degree;
}

//Procceses the degree of the polynomial and shows the user their polynomial
//Returns the constant value, c , of the polynomial
int polynomialDegree(int *coefficients, int degree)
{
	int constant = 0;
	bool want = 0;
	//Loop makes sure the user is satisfied with their polynomial
	do
	{
		//Accepts each coefficient of the function and then the constant value c
		for (int countDegree = degree - 1; countDegree >= 0; countDegree--)
		{
			do {
			cout << "Please enter the coefficient for x^" << countDegree + 1 << endl;
			cout << "between -5 and 5 inclusive" << endl;
			cin >> coefficients[countDegree];
			}while(coefficients[countDegree] < -5 || coefficients[countDegree] > 5 );
		}
		cout << "Please enter the constant value c" << endl;
		cin >> constant;

		cout << "Your polynomial is ";

		//Prints out the function for the user to see
		for (int countDegree = degree - 1; countDegree >= 0; countDegree--)
		{
			if (countDegree != 0 && coefficients[countDegree] != 0)
				cout << coefficients[countDegree] << "x^" << countDegree + 1 << " + ";
			else if (countDegree == 0 && coefficients[countDegree] != 0)
				cout << coefficients[countDegree] << "x"
					 << " + ";
		}
		cout << constant << endl;

		//Makes sure the user likes the polynomial
	} while (wantFunctionOrNah());
	return constant;
}

//Outputs points of the polynomial to the file
void processPolynomial(int *coefficients, int degree, int constant)
{
	ofstream fout("roboCoords.txt");
	double x = -10;
	double y = 0;
	//Goes through 20 x values and finds their respective y value
	for (x = -10; x <= 10; x++)
	{
		//Resets the y value
		y = 0;
		//Gets the y value
		for (int poly = 0; poly < degree; poly++)
		{
			y += (coefficients[poly] * pow(x, poly + 1)); //ax^n
		}
		y += constant;				   //add the constant at the end
		fout << x << " " << y << endl; // write the coordinate to the the file for the robot
	}
	fout.close();
	cout << "The coordinates have been placed into the file 'roboCoords.txt' " << endl;
}

//Outputs points of the rational function to the file
void processRational(int *coefficientsNumerator, int degreeNumerator,
					 int constantNumerator, int *coefficientsDenominator,
					 int degreeDenominator, int constantDenominator)
{

	ofstream fout("roboCoords.txt");
	double x = -10;
	int yDenominator = 0;
	int yNumerator = 0;
	double y = 0;

	//Goes through 20 x values and finds their respective y value
	for (x = -10; x <= 10; x++)
	{
		//Resets the y value
		yDenominator = 0;
		yNumerator = 0;
		y = 0;

		//Gets the y value in the numerator of f(x)
		for (int poly = 0; poly < degreeNumerator; poly++)
		{
			yNumerator += (coefficientsNumerator[poly] * pow(x, poly + 1));
		}
		yNumerator += constantNumerator;

		//Gets the y value in the denominator of g(x)
		for (int poly = 0; poly < degreeDenominator; poly++)
		{
			yDenominator += (coefficientsDenominator[poly] * pow(x, poly + 1));
		}
		yDenominator += constantDenominator;

		//Gets the y value using f(x)/g(x)
		y = (double)yNumerator / yDenominator;

		fout << fixed << setprecision(4) << x << " " << y << endl; // write the coordinate to the the file for the robot
	}
	fout.close();
	cout << "The coordinates have been placed into the file 'roboCoords.txt' " << endl;
}

void trigonometric()
{
	int choice = 0, coefficient = 0, innerCoefficient = 0, constant = 0;

	do
	{
		do
		{ //continually checks to make sure the user enters proper input
			cout << "Please choose a trigonometric function" << endl;
			cout << "\t1. sin" << endl;
			cout << "\t2. cos" << endl;
			cout << "\t3. tan" << endl;
			cin >> choice;
		} while (choice < 1 || choice > 3);

		do {
		cout << "please enter the coefficent a for y=af(x)" << endl;
		cout << "between -5 and 5 inclusive" << endl;
		cin >> coefficient;
		}while(coefficient > 5 || coefficient < -5);
		
		
		cout << "Please enter the inner coefficient k for y=f(kx)" << endl;
		cin >> innerCoefficient;
		cout << "Please enter the constant c for y=f(x) + c " << endl;
		cin >> constant;
		if (choice == 1)
		{
			cout << "y=" << coefficient << "sin(" << innerCoefficient << "x) + " << constant << endl;
		}
		else if (choice == 2)
		{
			cout << "y=" << coefficient << "cos(" << innerCoefficient << "x) + " << constant << endl;
		}
		else
		{
			cout << "y=" << coefficient << "tan(" << innerCoefficient << "x) + " << constant << endl;
		}

	} while (wantFunctionOrNah());
	trigFunctionOutput(choice, coefficient, innerCoefficient, constant);
}

//Outputs points of trig function a file
void trigFunctionOutput(int choice, int coefficient, int innerCoefficient, int constant)
{
	ofstream fout("roboCoords.txt");
	for (double x = -10; x <= 10; x++)
	{
		fout << x << " ";
		if (choice == 1)
		{
			fout << coefficient * sin(innerCoefficient * x) + constant;
		}
		else if (choice == 2)
		{
			fout << coefficient * cos(innerCoefficient * x) + constant;
		}
		else
		{
			fout << coefficient * tan(innerCoefficient * x) + constant;
		}
		fout << endl;
	}
	fout.close();
	cout << "The coordinates have been placed into the file 'roboCoords.txt' " << endl;
}

//Procceess the logarithmic function
void processLogarithmic()
{
	int base = 0, coefficient = 0, innerCoefficient = 0, constant = 0;
	do
	{
		cout << "please enter the base of logarithmic function: " << endl;
		cin >> base;
		do {
		cout << "please enter the coefficent a for y=af(x)" << endl;
		cout << "between -5 and 5 inclusive" << endl;
		cin >> coefficient;
		}while(coefficient > 5 || coefficient < -5);
		cout << "please enter the inner coefficent k for y=f(kx)" << endl;
		cin >> innerCoefficient;
		cout << "Please enter the constant c for y=f(x) + c " << endl;;
		cin >> constant;

		//print the function
		cout << "y = " << coefficient << "log" << base << "("
			 << innerCoefficient << "x) + " << constant << endl;
	} while (wantFunctionOrNah());

	ofstream fout("roboCoords.txt");
	for (double x = -10; x <= 10; x++)
	{
		fout << x << " " << coefficient * (log(innerCoefficient * x) / log(base)) + constant << endl;
	}
	fout.close();
	cout << "The coordinates have been placed into the file 'roboCoords.txt' " << endl;
}

//Proccesses the exponential function and writes to the file
void processExponential()
{
	int base = 0, coefficient = 0, constant = 0;
	double x = -10;
	double y = 0;
	do
	{
		cout << "Please enter the base of the exponential function" << endl;
		cin >> base;
		do {
		cout << "Please enter the coefficient a for the fucntion  y=a(b)^x + c" << endl;
		cout << "between -5 and 5 inclusive" << endl;
		cin >> coefficient;
		}while(coefficient< -5 || coefficient> 5 );
		cout << "Please enter the constant c for y= (b)^ x + c" << endl;
		cin >> constant;

		cout << "The expoential function you have entered is " << coefficient << "(" << base << ") ^ ";
		cout << "x + " << constant << endl;
	} while (wantFunctionOrNah());

	ofstream fout("roboCoords.txt");
	for (x = -10; x <= 10; x++)
	{
		y = 0;
		fout << x << " " << coefficient * (pow(base, x)) + constant;
	}
	fout.close();
	cout << "The coordinates have been placed into the file 'roboCoords.txt' " << endl;
}

//Checks if the user wants the function or not
bool wantFunctionOrNah()
{
	int choice = 0;
	cout << "Are you sure you want this function?" << endl;
	cout << "Please pick an option" << endl;
	cout << "\t1. Yes" << endl;
	cout << "\t2. No" << endl;
	cin >> choice;
	return choice == 2;
}

//Checks if the user wants to end the program
bool endOfProgram()
{
	int choice = 0;
	cout << "Please upload the file to the robot" << endl;
	cout << "and press the button on the robot for it to start graphing" << endl;
	cout << "You may now either end the program and run the robot or" << endl;
	cout << "enter a new function for the robot which will overwrite the old one" << endl;
	cout << "Please pick an option" << endl;
	cout << "\t1.End program" << endl;
	cout << "\t2.Re-enter the function" << endl;
	cin >> choice;
	return choice == 2;
}
