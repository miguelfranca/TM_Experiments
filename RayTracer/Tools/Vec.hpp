#ifndef _VEC_
#define _VEC_

#include <cmath>
#include <string>
#include <vector>

#include <cstdarg>

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// USEFUL MACROS
#define FORV1(IDX) FORV2(IDX, size())
#define FORV2(IDX, SZ) FORV3(IDX, 0, SZ)
#define FORV3(IDX, INIT, SZ) for (int IDX = INIT, MAX = SZ; IDX < MAX; ++IDX)

#define GET_MACRO_V(_1, _2, _3, NAME, ...) NAME
#define FORV(...) GET_MACRO_V(__VA_ARGS__, FORV3, FORV2, FORV1)(__VA_ARGS__)

#define FOR0(N) FOR1(i, N)
#define FOR1(IDX, DIM) FOR1_I(IDX, 0, DIM)
#define FOR1_I(IDX, INIT, DIM) for (int IDX = INIT; IDX < DIM; ++IDX)
#define FOR2(IDX1, IDX2, DIM1, DIM2) FOR2_I(IDX1, IDX2, 0, 0, DIM1, DIM2)
#define FOR2_2(IDX1, IDX2, DIM1, DIM2, AUX) XXXXXX // not defined
#define FOR2_I(IDX1, IDX2, INIT1, INIT2, DIM1, DIM2)                           \
    FOR1_I(IDX1, INIT1, DIM1) FOR1_I(IDX2, INIT2, DIM2)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define FOR(...)                                                               \
    GET_MACRO(__VA_ARGS__, FOR2_I, FOR2_2, FOR2, FOR1_I, FOR1, FOR0)           \
    (__VA_ARGS__)
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// there is still some redefinitions in initialization (since 'resize' is being
// used before setting values and 'resize' sets the vector the default T()) but
// it's because I found some problems in using reserve and push_back

template <class T> class Vec : public std::vector<T>
{
  public:
    inline Vec();
    inline Vec(const unsigned);
    inline Vec(const unsigned, const T &value);
    inline Vec(const unsigned,
               const T *); // can't have constant, not to
                           // conflict with the constructor above
    inline Vec(const std::vector<T> &);
    inline Vec(std::vector<T> &&);
    inline Vec(const Vec &); // copy constructor
    inline Vec(Vec &&);      // move constructor
    // example: Vec v("1 2.2 3") or "1,2.2,3" or "1asd2.2 p!?3" are all accepted
    // -> only for number type classes
    inline Vec(const char *);
    inline Vec(const std::string &);
    template <class T2> inline Vec(const Vec<T2> &);
    inline Vec(const std::initializer_list<T> list);

    // use updated constructor above (with initializer list {...}) instead.
    // Keeping the one below just for backward compatibility HAS TO BE HERE
    // DEFINED, TO AVOID COMPILER ISSUES (when executing the main, he was
    // selecting the specific templates needed and since they are not
    // instanciated explicitely, the compiler would throw an 'undefined
    // reference')
    template <typename... T2> inline Vec(const int n, const T x, T2... rest)
    {
        setEntries(n, x, rest...);
    }

    template <class T2> operator Vec<T2>() const;

    inline unsigned size() const { return this->std::vector<T>::size(); }
    inline unsigned capacity() const
    {
        return this->std::vector<T>::capacity();
    }
    inline void clear() { this->std::vector<T>::clear(); }
    inline T *data()
    {
        return this->std::vector<T>::data();
    } // access to entries
    inline const T *data() const { return this->std::vector<T>::data(); }

    void setEntries(
        int n,
        const T &value =
            0); // used also for last argument of template function of stdarg
                // with undefined argument (also why 'int n' can't be const)
    void setEntries(const unsigned,
                    const T *); // can't have constant, not to conflict with the
                                // constructor above
    void setEntries(const char *);
    inline void setEntries(const std::string &);
    inline void setEntries(const std::initializer_list<T> list);
    // template <class T2> void setEntries2(const unsigned, const T2&
    // init=0);//used to initialize T as T2 as T(T2) -> useful for Matrix

    template <typename... T2>
    void setEntries(const int n, const T x, T2... rest);

    // void resize (const unsigned, const T& value=T());
    // void push_back(const T&);
    // T& back()				{return get(-1);}
    // const T& back() const 	{return get(-1);}

    // T& operator[](const unsigned);//returns the element itself, giving access
    // to it const T& operator[](const unsigned) const;
    T &get(int); // return the element itself, allowing for negative indexes as
                 // "counting from end"
    const T &get(int) const;
    inline T &operator()(int);
    inline const T &operator()(int) const;

    Vec operator-() const &; // symmetric
    Vec &&operator-() &&;    // symmetric
    inline Vec operator+() const &;
    inline Vec &&operator+() &&;
    Vec &operator=(const Vec &);
    Vec &operator=(const Vec &&);
    Vec &operator=(const std::vector<T> &);
    Vec &operator=(const std::vector<T> &&);
    Vec &operator=(const char *);
    Vec &operator=(const std::string &);
    Vec &operator=(const std::initializer_list<T> list);

    Vec &operator+=(const Vec &);
    Vec &operator+=(const T &);
    Vec operator+(const Vec &) const &;
    Vec &&operator+(Vec &&) const &;
    Vec &&operator+(const Vec &) &&;
    Vec &&operator+(Vec &&) &&;
    Vec operator+(const T &) const &;
    Vec &&operator+(const T &) &&;
    template <class T2>
    inline friend Vec<T2> operator+(const T2 &, const Vec<T2> &);
    template <class T2>
    inline friend Vec<T2> &&operator+(const T2 &, Vec<T2> &&);
    template <class T2>
    inline friend Vec<T2> &operator+=(const T2 &, Vec<T2> &);

    Vec &operator-=(const Vec &);
    Vec &operator-=(const T &);
    Vec operator-(const Vec &) const &;
    Vec &&operator-(Vec &&) const &;
    Vec &&operator-(const Vec &) &&;
    Vec &&operator-(Vec &&) &&;
    inline Vec operator-(const T &) const &;
    inline Vec &&operator-(const T &) &&;
    template <class T2>
    inline friend Vec<T2> operator-(const T2 &, const Vec<T2> &);
    template <class T2>
    inline friend Vec<T2> &&operator-(const T2 &, Vec<T2> &&);
    template <class T2>
    inline friend Vec<T2> &operator-=(const T2 &, Vec<T2> &);

    Vec &operator*=(const Vec &);
    Vec &operator*=(const T &);
    Vec operator*(const Vec &)const &;
    Vec &&operator*(Vec &&)const &;
    Vec &&operator*(const Vec &)&&;
    Vec &&operator*(Vec &&)&&;
    Vec operator*(const T &)const &;
    Vec &&operator*(const T &)&&;
    template <class T2>
    inline friend Vec<T2> operator*(const T2 &, const Vec<T2> &);
    template <class T2>
    inline friend Vec<T2> &&operator*(const T2 &, Vec<T2> &&);
    template <class T2>
    inline friend Vec<T2> &operator*=(const T2 &, Vec<T2> &);

    Vec &operator/=(const Vec &);
    Vec &operator/=(const T &);
    Vec operator/(const Vec &) const &;
    Vec &&operator/(Vec &&) const &;
    Vec &&operator/(const Vec &) &&;
    Vec &&operator/(Vec &&) &&;
    Vec operator/(const T &) const &;
    Vec &&operator/(const T &) &&;
    template <class T2> friend Vec<T2> operator/(const T2 &, const Vec<T2> &);
    template <class T2> friend Vec<T2> &&operator/(const T2 &, Vec<T2> &&);
    template <class T2> friend Vec<T2> &operator/=(const T2 &, Vec<T2> &);

    // Note: Note careful because operations like + or - have precedence over ^
    inline Vec operator^(const T &power) const &
    {
        return pow(*this, power);
    } // power of vector
    inline Vec &&operator^(const T &power) &&
    {
        return std::move(pow((Vec &&)(*this), power));
    }
    inline Vec operator^(const Vec<T> &powers) const &
    {
        return pow(*this, powers);
    }
    inline Vec &&operator^(Vec<T> &&powers) const &
    {
        return std::move(pow(*this, (Vec &&)(powers)));
    }
    inline Vec &&operator^(const Vec<T> &powers) &&
    {
        return std::move(pow((Vec &&)(*this), powers));
    }
    inline Vec &&operator^(Vec<T> &&powers) &&
    {
        return std::move(pow((Vec &&)(*this), (Vec &&)(powers)));
    }

    // comparisons
    Vec operator>(const Vec &) const &;
    Vec &&operator>(Vec &&) const &;
    Vec &&operator>(const Vec &) &&;
    Vec &&operator>(Vec &&) &&;
    Vec operator>=(const Vec &) const &;
    Vec &&operator>=(Vec &&) const &;
    Vec &&operator>=(const Vec &) &&;
    Vec &&operator>=(Vec &&) &&;
    Vec operator<(const Vec &) const &;
    Vec &&operator<(Vec &&) const &;
    Vec &&operator<(const Vec &) &&;
    Vec &&operator<(Vec &&) &&;
    Vec operator<=(const Vec &) const &;
    Vec &&operator<=(Vec &&) const &;
    Vec &&operator<=(const Vec &) &&;
    Vec &&operator<=(Vec &&) &&;
    inline bool operator!=(const Vec &) const;
    bool operator==(const Vec &) const;

    // concatenate vectors
    inline Vec operator&(const Vec &)const &;
    inline Vec &&operator&(Vec &&)const &;
    inline Vec &&operator&(const Vec &)&&;
    inline Vec &&operator&(Vec &&)&&;
    inline Vec join(const Vec &v) const & { return (*this) & v; }
    inline Vec &&join(Vec &&v) const &
    {
        return std::move((*this) & ((Vec &&) v));
    }
    inline Vec &&join(const Vec &v) &&
    {
        return std::move(((Vec &&)(*this)) & v);
    }
    inline Vec &&join(Vec &&v) &&
    {
        return std::move(((Vec &&)(*this)) & ((Vec &&) v));
    }

    inline Vec mapV(T (*f)(T)) const &;
    inline Vec &&mapV(T (*f)(T)) &&;
    template <class T2>
    inline friend Vec<T2> mapV(T2 (*f)(T2), const Vec<T2> &v);
    template <class T2> inline friend Vec<T2> &&mapV(T2 (*f)(T2), Vec<T2> &&v);
    template <class T2> inline Vec<T2> mapV(T2 (*f)(T)) const;
    template <class T2, class T3>
    friend Vec<T2> mapV(T2 (*f)(T3), const Vec<T3> &v);
    template <class T2>
    friend Vec<T2> mapV2(T2 (*f)(T2, T2), const Vec<T2> &v1, const Vec<T2> &v2);
    template <class T2>
    friend Vec<T2> &&mapV2(T2 (*f)(T2, T2), Vec<T2> &&v1, const Vec<T2> &v2);
    template <class T2>
    friend Vec<T2> &&mapV2(T2 (*f)(T2, T2), const Vec<T2> &v1, Vec<T2> &&v2);
    template <class T2>
    friend Vec<T2> &&mapV2(T2 (*f)(T2, T2), Vec<T2> &&v1, Vec<T2> &&v2);

    // math functions (example: sqrt(v) returns a vector with the square roots
    // of all the elements)
    inline friend Vec sqrt(const Vec &v) { return v.mapV(sqrt); }
    inline friend Vec &&sqrt(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(sqrt));
    }
    inline friend Vec fabs(const Vec &v) { return v.mapV(fabs); }
    inline friend Vec &&fabs(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(fabs));
    }
    inline friend Vec abs(const Vec &v) { return fabs(v); }
    inline friend Vec &&abs(Vec &&v) { return std::move(fabs(((Vec &&) v))); }
    inline friend Vec cos(const Vec &v) { return v.mapV(cos); }
    inline friend Vec &&cos(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(cos));
    }
    inline friend Vec cosh(const Vec &v) { return v.mapV(cosh); }
    inline friend Vec &&cosh(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(cosh));
    }
    inline friend Vec acos(const Vec &v) { return v.mapV(acos); }
    inline friend Vec &&acos(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(acos));
    }
    inline friend Vec sin(const Vec &v) { return v.mapV(sin); }
    inline friend Vec &&sin(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(sin));
    }
    inline friend Vec sinh(const Vec &v) { return v.mapV(sinh); }
    inline friend Vec &&sinh(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(sinh));
    }
    inline friend Vec asin(const Vec &v) { return v.mapV(asin); }
    inline friend Vec &&asin(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(asin));
    }
    inline friend Vec tan(const Vec &v) { return v.mapV(tan); }
    inline friend Vec &&tan(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(tan));
    }
    inline friend Vec tanh(const Vec &v) { return v.mapV(tanh); }
    inline friend Vec &&tanh(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(tanh));
    }
    inline friend Vec atan(const Vec &v) { return v.mapV(atan); }
    inline friend Vec &&atan(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(atan));
    }
    inline friend Vec exp(const Vec &v) { return v.mapV(exp); }
    inline friend Vec &&exp(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(exp));
    }
    inline friend Vec log(const Vec &v) { return v.mapV(log); }
    inline friend Vec &&log(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(log));
    }
    inline friend Vec log10(const Vec &v) { return v.mapV(log10); }
    inline friend Vec &&log10(Vec &&v)
    {
        return std::move(((Vec &&) v).mapV(log10));
    }

    inline friend Vec atan2(const Vec &v1, const Vec &v2)
    {
        return mapV2(atan2, v1, v2);
    }
    inline friend Vec &&atan2(Vec &&v1, const Vec &v2)
    {
        return std::move(mapV2(atan2, ((Vec &&) v1), v2));
    }
    inline friend Vec &&atan2(const Vec &v1, Vec &&v2)
    {
        return std::move(mapV2(atan2, v1, ((Vec &&) v2)));
    }
    inline friend Vec &&atan2(Vec &&v1, Vec &&v2)
    {
        return std::move(mapV2(atan2, ((Vec &&) v1), ((Vec &&) v2)));
    }

    inline friend Vec pow(const Vec &v, const T &power)
    {
        return mapV2(pow, v, Vec(v.size(), power));
    }
    inline friend Vec &&pow(Vec &&v, const T &power)
    {
        return std::move(mapV2(pow, ((Vec &&) v), Vec(v.size(), power)));
    }
    inline friend Vec pow(const Vec &v1, const Vec &v2)
    {
        return mapV2(pow, v1, v2);
    }
    inline friend Vec &&pow(Vec &&v1, const Vec &v2)
    {
        return std::move(mapV2(pow, ((Vec &&) v1), v2));
    }
    inline friend Vec &&pow(const Vec &v1, Vec &&v2)
    {
        return std::move(mapV2(pow, v1, ((Vec &&) v2)));
    }
    inline friend Vec &&pow(Vec &&v1, Vec &&v2)
    {
        return std::move(mapV2(pow, ((Vec &&) v1), ((Vec &&) v2)));
    }
    inline friend Vec pow(const T &base, const Vec &v)
    {
        return mapV2(pow, Vec(v.size(), base), v);
    }
    inline friend Vec &&pow(const T &base, Vec &&v)
    {
        return std::move(mapV2(pow, Vec(v.size(), base), ((Vec &&) v)));
    }

    template <class T2> friend Vec<T2> max(const Vec<T2> &, const Vec<T2> &);
    template <class T2> friend Vec<T2> &&max(Vec<T2> &&, const Vec<T2> &);
    template <class T2> friend Vec<T2> &&max(const Vec<T2> &, Vec<T2> &&);
    template <class T2> friend Vec<T2> &&max(Vec<T2> &&, Vec<T2> &&);
    template <class T2> friend Vec<T2> min(const Vec<T2> &, const Vec<T2> &);
    template <class T2> friend Vec<T2> &&min(Vec<T2> &&, const Vec<T2> &);
    template <class T2> friend Vec<T2> &&min(const Vec<T2> &, Vec<T2> &&);
    template <class T2> friend Vec<T2> &&min(Vec<T2> &&, Vec<T2> &&);

    T dot(const Vec &) const; // dot or internal product
    inline friend T dot(const Vec &a, const Vec &b) { return a.dot(b); }

    // PRITING - print() or print(text) prints normal text in the command line
    // (it goes to the "file print" function first, but leaves after detecting
    // no
    // '.txt')
    std::string toString(const std::string title = "",
                         const unsigned precision = 3) const;
    void print(const std::string title, const unsigned precision)
        const; // prints string 'name' with 'precision' decimal places
    inline friend std::ostream &operator<<(std::ostream &os, const Vec &v)
    {
        v.print();
        return os;
    } // cout overloaded (example: std::cout << v1 << v2 << std::endl;)
    // print to file
    // mode - change to "a", "app" or "append" to append instead of rewriting
    //'separator' between numbers
    // warning for prints
    // reject if some number should be excluded (for example, 0's may be
    // excluded if wanted)
    int print(const std::string name = "", const std::string title = "",
              const std::string mode = "", const unsigned precision = 4,
              const std::string separator = " ", const int warning = 0) const;

    Vec &swap(const unsigned, const unsigned); // swap indexes

    Vec reduce(int start = 0,
               int end = -1) const &; // vector with the elements of the vector
                                      // between indexes 'start' and 'end'
    Vec &&reduce(int start = 0,
                 int end = -1) &&; // vector with the elements of the vector
                                   // between indexes 'start' and 'end'
    Vec &remove(int start = 0, int end = -1,
                bool force = true); // remove index 'index'
    Vec &removeV(const T &);        // remove value
    inline Vec &erase(int start = 0, int end = -1)
    {
        return remove(start, end, false);
    }
    Vec &pop() { return remove(-1); }

    // negative indexes correspond to counting from the back and placing
    // AFTERWARDS instead of AT index
    inline Vec &insert(const T &x, int index = -1, unsigned places = 1)
    {
        return insert(Vec(places, x), index);
    } // insert at index 'index', 'places' new numbers with value 'x'
    Vec &insert(const Vec &v, int index = -1); // insert Vec 'v' at index
                                               // 'index'

    int find(const T &value, unsigned Nocorr = 1)
        const; // finds the index that has a value of 'value' (the 'Nocorr'th
               // occurence) -1 if it doesn't exist
    unsigned findMax(int start = 0,
                     int end = -1) const; // index of the maximum value
    inline friend unsigned findMax(const Vec &v, int start = 0, int end = -1)
    {
        return v.findMax(start, end);
    }
    T max(int start = 0, int end = -1) const
    {
        return (*this)[findMax(start, end)];
    } // maximum value
    inline friend T max(const Vec &v, int start = 0, int end = -1)
    {
        return v.max(start, end);
    }
    unsigned findMin(int start = 0, int end = -1) const;
    inline friend unsigned findMin(const Vec &v, int start = 0, int end = -1)
    {
        return v.findMin(start, end);
    }
    T min(int start = 0, int end = -1) const
    {
        return (*this)[findMin(start, end)];
    }
    inline friend T min(const Vec &v, int start = 0, int end = -1)
    {
        return v.min(start, end);
    }
    unsigned findZero(int start = 0,
                      int end = -1) const; // index of first TURNOVER point
    inline friend unsigned findZero(const Vec &v, int start = 0, int end = -1)
    {
        return v.findZero(start, end);
    }

    T norm(const unsigned type = 2) const;
    template <class T2>
    inline friend T2 norm(const Vec<T2> &v,
                          const unsigned type); // type=0 to infinity norm

    template <class T2>
    friend T2
    averageDifference(const Vec<T2> &v1,
                      const Vec<T2> &v2); // Standard deviation of v1-v2

    // sort vector (descendent order if down==true)
    Vec sort(bool down = false) const &;
    Vec &&sort(bool down = false) &&;
    inline friend Vec sort(const Vec &v, bool down = false)
    {
        return v.sort(down);
    }
    inline friend Vec &&sort(Vec &&v, bool down = false)
    {
        return std::move(((Vec &&) v).sort(down));
    }

    // friend std::vector<T> toVector(const Vec& v){return
    // std::vector<T>(v.entries,v.entries+v.N);}	//convert Vec to
    // vector<T>

    inline Vec invert() const &; // invert order of vector
    inline Vec &&invert() &&;    // invert order of vector

    inline Vec &indices();
    Vec &randomise(const T &a = 0., const T &b = 1.);

    inline T sum() const;
    Vec shift(int s) const &;
    Vec &&shift(int s) &&;
};

typedef Vec<double> VecD;

#include "Vec.impl.hpp"

#endif