//#include <cstdio>
//#include <cstdlib>
//#include <iostream>
//#include "DeviceFS.h"
//#include "SuperFS.h"
#include "Shell.h"

using namespace std;

int main()
{
  Shell * shell = new Shell();
  shell->doShell();
	system("PAUSE");
	return 0;
}