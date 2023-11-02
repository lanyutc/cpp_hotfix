#!/bin/sh

g++ -o test test.cpp loadso.cpp -ldl -rdynamic -g

g++ -fPIC -shared -o libhotfix.so hotfix.cpp
