
#include "Clock.hpp"

#include <cstdio>

Clock::Clock(const std::string pstr,const state ps):init(chrono_clock()),T(0.),t(0.),str(pstr),s(ps), verbose(true){
	if(ps==start) Start();
}


double Clock::Start(){
	double temp=count();
	if(s==pause)
		T=temp-(t-T); //take into account when pause was made
	else
		T=temp;
	t=T;
	s=start;
	return 0;
}
double Clock::Lap(const std::string str2){
	if(s!=start) return t;
	double temp=count();
	double save = temp-t;
	if(verbose) printf("%s%s\t%lf %lf\n",str.c_str(),str2.c_str(),temp-T,temp-t);
	t=temp;
	return save;
}

double Clock::Stop(const std::string str2, const std::string str3){
	if(s==stop) return t;
	double temp= (s==pause) ? t : count();
	if(verbose) printf("%s%s%lf%s",str.c_str(),str2.c_str(),temp-T,str3.c_str());
	s=stop;
	return temp-T;
}

double Clock::Pause(const std::string str2, const std::string str3){
	if(s!=start) return t;
	double temp=count();
	if(verbose) printf("%s%s%lf%s",str.c_str(),str2.c_str(),temp-T,str3.c_str());
	t=temp;
	s=pause;
	return t-T;
}
double Clock::Restart(const std::string str2, const std::string str3){
	double temp=0.;
	if(s!=stop) temp=S(str2,str3);
	S(str2,str3);
	return temp;
}


double Clock::count(){
	//chrono_clock returns a very ugly time and using the line below is how to decode it. That's all that matters. Practicality :P 
	auto temp = chrono_clock();
	std::chrono::duration<double> elapsed = temp - init;
	return elapsed.count();
}