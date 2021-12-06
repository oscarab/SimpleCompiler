#ifndef QUATERNION_H
#define QUATERNION_H

enum class Operator {
	JNZ, JG, JL, JGE, JLE, JE, JNE,
	ADD, MINUS, UMINUS, MULT, DIV
};

class Quaternion {
private:
	Operator operate;
	int arg1;
	int arg2;
	int result;

	bool constArg1;
	bool constArg2;
public:
	Quaternion(Operator, int, int, int);

	void setConst(bool, bool);
	void setResult(int);
};

#endif // !QUATERNION_H

