#pragma once
#include <stack>

struct InvalidOperation {
	const char* issue;
};
class Calculator
{

	static double SendUnaryOp(double(*unaryOp)(double a));
	static double SendBinaryOp(double(*binaryOp)(double a, double b));

public:
	static std::stack<double> calcStack;

	static void PushNumber(double number);
	static void PopNumber();

	static double SendPi();
	static double SendE();

	static double SendNegate();
	static double SendInverse();
	static double SendSquare();
	static double SendSqrt();
	static double SendSine();
	static double SendCosine();
	static double SendTangent();
	static double SendArcsine();
	static double SendArccosine();
	static double SendArctangent();
	static double SendLog10();
	static double SendLogE();
	static double SendExp();
	static double SendExp10();
	
	static double SendPlus();
	static double SendMinus();
	static double SendMult();
	static double SendDiv();
	static double SendPow();
	static double SendRoot();
};