// By Tiago Franca
/*
Class to setup easy timing operators.
Main Usage:

	Clock C; //start clock
	
	(...) code
	
	double time = C.L(); //single lap & continue clock

	(...) code

	C();

Other useful functions: 
	sleep(double)		- sleep for a given number of seconds (only for current thread if multiple threads are used)
	stats([see below]) 	- repeats a given function execution N times and outputs statistics about it (mean time and standard deviation)
*/

#pragma once

#include <chrono>
#include <string>
#include <cmath> //sqrt
// #include <unistd.h> //usleep - being used before 'std::this_thread::sleep_for'
#include <thread> //std::this_thread::sleep_for

//high_resolution_clock
typedef std::chrono::time_point<std::chrono::high_resolution_clock> chrono_time;

class Clock {
public:
	enum state {start,stop,pause};
	
	Clock(const std::string pstr="",const state ps=start);
	double Start();															//continue clock
	double Lap(const std::string str2="");									//don't stop clock, just output and save current time
	double Stop(const std::string str2="", const std::string str3="");		//stop clock
	double Pause(const std::string str2="", const std::string str3="");		//pause clock
	double Restart(const std::string str2="", const std::string str3="");	//start clock is paused or stopped, reset if on start

	//Redefine Start/Stop, Lap, Pause, Restart for ease of use
	//assumes Start if s=pause
	inline double S(const std::string str2="", const std::string str3=""){//Does Start/Stop if on Stop/Start
		if(s==stop) 	return Start();
		else 			return Stop(str2,str3);
	};
	inline double L(const std::string str2="")								{return Lap	   (str2);	   }
	inline double P(const std::string str2="", const std::string str3="")	{return Pause  (str2,str3);}
	inline double R(const std::string str2="", const std::string str3="")	{return Restart(str2,str3);}

	inline double operator()(const std::string str2="", const std::string str3=""){return S(str2,str3);}

	inline double GetT(){return T;}
	inline double Gett(){return t;}
	inline state  GetState(){return s;}

	inline void setVerbose(bool v) { verbose = v; }

	//Output time since start of clock
	double count();

	//Repeats a given function execution N times and outputs statistics about it (mean time and standard deviation)
	//Receives the number of iteration 'N', the function to be executed 'func' and the arguments with which we want it to be executed, in a list following 'func'
	//e.g.: Clock::stats(100,Clock::sleep,0.1);
	//bias (due to calling Clock): ~50ns (0.05us)
	template<typename T_out, typename ... ARGS1, typename ... ARGS2>
	static void stats(unsigned N, T_out (*func)(ARGS1 ...), ARGS2 && ... args);
	template<typename T_out, typename ... ARGS1, typename ... ARGS2>
	static void statsV(unsigned N, T_out (*func)(ARGS1 ...), ARGS2 && ... args); //average and std dev. of output itself

	//sleep for a given number of seconds (only for current thread if multiple threads are used)
	static inline void sleep(double secs) { std::this_thread::sleep_for(std::chrono::microseconds((int)(secs*1.e6))); /*usleep(secs*1.e6);*/ }
private:
	inline chrono_time chrono_clock() { return std::chrono::high_resolution_clock::now(); }
	chrono_time init;

	double T,t; 		//T - time since last Start, t - time since last Lap/Pause (or Start if no Lap/Pause was made)
	std::string str; 	//title to be printed in every output
	state s;			//state of clock
	bool verbose;		//whether to print the time when functions like S() or L() are called
};

template<typename T_out, typename ... ARGS1, typename ... ARGS2>
void Clock::stats(unsigned N, T_out (*func)(ARGS1 ...), ARGS2 && ... args){
	if(N==0) return;
	double sum = 0., sum2 = 0.;
	// double sum3 = 0., sum4 = 0.;

	Clock C("",stop);
	C.setVerbose(false);

	//Calculate sum(t) and sum(t^2) to calculate mean and variance
	for(unsigned i=0; i<N; i++){
		C();
		func(args...);
		double t = C();
		sum += t;
		sum2 += t*t;
		// sum3 += t*t*t;
		// sum4 += t*t*t*t;
	}
	sum /= N; //mean
	if(N>1){
		sum2 = sqrt(sum2/(N-1.) - sum*sum*N/(N-1.)); //population/sample standard deviation

		// attempt to calculate standard deviation of standard deviation. Under development.
		// printf("%lf\n",sum4);
		// sum2 = sum2/N - sum*sum; //variance (not corrected)
		// sum4 = sum4/N - 4.*sum*sum3/N + 6.*sum*sum*sum2 - 3.*sum*sum*sum*sum; //4th moment experimental!!!!!!!!
		// printf("%.10lf %.10lf\n",sum4,sum2*sum2);
		// sum4 = sum2*(1./(4.*N)*(sum4/(sum2*sum2)-1.));// + 1./(2.*N*N) + 3./(64.*N*N)*(sum4/(sum2*sum2)-1.)*(sum4/(sum2*sum2)-1.)); //variance of sample standard deviation up to O(1/N^3)
		// printf("%lf\n",sum4);


		// sum2 = sqrt(sum2*N/(N-1.)); //population standard deviation
		// sum4 = sqrt(sum4); //standard deviation of sample standard deviation
	}

	printf("TIME\t = %lf (%u iterations)\n",sum*N,N);
	printf("AVERAGE\t = %lf\n",sum);
	// if(N>1)	printf("STD.DEV. = %lf +- %lf (%lf%% | %lf)\n",sum2,sum4,sum2/sum,sum4/sum2);
	if(N>1)	printf("STD.DEV. = %lf (%lf%%)\n",sum2,sum2/sum);
}

template<typename T_out, typename ... ARGS1, typename ... ARGS2>
void Clock::statsV(unsigned N, T_out (*func)(ARGS1 ...), ARGS2 && ... args){
	if(N==0) return;
	T_out sum = 0., sum2 = 0.;

	//Calculate sum(t) and sum(t^2) to calculate mean and variance
	for(unsigned i=0; i<N; i++){
		T_out t = func(args...);
		sum += t;
		sum2 += t*t;
	}
	sum /= N; //mean
	if(N>1)
		sum2 = sqrt(sum2/(N-1.) - sum*sum*N/(N-1.)); //population/sample standard deviation

	printf("AVERAGE\t = %lf\n",sum);
	if(N>1)	printf("STD.DEV. = %lf (%lf%%)\n",sum2,sum2/sum);
}
