#include <iostream>
#include "test.h"

int TestClass::Decr(int decrVal) //Decr_hotfix
{
    m_val -= decrVal;
	std::cout << "After Decr Val:" << m_val << std::endl;
}

