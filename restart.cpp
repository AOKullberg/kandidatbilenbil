#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

void start()
{
	system("/home/pi/Desktop/Bilen_Original/sensor");
}

int main()
{
	this_thread::sleep_for(chrono::milliseconds(2000));
	thread t(start);
	t.detach();
	return 0;
}
