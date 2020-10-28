#include "pch.h"
#include "Calculator.h"

static const double M_PI = 3.141592653589793238462643383279502884197;
static const double M_E = 2.7182818284590452353602874713526624977572;

using namespace std;

void Calculator::PushNumber(double number)
{
	calcStack.push(number);
}

void Calculator::PopNumber()
{
	if(calcStack.size() > 0)
	{
		calcStack.pop();
	}
}

double Calculator:: SendPi(){calcStack.push(M_PI); return M_PI;};

double Calculator::SendE(){calcStack.push(M_E); return M_E;};

double Calculator::SendUnaryOp(double(*unaryOp)(double a))
{
	if (calcStack.size() < 1)
	{
		InvalidOperation e = {"Not enough arguments"};
		throw e;
	}

	double a = calcStack.top();
	calcStack.pop();

	try
	{
		calcStack.push(unaryOp(a));
		return calcStack.top();
	}
	catch (const InvalidOperation& e)
	{
		calcStack.push(a);
		throw e;
	}
}

double Calculator::SendBinaryOp(double(*binaryOp)(double a, double b))
{
	if (calcStack.size() < 2)
	{
		InvalidOperation e = {"Not enough arguments"};
		throw e;
	}

	double b = calcStack.top();
	calcStack.pop();
	
	double a = calcStack.top();
	calcStack.pop();

	try
	{
		calcStack.push(binaryOp(a, b));
		return calcStack.top();
	}
	catch (const InvalidOperation& e)
	{
		calcStack.push(a);
		calcStack.push(b);
		throw e;
	}
}

/*
 * Unary functions Start here
 */

double Calculator::SendNegate()
{
	return SendUnaryOp([](double a){return -a;});
}

double Calculator::SendInverse()
{
	return SendUnaryOp([](double a)
	{
		if (a == 0) 
		{
			InvalidOperation e = {"Divide by zero"};
			throw e;
		}

		return 1 / a;
	});	
}

double Calculator::SendSquare()
{
	return SendUnaryOp([](double a){return a * a;});
}

double Calculator::SendSqrt()
{
	return SendUnaryOp([](double a)
	{
		if (a < 0) 
		{
			InvalidOperation e = {"Square root of negative number"};
			throw e;
		}

		return sqrt(a);
	});
}

double Calculator::SendSine()
{
	return SendUnaryOp([](double a){return sin(a);});
}

double Calculator::SendCosine()
{
	return SendUnaryOp([](double a){return cos(a);});
}

double Calculator::SendTangent()
{
	return SendUnaryOp([](double a)
	{
		if (cos(a) == 0) 
		{
			InvalidOperation e = {"Undefined tangent"};
			throw e;
		}

		return tan(a);
	});
}

double Calculator::SendArcsine()
{
	return  SendUnaryOp([](double a)
	{
		if (-1 > a || a > 1) 
		{
			InvalidOperation e = {"Undefined arcsine"};
			throw e;
		}

		return asin(a);
	});
}

double Calculator::SendArccosine()
{
	return SendUnaryOp([](double a)
	{
		if (-1 > a || a > 1) 
		{
			InvalidOperation e = {"Undefined arccosine"};
			throw e;
		}

		return acos(a);
	});
}

double Calculator::SendArctangent()
{
	return SendUnaryOp([](double a){return atan(a);});
}

double Calculator::SendLog10()
{
	return SendUnaryOp([](double a)
	{
		if (a <= 0) 
		{
			InvalidOperation e = {"Undefined logarithm"};
			throw e;
		}

		return log10(a);
	});
}

double Calculator::SendLogE()
{
	return SendUnaryOp([](double a)
	{
		if (a <= 0) 
		{
			InvalidOperation e = {"Undefined logarithm"};
			throw e;
		}

		return log(a);
	});
}

double Calculator::SendExp()
{
	return SendUnaryOp([](double a){return exp(a);});
}

double Calculator::SendExp10()
{
	return SendUnaryOp([](double a){return pow(10, a);});
}

/*
 *  Binary operations Start here
 */

double Calculator::SendPlus()
{
	return SendBinaryOp([](double a, double b){return a + b;});
}

double Calculator::SendMinus()
{
	return SendBinaryOp([](double a, double b){return a - b;});
}

double Calculator::SendMult()
{
	return SendBinaryOp([](double a, double b){return a * b;});
}

double Calculator::SendDiv()
{
	return SendBinaryOp([](double a, double b)
	{
		if (b == 0) 
		{
			InvalidOperation e = {"Divide by zero"};
			throw e;
		}

		return a / b;
	});
}

double Calculator::SendPow()
{
	return SendBinaryOp([](double a, double b)
	{
		if(a == 0 && b == 0)
		{
			InvalidOperation e = {"Undefined operation"};
			throw e;
		}
		if(a < 0 && abs(b) < 1)
		{
			InvalidOperation e = {"Root of negative number"};
			throw e;
		}
		try
		{
			return pow(a, b);
		}
		catch (...)
		{
			InvalidOperation e = {"Invalid exponent"};
			throw e;
		}
	});
}

double Calculator::SendRoot()
{
	return SendBinaryOp([](double a, double b)
	{
		if(a == 0)
			return 0.0;

		if(a < 0)
		{
			InvalidOperation e = {"Root of negative number"};
			throw e;
		}
		try
		{			
			return pow(a, 1 / b);
		}
		catch (...)
		{
			InvalidOperation e = {"Invalid Root"};
			throw e;
		}
	});
}

std::stack<double> Calculator::calcStack;

