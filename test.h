#pragma once

class TestClass
{
public:
	TestClass(int initVal) : m_val(initVal) {}
	int Incr(int incrVal);
	int Decr(int decrVal);

private:
	int m_val;
};
