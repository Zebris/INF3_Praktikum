//============================================================================
// Name        : INF3_Praktikum.cpp
// Author      : Gruppe 2; Fichtner, Holzer, Hornig
// Version     : 0.01
//============================================================================

#include <string>
#include <iostream>

#include <cstdio>      /* printf, NULL */
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */

#include <unistd.h>


#include "TASK3.h"


using namespace std;


int main(){
	srand(time(nullptr));

    TASK3::demoTask3();

    return 0;
}
