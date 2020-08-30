#if !defined(_VEC_)
#error "This file should only be included through Vec.hpp"
#endif

#ifndef _VEC_IMPL_
#define _VEC_IMPL_

#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <cstdio>
#include <algorithm> //sort

#include <iomanip>

#include <ctime>

template <class T>
Vec<T>::Vec(): N(0), M(0), entries(NULL) {}

template <class T>
Vec<T>::Vec(const unsigned n, const T& value){setEntries(n,value);}
template <class T>
Vec<T>::Vec(const unsigned n, const T* vect): N(n), M(n) {
	entries = new T[M];
	for(unsigned i=0; i<N; i++)
		entries[i] = vect[i];
}
template <class T>
Vec<T>::Vec(const std::vector<T>& var): N(var.size()), M(var.size()){
	entries = new T[M];
	for(unsigned i=0; i<N; i++)
		entries[i] = var[i];
}
template <class T>
Vec<T>::Vec(const Vec& vec): N(vec.N), M(vec.M) {
	entries = new T[M];		
	for(unsigned i=0; i<N; i++)
		entries[i] = vec.entries[i];
}
template <class T>
Vec<T>::Vec(const char* str): N(0), entries(NULL){setEntries(str);}
template <class T>
Vec<T>::Vec(const std::string& str): N(0), entries(NULL){setEntries(str);}

template <class T>
Vec<T>::~Vec(){
	delEntries();
}

template <class T>
template <class T2>
Vec<T>::operator Vec<T2>() const{
	Vec<T2> newv(M);
	for(unsigned i=0; i<N; i++)
		newv[i]=(T2)entries[i];
	return newv;
}


template <class T>
void Vec<T>::clear(){
	N=0;
	M=0;
	delEntries();
}

template <class T>
void Vec<T>::delEntries(){
	if(entries != NULL){ //added this
		delete[] entries;
		entries = NULL;
	}
}


//used also for last argument of template function of stdarg with undefined argument
template <class T>
void Vec<T>::setEntries(int n, const T& value){
	//case of stdarg entry
	if(entries!=NULL && N!=0 && n==-1){
		while(n<0) n+=N;
		entries[n]=value;
		return;
	}
	
	//normal Constructor
	if ((int)N != n){
		clear();
		N = n;
		M = N;
		entries = new T[M];
	}
	for(unsigned i=0; i<N; i++)
		entries[i] = value;
}

template <class T>
void Vec<T>::setEntries(const unsigned n, const T* line){
	if(line==NULL){//coloca a NULL se receber NULL
		clear();
		return;
	}
	if (N != n){
		clear();
		N = n;
		M = N;
		entries = new T[M];
	}
	for(unsigned i=0; i<N; i++)
		entries[i] = line[i];
}

template <class T>
void Vec<T>::setEntries(const char* str){
	//first count how many numbers the string contains, and then saves it 'entries'

	std::string s(str);
	//detect if string is file name
	if(s.size()>=4 && s.substr(s.size()-4,4)==".txt"){
		//save the whole file in string 's'
		std::ifstream ifs (str);
		if(!ifs.is_open()){
			printf("Couldn't open file");
			*this = Vec<T>();
			return;
		}
		getline(ifs, s, (char) ifs.eof());
	}
	//important!
	N=0;
	M=0;

	if(s.substr(0,2)=="//"){
		clear();
		entries = new T[0];
		return;
	}

	T temp;
	char c;
	std::stringstream stream(s.c_str());	

	while(stream.get(c))
	{
		//isolated '.' will count as 0
		if((c>='0' && c<='9') || c == '.' || c== '-'){
			stream.putback(c);	//place the 'cursor' one character back
			stream>>temp;		//reads a full number from the stringstream
			N++;				//this counts as passing through one more number, and the stringstreams follows after that number
			M++;
		}
	}

	delEntries();
	entries = new T[M];

	stream.clear();
	stream.seekg(0,stream.beg);

	unsigned i=0;
	while(stream.get(c))
	{
		if((c>='0' && c<='9') || c == '.' || c == '-'){
			stream.putback(c);		//puts back the character read and then saves the number in the stream to entries
			stream>>entries[i++];
		}
	}
}
template <class T>
void Vec<T>::setEntries(const std::string& str){setEntries(str.c_str());}


template <class T>
template< typename ...T2>
void Vec<T>::setEntries(const int n, const T x, T2...rest){
	if(N==0){
		N=n;
		M=N;
		entries = new T[M];
	}
	entries[N-n] = x;
	//defined recursively
	n>2 ? setEntries(n-1,rest...) : setEntries(-1, rest...);
}


// template <class T>
// template <class T2>
// void Vec<T>::setEntries2(const unsigned n, const T2& init){
// 	if (N != n){
// 		if(entries != NULL)
// 			delete[] entries;
// 		N = n;
// 		M = N;
// 		entries = new T[M];
// 	}
// 	for(unsigned i=0; i<N; i++)
// 		entries[i] = T(init);
// }


template <class T>
void Vec<T>::resize (const unsigned n, const T& value){
	if(n==M) return;

	T* temp = new T[n];

	for(unsigned i=0; i<std::min(n,M); i++)
		temp[i]=entries[i];
	for(unsigned i=std::min(n,M); i<n; i++)
		temp[i]=value;

	delEntries();
	M = n;
	if(N>M) N=M;

	entries = temp;
}
template <class T>
void Vec<T>::push_back(const T& v){
	if(N<M){
		entries[N]=v;
		N++;
	}
	else{
		resize(M==0 ? 1: 2*M);
		push_back(v);
	}
}

template <class T>
T& Vec<T>::operator[](const unsigned i){
	return entries[i];
}
template <class T>
const T& Vec<T>::operator[](const unsigned i) const{
	return entries[i];
}
template <class T>
T& Vec<T>::get(int i){
	while(i<0) i+=N;
	return entries[i];
}
template <class T>
const T& Vec<T>::get(int i) const{
	while(i<0) i+=N;
	return entries[i];
}


template <class T>
Vec<T> Vec<T>::operator-() const{
	Vec<T> newv(N,entries);
	for(unsigned i=0; i<N; i++)
		newv[i]=-newv[i];
	return newv;
}
//creates a copy!
template <class T>
Vec<T> Vec<T>::operator+() const{
	Vec<T> newv(N,entries);
	return newv;
}


//############# = 
template <class T>
Vec<T>&   Vec<T>::operator=(const Vec<T>& other){
	if(&other != this)
		setEntries(other.N, other.entries);
	return *this;
}
template <class T>
Vec<T>& 	Vec<T>::operator= (const std::vector<T>& other){
	setEntries(other.size(),other.data());
	return *this;
}
template <class T>
Vec<T>& 	Vec<T>::operator= (const char* s){
	setEntries(s);
	return *this;
}
template <class T>
Vec<T>& 	Vec<T>::operator= (const std::string& s){
	setEntries(s.c_str());
	return *this;
}

//############# +
//it could be defined less efficiently with the operators '=' and '+' above
template <class T>
Vec<T>&   Vec<T>::operator+=(const Vec<T>& other){
	if(N != other.N){
		std::cerr << "'+=' for vectors with diferent size (nothing done)" << std::endl;
		return *this;
	}
	for(unsigned i=0; i<N; i++)
		entries[i]+=other[i];
	return *this;
}
template <class T>
Vec<T>&    Vec<T>::operator+= (const T& k){
	for(unsigned i=0; i<N; i++)
		entries[i]+=k;
	return *this;
}
template <class T>
Vec<T>    Vec<T>::operator+ (const Vec<T>& other) const{
	Vec<T> newv;
	if(N != other.N){
		std::cerr << "'+' for vectors with different size (returned empty Vec)" << std::endl;
		return newv;
	}
	newv.setEntries(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]+=other[i];
	return newv;
}
template <class T>
Vec<T>    Vec<T>::operator+ (const T& k) const{
	Vec<T> newv(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]+=k;
	return newv;
}
template <class T>
Vec<T>    operator+ (const T& k, const Vec<T>& other)	{return other+k;}
template <class T>
Vec<T>&   operator+=(const T& k, Vec<T>& other)	{
	other+=k;
	return other;
}


//############# -
//it could be defined less efficiently with the operators '+=' and '-' above
template <class T>
Vec<T>&   Vec<T>::operator-=(const Vec<T>& other){
	if(N != other.N){
		std::cerr << "'-=' for vectors with diferent size (nothing done)" << std::endl;
		return *this;
	}
	for(unsigned i=0; i<N; i++)
		entries[i]-=other[i];
	return *this;
}
template <class T>
Vec<T>&    Vec<T>::operator-=(const T& k){
	*this += (-k);
	return *this;
}
template <class T>
Vec<T>    Vec<T>::operator- (const Vec<T>& other) const{
	Vec<T> newv;
	if(N != other.N){
		std::cerr << "'-' for vectors with different size (returned empty Vec)" << std::endl;
		return newv;
	}
	newv.setEntries(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]-=other[i];
	return newv;
}
template <class T>
Vec<T>    Vec<T>::operator- (const T& k)   const		{return (*this)+(-k);}
template <class T>
Vec<T>    operator- (const T& k, const Vec<T>& other)	{return k+(-other);}
template <class T>
Vec<T>&   operator-=(const T& k, Vec<T>& other)	{
	other=k-other;
	return other;
}

//############# *
template <class T>
Vec<T>& Vec<T>::operator*=(const Vec<T>& other){
	if(N != other.N){
		std::cerr << "'*=' for vectors with diferent size (nothing done)" << std::endl;
		return *this;
	}
	for(unsigned i=0; i<N; i++)
		entries[i]*=other[i];
	return *this;
}

template <class T>
Vec<T>& Vec<T>::operator*=(const T& k){
	for(unsigned i=0; i<N; i++)
		entries[i]*=k;
	return *this;
}
template <class T>
Vec<T>    Vec<T>::operator* (const Vec<T>& other) const{
	Vec<T> newv;
	if(N != other.N){
		std::cerr << "'*' for vectors with different size (returned empty Vec)" << std::endl;
		return newv;
	}
	newv.setEntries(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]*=other[i];
	return newv;
}
template <class T>
Vec<T>    Vec<T>::operator* (const T& k)   const{
	Vec<T> newv(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]*=k;
	return newv;
}
template <class T>
Vec<T>    operator* (const T& k, const Vec<T>& other){return other*k;}
template <class T>
Vec<T>&   operator*=(const T& k, Vec<T>& other){
	other*=k;
	return other;
}


//############# /
template <class T>
Vec<T>&    Vec<T>::operator/=(const Vec<T>& other){
	if(N != other.N){
		std::cerr << "'/=' for vectors with diferent size (nothing done)" << std::endl;
		return *this;
	}
	for(unsigned i=0; i<N; i++)
		entries[i]/=other[i];
	return *this;
}
//needed to redefine it instead of doing *(1./k) since for T=int it wouldn't work
template <class T>
Vec<T>& Vec<T>::operator/=(const T& k){
	for(unsigned i=0; i<N; i++)
		entries[i]/=k;
	return *this;
}
template <class T>
Vec<T>    Vec<T>::operator/ (const Vec<T>& other) const{
	Vec<T> newv;
	if(N != other.N){
		std::cerr << "'/' for vectors with different size (returned empty Vec)" << std::endl;
		return newv;
	}
	newv.setEntries(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]/=other[i];
	return newv;
}
//needed to redefine it instead of doing *(1./k) since for T=int it wouldn't work
template <class T>
Vec<T>    Vec<T>::operator/ (const T& k)   const{
	Vec<T> newv(N, entries);
	for(unsigned i=0; i<N; i++)
		newv.entries[i]/=k;
	return newv;
}
template <class T>
Vec<T>    operator/ (const T& k, const Vec<T>& other)	{
	Vec<T> newv(other);
	for(unsigned i=0; i<other.N; i++)
		newv.entries[i]=k/newv.entries[i];
	return newv;
}
template <class T>
Vec<T>&   operator/=(const T& k, Vec<T>& other)	{
	for(unsigned i=0; i<other.N; i++)
		other.entries[i]=k/other.entries[i];
	return other;
}


template <class T>
Vec<T> Vec<T>::operator>(const Vec<T>& other) const{
	if(N != other.N){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(N);
	for(unsigned i=0; i<N;i++)
		out[i] = entries[i]>other[i] ? 1 : 0;
	return out;
}
template <class T>
Vec<T> Vec<T>::operator>=(const Vec<T>& other) const{
	if(N != other.N){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(N);
	for(unsigned i=0; i<N;i++)
		out[i] = entries[i]>=other[i] ? 1 : 0;
	return out;
}
template <class T>
Vec<T> Vec<T>::operator<(const Vec<T>& other) const{
	if(N != other.N){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(N);
	for(unsigned i=0; i<N;i++)
		out[i] = entries[i]<other[i] ? 1 : 0;
	return out;
}
template <class T>
Vec<T> Vec<T>::operator<=(const Vec<T>& other) const{
	if(N != other.N){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(N);
	for(unsigned i=0; i<N;i++)
		out[i] = entries[i]<=other[i] ? 1 : 0;
	return out;
}
template <class T>
bool Vec<T>::operator!=(const Vec<T>& other) const{
	return !((*this)==other);
}
template <class T>
bool Vec<T>::operator==(const Vec<T>& other) const{
	if(N != other.N)
		return false;
	for(unsigned i=0; i<N;i++)
		if(entries[i]!=other[i]) return false;
	return true;
}


template <class T>
Vec<T> Vec<T>::operator&(const Vec<T>& v) const{
	Vec<T> newv(*this);
	return newv.insert(v,-1);
}


//###################

template <class T>
Vec<T> Vec<T>::mapV(T (*f)(T)) const{
	Vec<T> out(N);
	for(unsigned i=0; i<N; i++)
		out[i]=f(entries[i]);
	return out;
}
template <class T2> Vec<T2> mapV(T2 (*f)(T2), const Vec<T2>& v){return v.mapV(f);}

template <class T>
Vec<T> mapV2(T (*f)(T,T), const Vec<T>& v1, const Vec<T>& v2){
	if(v1.N != v2.N){
		std::cerr << "'map' for vectors with different size (returned empty Vec)" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(v1.size());
	for(unsigned i=0; i<v1.size(); i++)
		out[i]=f(v1[i],v2[i]);
	return out;
}


template <class T>
Vec<T> max  (const Vec<T>& v1, const Vec<T>& v2){
	if(v1.size() != v2.size()){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(v1.size());
	for(unsigned i=0; i<v1.size();i++)
		out[i] = v1[i]>v2[i] ? v1[i] : v2[i];
	return out;
}
template <class T>
Vec<T> min  (const Vec<T>& v1, const Vec<T>& v2){
	if(v1.size() != v2.size()){
		std::cerr << "Different size vectors\n" << std::endl;
		return Vec<T>();
	}
	Vec<T> out(v1.size());
	for(unsigned i=0; i<v1.size();i++)
		out[i] = v1[i]<v2[i] ? v1[i] : v2[i];
	return out;
}




template <class T>
T  Vec<T>::dot(const Vec<T>& other) const{
	T prod=0;

	if(N != other.N){
		std::cerr << "dot for vectors with different size (return 0)" << std::endl;
		return prod;
	}

	for (unsigned i=0; i < N; i++)
		prod+= entries[i]*other[i];
	return prod;
}
template <class T>
void Vec<T>::print(const std::string title, const unsigned precision) const{
	std::cout << "\t";
	if(title!="") std::cout << title << "\t";
	std::cout << std::setprecision(precision);
	for(unsigned i=0; i<N; i++){
		// std::cout.width(precision);
		std::cout << std::left <<  entries[i] << "\t" << std::flush;
	}
	std::cout << std::endl;
}
template <class T>
int Vec<T>::print(const std::string name, const std::string title, const std::string mode, const unsigned precision, const std::string separator, const int warning, const T& reject) const{
	if(name.size()>=4 && name.substr(name.size()-4,4)==".txt"){
		std::ofstream::openmode open = (mode=="append" || mode=="a" || mode=="app" ) ? std::ofstream::app : std::ofstream::out;
		std::ofstream file(name.c_str(),open);

		if(title!="") file << title;
		file << std::setprecision(precision);
		for(unsigned i=0; i<N-1;i++){
	  		if(entries[i]!=reject){
				// file.width(precision);
	  			file << entries[i] << separator;
	  		}
		}
		if(N>0){
			//file.width(precision);
			file << entries[N-1] << "\n";
		}
  		file.close();
  		if(warning) std::cout << "File " << name << " correctly written." << std::endl;
  		return 1;
	}
	if(!warning) print(name,precision);
	else	std::cout << "'" << name <<  "' is not a .txt file name" << std::endl;
	return 0;
}
//swap a and b indixes
template <class T>
Vec<T>& Vec<T>::swap(const unsigned a,const unsigned b){
	if(a>N-1 || b>N-1 || a==b){
		std::cerr << "error when changing positions " << a << " and " << b << std::endl;
		return *this;
	}
	T temp = entries[a];
	entries[a]=entries[b];
	entries[b]=temp;
	return *this;
}


template <class T>
Vec<T> Vec<T>::reduce(int start, int end) const{
	while(start<0) start+=N;
	while(end<0) end+=N;

	if(start >= (int)N) start=N-1;
	if(end >= (int)N) end=N-1;
	if(start>end){
		int temp = start;
		start = end;
		end = temp;
	}

	Vec<T> v(end-start+1,0.);
	for(int i=start; i<=end; i++)
		v[i-start] = entries[i];
	return v;
}

template <class T>
Vec<T>& Vec<T>::remove(int start, int end){
	while(start<0) start+=N; //or recursion "remove(start+N,end)"

	if(end<start || end >= (int)N) end=start;
	else if(start>end){
		int temp = start;
		start = end;
		end = temp;
	}

	for(unsigned i=start; i<N-(end-start+1); i++)
		entries[i]=entries[i+end-start+1];

	// resize(N-(end-start+1));
	resize(M-(end-start+1));

	return *this;
}

template <class T>
Vec<T>& Vec<T>::removeV(const T& v){
	int i;
	while((i=find(v))>=0)
		remove(i);

	return *this;
}


template <class T>
Vec<T>& Vec<T>::insert(const Vec<T>& v, int index){
	if(v.size()==0) return *this;
	if(index<0){
		while(index<0) index+=N;
		index++; //to place it after, as usual default is place before
	}
	if(index > (int)N) index=N;

	resize(N+v.size());//M changes to 'N+v.size()'
	N += v.size();

	for(unsigned i=N-1; i>=index+v.size(); i--)
		entries[i]=entries[i-v.size()];
	for(unsigned i=0; i<v.size(); i++)
		entries[i+index]=v[i];

	return *this;
}


template <class T>
int Vec<T>::find(const T& value, unsigned Nocorr) const{
	for(unsigned i=0; i<N; i++)
		if(entries[i]==value && 1==Nocorr--) return i;
	return -1;
}

template <class T>
unsigned Vec<T>::findMax(int start, int end) const{
	if(end<0 || end >= (int)N) end=N-1;
	if(start>end){
		int temp = start;
		start = end;
		end = temp;
	}
	if(start < 0) start = 0;

	T m=entries[start];
	int index=start;
	for(int i=start+1; i<=end; i++)
		if(entries[i]>m){
			m=entries[i];
			index=i;
		}
	return index;
}

template <class T>
unsigned Vec<T>::findMin(int start, int end) const{
	if(end<0 || end >= (int)N) end=N-1;
	if(start>end){
		int temp = start;
		start = end;
		end = temp;
	}
	if(start < 0) start = 0;

	T m=entries[start];
	int index=start;
	for(int i=start+1; i<=end; i++)
		if(entries[i]<m){
			m=entries[i];
			index=i;
		}
	return index;
}

template <class T>
unsigned Vec<T>::findZero(int start, int end) const{
	if(end<0 || end >= (int)N) end=N-1;
	if(start>end){
		int temp = start;
		start = end;
		end = temp;
	}
	if(start < 0) start = 0;

	unsigned n;
	int sign = entries[start] < 0 ? -1 : 1;
	for(n=start; (int)n<=end && entries[n]*sign > 0.; n++);
	return n;
}

//norm p: (sum(v[i]^p))^(1/p)
//norm 0 uses norm infinity = max(Vec)
template <class T>
T norm(const Vec<T>& v, const unsigned type){
	T res=0.;
	if(v.size()==0) return res;
	if(type==0){
		res=v[0];
		for(unsigned i=1; i<v.size(); i++){
			if(fabs(v[i])>fabs(res)) res = v[i];
		}
		return res;
	}
	else{
		for(unsigned i=0; i<v.size(); i++)
			res += pow(v[i],type);
	}
	return pow(res,1./type);
}

//Standard Deviation of v1-v2
template <class T>
T averageDifference(const Vec<T>& v1, const Vec<T>& v2){
	if(v1.size()!=v2.size())
		return 0.;

	T med = 0., desv = 0.;
	for(unsigned i=0; i<v1.size(); i++){
		med  += fabs(v1[i] - v2[i]);
		desv += (v1[i] - v2[i])*(v1[i] - v2[i]);
	}
	return sqrt((T)v1.size()*desv - med*med)/v1.size();
}


template <class T>
bool descend(T i, T j) { return (i>j);}//to use in std::sort for descendent order
template <class T>
Vec<T> Vec<T>::sort(bool down) const{
	std::vector<T> vv = toVector(*this);
	if(down)
		std::sort(vv.begin(),vv.begin()+N,descend<T>);
	else
		std::sort(vv.begin(),vv.begin()+N);
	return Vec<T>(vv);
}

template <class T>
Vec<T> Vec<T>::invert() const{
	Vec<T> out(N);
	for(unsigned i=0; i<N; i++){
		out[i]=entries[N-i-1];
	}
	return out;
}


template <class T>
Vec<T> Vec<T>::indices(){
	for(unsigned i=0; i<N; i++) entries[i]=T(i);
	return *this;
}

template <class T>
T Vec<T>::sum() const{
	T out=0;
	for(unsigned i=0; i<N; i++)
		out+=entries[i];
	return out;
}

template <class T>
Vec<T> Vec<T>::shift(int s) const{
	if(s<0)
		while(s<0) s+=N;
	else
		s=s%N;

	Vec<T> out(N);
	for(unsigned i=0; i<(unsigned)s; i++)
			out[i]=entries[N-s+i];
	for(unsigned i=s; i<N; i++)
			out[i]=entries[i-s];

	return out;
}

template <class T>
Vec<T> Vec<T>::randomise(const T& a, const T& b){
	srand(time(NULL));
	for(unsigned i=0; i<N; i++) entries[i]=T((double)rand()/RAND_MAX*(b-a)+a);
	return *this;
}

#endif