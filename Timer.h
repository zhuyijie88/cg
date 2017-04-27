#ifndef _TIMER_
#define _TIMER_

#include <Windows.h>
#include <string.h>
#include <time.h>

class Timer
{
public:
	double costTime;

	Timer(){
		LARGE_INTEGER tmp;
		QueryPerformanceFrequency(&tmp);
		_freq = tmp.QuadPart;
		costTime = 0;
	}
	~Timer();

	void Start()            // ��ʼ��ʱ
	{
		QueryPerformanceCounter(&_begin);
	}

	int getTime(){
		int seconds = time((time_t*)NULL);
		return seconds;
	}

	void End()                // ������ʱ
	{
		QueryPerformanceCounter(&_end);
		costTime = (double)(1.0*(_end.QuadPart - _begin.QuadPart)/ _freq);
	}

	void Reset()            // ��ʱ��0
	{
		costTime = 0;
	}

private:
	int _freq;
	LARGE_INTEGER _begin;
	LARGE_INTEGER _end;
};

#endif