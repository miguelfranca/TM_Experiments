#ifndef _VEC_
#define _VEC_

#include <vector>
#include <string>
#include <cmath>

#include <cstdarg>

template <class T>
class Vec{
public:
	Vec();
	Vec(const unsigned,const T& value=0);
	Vec(const unsigned, const T*);//can't have constant, not to conflict with the constructor above
	Vec(const std::vector<T>&);
	Vec(const Vec&);//copy constructor
	//example: Vec v("1 2.2 3") or "1,2.2,3" or "1asd2.2 p!?3" are all accepted -> only for number type classes
	Vec(const char*);
	Vec(const std::string&);

	//HAS TO BE HERE DEFINED, TO AVOID COMPILER ISSUES
	//(when executing the main, he was selecting the specific templates needed and since they are not instanciated explicitely, the compiler would throw an 'undefined reference')
    template< typename ...T2>
    Vec(const int n, const T x, T2...rest){setEntries(n,x,rest...);}
	~Vec();

	template <class T2> operator Vec<T2>() const;

	void clear();

	void setEntries(int n, const T& value=0);//used also for last argument of template function of stdarg with undefined argument (also why 'int n' can't be const)
	void setEntries(const unsigned, const T*);//can't have constant, not to conflict with the constructor above
	void setEntries(const char*);
	void setEntries(const std::string&);
	// template <class T2> void setEntries2(const unsigned, const T2& init=0);//used to initialize T as T2 as T(T2) -> useful for Matrix

    template< typename ...T2>
	void setEntries(const int n, const T x, T2...rest);

	void resize (const unsigned, const T& value=T());
	void push_back(const T&);
	T& back()				{return get(-1);}
	const T& back() const 	{return get(-1);}

	T& operator[](const unsigned);//returns the element itself, giving access to it
	const T& operator[](const unsigned) const;
	T& get(int);//return the element itself, allowing for negative indexes as "counting from end"
	const T& get(int) const;

	Vec  operator- () const;//symmetric
	Vec  operator+ () const;
	Vec& operator= (const Vec&);
	Vec& operator= (const std::vector<T>&);
	Vec& operator= (const char*);
	Vec& operator= (const std::string&);

	Vec& operator+=(const Vec&);
	Vec& operator+=(const T&);
	Vec  operator+ (const Vec&) const;
	Vec  operator+ (const T&) const;
	template <class T2> friend Vec<T2> operator+ (const T2&, const Vec<T2>&);
	template <class T2> friend Vec<T2>& operator+=(const T2&, Vec<T2>&);

	Vec& operator-=(const Vec&);
	Vec& operator-=(const T&);
	Vec  operator- (const Vec&) const;
	Vec  operator- (const T&) const;
	template <class T2> friend Vec<T2> operator- (const T2&, const Vec<T2>&);
	template <class T2> friend Vec<T2>& operator-=(const T2&, Vec<T2>&);

	Vec& operator*=(const Vec&);
	Vec& operator*=(const T&);
	Vec  operator* (const Vec&) const;
	Vec  operator* (const T&) const;
	template <class T2> friend Vec<T2> operator* (const T2&, const Vec<T2>&);
	template <class T2> friend Vec<T2>& operator*=(const T2&, Vec<T2>&);

	Vec& operator/=(const Vec&);
	Vec& operator/=(const T&);
	Vec  operator/ (const Vec&) const;
	Vec  operator/ (const T&) const;
	template <class T2> friend Vec<T2> operator/ (const T2&, const Vec<T2>&);
	template <class T2> friend Vec<T2>& operator/=(const T2&, Vec<T2>&);

	//Note: Nobe careful because operations like + or - have precedence over ^
	// Vec& operator^(const T power) {*this=pow(*this,power);return *this;} //power of vector - could be used to replace the Vec itself when not defined as const
	Vec operator^(const T& power) const{return pow(*this,power);} //power of vector
	Vec operator^(const Vec<T> powers) const{return pow(*this,powers);} //power of vector

	//comparisons
	Vec operator>(const Vec&) const;
	Vec operator>=(const Vec&) const;
	Vec operator<(const Vec&) const;
	Vec operator<=(const Vec&) const;
	bool operator!=(const Vec&) const;
	bool operator==(const Vec&) const;

	//concatenate vectors
	Vec operator&(const Vec&) const;
	Vec join(const Vec& v){return (*this)&v;}
	friend Vec join(const Vec& v1, const Vec& v2){return v1&v2;}

	Vec mapV(T (*f)(T)) const;
	template <class T2> friend Vec<T2> mapV(T2 (*f)(T2), const Vec<T2>& v);
	template <class T2> friend Vec<T2> mapV2(T2 (*f)(T2,T2), const Vec<T2>& v1, const Vec<T2>& v2);

	//math functions (example: sqrt(v) returns a vector with the square roots of all the elements)
	friend Vec sqrt (const Vec& v){return v.mapV(sqrt);}
	friend Vec fabs (const Vec& v){return v.mapV(fabs);}
	friend Vec abs  (const Vec& v){return fabs(v);}
	friend Vec cos  (const Vec& v){return v.mapV(cos);}
	friend Vec cosh (const Vec& v){return v.mapV(cosh);}
	friend Vec acos (const Vec& v){return v.mapV(acos);}
	friend Vec sin  (const Vec& v){return v.mapV(sin);}
	friend Vec sinh (const Vec& v){return v.mapV(sinh);}
	friend Vec asin (const Vec& v){return v.mapV(asin);}
	friend Vec tan  (const Vec& v){return v.mapV(tan);}
	friend Vec tanh (const Vec& v){return v.mapV(tanh);}
	friend Vec atan (const Vec& v){return v.mapV(atan);}
	friend Vec exp  (const Vec& v){return v.mapV(exp);}
	friend Vec log  (const Vec& v){return v.mapV(log);}
	friend Vec log10(const Vec& v){return v.mapV(log10);}

	//mapV(T,T) not working here for some reason
	// template <class T2> friend Vec<T2> atan2(const Vec<T2>&, const Vec<T2>&);
	friend Vec atan2(const Vec& v1, const Vec& v2)	{return mapV2(atan2,v1,v2);}
	friend Vec pow(const Vec& v, const T& power=1)	{return mapV2(pow,v,Vec(v.size(),power));}
	friend Vec pow(const Vec& v1, const Vec& v2)	{return mapV2(pow,v1,v2);}
	friend Vec pow(const T& base, const Vec& v)		{return mapV2(pow,Vec(v.size(),base),v);}
	
	template <class T2> friend Vec<T2> max  (const Vec<T2>&, const Vec<T2>&);
	template <class T2> friend Vec<T2> min  (const Vec<T2>&, const Vec<T2>&);

	unsigned size() const 		{return N;}
	unsigned capacity() const 	{return M;}
	T dot(const Vec&) const;//dot or internal product
	friend T dot(const Vec& a, const Vec& b){return a.dot(b);}

	//PRITING - print() or print(text) prints normal text in the command line (it goes to the "file print" function first, but leaves after detecting no '.txt')
	void print(const std::string title, const unsigned precision) const; //prints string 'name' with 'precision' decimal places
	friend std::ostream& operator<<(std::ostream& os, const Vec& v){v.print();return os;} //cout overloaded (example: std::cout << v1 << v2 << std::endl;)
	//print to file
	//mode - change to "a", "app" or "append" to append instead of rewriting
	//'separator' between numbers
	//warning for prints
	//reject if some number should be excluded (for example, 0's may be excluded if wanted)
	int print(const std::string name="", const std::string title="", const std::string mode="", const unsigned precision=4, const std::string separator=" ", const int warning =0, const T& reject=1.e8) const;

	Vec& swap(const unsigned,const unsigned);//swap indexes
	T* data() {return entries;}//access to entries
	const T* data() const {return entries;}

	Vec reduce(int start=0, int end=-1) const; //vector with the elements of the vector between indexes 'start' and 'end'
	Vec& remove(int start=0, int end=-1);	//remove index 'index'
	Vec& removeV(const T&);	//remove value
	Vec& erase(int start=0, int end=-1){return remove(start,end);}
	Vec& pop(){return remove(-1);}

	//negative indexes correspond to counting from the back and placing AFTERWARDS instead of AT index
	Vec& insert(const T& x, int index=-1, unsigned places=1){return insert(Vec(places,x),index);} //insert at index 'index', 'places' new numbers with value 'x'
	Vec& insert(const Vec& v, int index=-1); //insert Vec 'v' at index 'index'

	int find(const T& value, unsigned Nocorr=1) const; //finds the index that has a value of 'value' (the 'Nocorr'th occurence) -1 if it doesn't exist
	unsigned findMax(int start=0, int end=-1) const; //index of the maximum value
	friend unsigned findMax(const Vec& v, int start=0, int end=-1){return v.findMax(start,end);}
	T max(int start=0, int end=-1) const{return entries[findMax(start,end)];} //maximum value
	friend T max(const Vec& v, int start=0, int end=-1){return v.max(start,end);}
	unsigned findMin(int start=0, int end=-1) const;
	friend unsigned findMin(const Vec& v, int start=0, int end=-1){return v.findMin(start,end);}
	T min(int start=0, int end=-1) const{return entries[findMin(start,end)];}
	friend T min(const Vec& v, int start=0, int end=-1){return v.min(start,end);}
	unsigned findZero(int start=0, int end=-1) const; //index of first TURNOVER point
	friend unsigned findZero(const Vec& v, int start=0, int end=-1){return v.findZero(start,end);}
	
	T norm(const unsigned type=2){return norm(*this,type);}
	template <class T2> friend T2 norm(const Vec<T2>& v, const unsigned type); //type=0 to infinity norm

	template <class T2> friend T2 averageDifference(const Vec<T2>& v1, const Vec<T2>& v2);	//Standard deviation of v1-v2

	//sort vector (descendent order if down==true)
	Vec sort(bool down=false) const;
	friend Vec sort(const Vec& v,bool down=false){return v.sort(down);}

	friend std::vector<T> toVector(const Vec& v){return std::vector<T>(v.entries,v.entries+v.N);}	//convert Vec to vector<T>

	Vec invert() const;	//invert order of vector


	Vec indices();
	Vec randomise(const T& a=0.,const T& b=1.);

	T sum() const;
	Vec shift(int s) const;
protected:
	unsigned N=0; //number of elements
	unsigned M=0; //capacity allocated in entries -> only important if push_back is used, since capacity doubles
	T *entries=NULL; // pointer to array of T's

private:
	void delEntries();
};

typedef Vec<double> VecD;

#include "Vec.impl.hpp"

#endif