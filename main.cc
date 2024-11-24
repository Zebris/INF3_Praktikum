//============================================================================
// Name        : INF3_Prak.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <string>
#include <iostream>

#include <cstdio>      /* printf, NULL */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

#include <unistd.h>

#include "TASK3.H"


using namespace std;


int main(){
	srand(time(nullptr));

    TASK3::demoTask3();

    return 0;
}