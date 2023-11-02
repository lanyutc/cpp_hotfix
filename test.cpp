#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "test.h"
#include "loadso.h"

int TestClass::Incr(int incrVal)
{
	m_val += incrVal;
	std::cout << "After Incr Val:" << m_val << std::endl;
}

int TestClass::Decr(int decrVal)
{
	m_val += decrVal;
	std::cout << "After Decr Val:" << m_val << std::endl;
}

int main()
{
	signal(SIGUSR1, signalUserHander);

	while (1) {
		TestClass stTest(100);
		stTest.Incr(100);
		stTest.Decr(200);
		std::cout << "-----" << std::endl;
		sleep(1);
	}
	return 0;
}
