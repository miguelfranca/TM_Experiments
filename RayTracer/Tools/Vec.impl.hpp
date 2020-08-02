#if !defined(_VEC_)
#error "This file should only be included through Vec.hpp"
#endif

#ifndef _VEC_IMPL_
#define _VEC_IMPL_

#include <algorithm> //std::sort, std::transform
#include <cstdio>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>

#include <iomanip>

#include <ctime>

template <class T> Vec<T>::Vec() : std::vector<T>(0) {}

template <class T> inline Vec<T>::Vec(const unsigned n) : std::vector<T>(n) {}
template <class T>
inline Vec<T>::Vec(const unsigned n, const T &value) : std::vector<T>(n, value)
{
}
template <class T> inline Vec<T>::Vec(const unsigned n, const T *vect)
{
    this->resize(n);
    FORV(i)(*this)[i] = vect[i];
}
template <class T>
inline Vec<T>::Vec(const std::vector<T> &var) : std::vector<T>(var)
{
}
template <class T>
inline Vec<T>::Vec(std::vector<T> &&var) : std::vector<T>(var)
{
}
template <class T> inline Vec<T>::Vec(const Vec<T> &vec) : std::vector<T>(vec)
{
}
template <class T>
inline Vec<T>::Vec(Vec<T> &&vec) : std::vector<T>((std::vector<T> &&) vec)
{
}

template <class T> inline Vec<T>::Vec(const char *str) { setEntries(str); }
template <class T> inline Vec<T>::Vec(const std::string &str)
{
    setEntries(str);
}

template <class T> template <class T2> inline Vec<T>::Vec(const Vec<T2> &other)
{
    this->resize(other.size());
    FORV(i)(*this)[i] = (T)other[i];
}

template <class T>
inline Vec<T>::Vec(const std::initializer_list<T> list) : std::vector<T>(list)
{
}

template <class T> template <class T2> Vec<T>::operator Vec<T2>() const
{
    Vec<T2> newv(capacity());
    FORV(i) newv[i] = (T2)(*this)[i];
    return newv;
}

// used also for last argument of template function of stdarg with undefined
// argument
template <class T> void Vec<T>::setEntries(int n, const T &value)
{
    // case of stdarg entry
    if (!this->empty() && n == -1)
    {
        if (size())
            while (n < 0)
                n += size();
        (*this)[n] = value;
        return;
    }

    // normal Constructor
    if ((int)size() != n)
    {
        clear();
        this->resize(n);
    }
    FORV(i)(*this)[i] = value;
}

template <class T> void Vec<T>::setEntries(const unsigned n, const T *line)
{
    if (line == NULL)
    { // coloca a NULL se receber NULL
        clear();
        return;
    }
    if (size() != n)
    {
        clear();
        this->resize(n);
    }
    FORV(i)(*this)[i] = line[i];
}

template <class T> void Vec<T>::setEntries(const char *str)
{
    // first count how many numbers the string contains, and then saves it
    // 'entries'

    std::string s(str);
    // detect if string is file name
    if (s.size() >= 4 && s.substr(s.size() - 4, 4) == ".txt")
    {
        // save the whole file in string 's'
        std::ifstream ifs(str);
        if (!ifs.is_open())
        {
            std::cout << "Couldn't open file" << std::endl;
            *this = Vec<T>();
            return;
        }
        getline(ifs, s, (char)ifs.eof());
    }

    if (s.substr(0, 2) == "//")
    {
        clear();
        this->resize(0);
        return;
    }

    T temp;
    char c;
    std::stringstream stream(s.c_str());

    int N = 0;
    while (stream.get(c))
    {
        // isolated '.' will count as 0
        if ((c >= '0' && c <= '9') || c == '.' || c == '-')
        {
            stream.putback(c); // place the 'cursor' one character back
            stream >> temp;    // reads a full number from the stringstream
            N++;
        }
    }

    this->resize(N);

    stream.clear();
    stream.seekg(0, stream.beg);

    unsigned i = 0;
    while (stream.get(c))
    {
        if ((c >= '0' && c <= '9') || c == '.' || c == '-')
        {
            stream.putback(c); // puts back the character read and then saves
                               // the number in the stream to entries
            stream >> (*this)[i++];
        }
    }
}
template <class T> inline void Vec<T>::setEntries(const std::string &str)
{
    setEntries(str.c_str());
}
template <class T>
inline void Vec<T>::setEntries(const std::initializer_list<T> list)
{
    this->std::vector<T>::operator=(list);
}

template <class T>
template <typename... T2>
void Vec<T>::setEntries(const int n, const T x, T2... rest)
{
    if (size() == 0)
        this->resize(n);
    (*this)[size() - n] = x;
    // defined recursively
    n > 2 ? setEntries(n - 1, rest...) : setEntries(-1, rest...);
}

template <class T> T &Vec<T>::get(int i)
{
    if (size())
        while (i < 0)
            i += size();
    return (*this)[i];
}
template <class T> const T &Vec<T>::get(int i) const
{
    if (size())
        while (i < 0)
            i += size();
    return (*this)[i];
}
template <class T> inline T &Vec<T>::operator()(int i) { return get(i); }
template <class T> inline const T &Vec<T>::operator()(int i) const
{
    return get(i);
}

template <class T> Vec<T> Vec<T>::operator-() const &
{
    Vec<T> newv(size());
    FORV(i) newv[i] = -(*this)[i];
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator-() &&
{
    FORV(i)(*this)[i] = -(*this)[i];
    return std::move(*this);
}
// creates a copy!
template <class T> inline Vec<T> Vec<T>::operator+() const &
{
    return Vec<T>(*this);
}
template <class T> inline Vec<T> &&Vec<T>::operator+() &&
{
    return std::move(*this);
}

//############# =
template <class T> Vec<T> &Vec<T>::operator=(const Vec<T> &other)
{
    if (&other != this)
        setEntries(other.size(), other.data());
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator=(const Vec<T> &&other)
{
    std::vector<T>::operator=((Vec<T> &&) other);
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator=(const std::vector<T> &other)
{
    setEntries(other.size(), other.data());
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator=(const std::vector<T> &&other)
{
    std::vector<T>::operator=((std::vector<T> &&) other);
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator=(const char *s)
{
    setEntries(s);
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator=(const std::string &s)
{
    setEntries(s.c_str());
    return *this;
}
template <class T>
Vec<T> &Vec<T>::operator=(const std::initializer_list<T> list)
{
    setEntries(list);
    return *this;
}

//############# +
// it could be defined less efficiently with the operators '=' and '+' above
template <class T> Vec<T> &Vec<T>::operator+=(const Vec<T> &other)
{
    if (size() != other.size())
    {
        std::cerr << "'+=' for vectors with diferent size (nothing done)"
                  << std::endl;
        return *this;
    }
    FORV(i)(*this)[i] += other[i];
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator+=(const T &k)
{
    FORV(i)(*this)[i] += k;
    return *this;
}
template <class T> Vec<T> Vec<T>::operator+(const Vec<T> &other) const &
{
    Vec<T> newv;
    if (size() != other.size())
    {
        std::cerr
            << "'+' for vectors with different size (returned this->empty Vec)"
            << std::endl;
        return newv;
    }
    newv.setEntries(size(), data());
    FORV(i) newv[i] += other[i];
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator+(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned argument)"
                  << std::endl;
        return std::move(other);
    }
    FORV(i) other[i] += (*this)[i];
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator+(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] += other[i];
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator+(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] += other[i];
    return std::move(*this);
}

template <class T> Vec<T> Vec<T>::operator+(const T &k) const &
{
    Vec<T> newv(size(), data());
    FORV(i) newv[i] += k;
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator+(const T &k) &&
{
    FORV(i)(*this)[i] += k;
    return std::move(*this);
}
template <class T> inline Vec<T> operator+(const T &k, const Vec<T> &other)
{
    return other + k;
}
template <class T> inline Vec<T> &&operator+(const T &k, Vec<T> &&other)
{
    return std::move(((Vec<T> &&) other) + k);
}
template <class T> inline Vec<T> &operator+=(const T &k, Vec<T> &other)
{
    other += k;
    return other;
}

//############# -
// it could be defined less efficiently with the operators '+=' and '-' above
template <class T> Vec<T> &Vec<T>::operator-=(const Vec<T> &other)
{
    if (size() != other.size())
    {
        std::cerr << "'-=' for vectors with diferent size (nothing done)"
                  << std::endl;
        return *this;
    }
    FORV(i)(*this)[i] -= other[i];
    return *this;
}
template <class T> Vec<T> &Vec<T>::operator-=(const T &k)
{
    *this += (-k);
    return *this;
}
template <class T> Vec<T> Vec<T>::operator-(const Vec<T> &other) const &
{
    Vec<T> newv;
    if (size() != other.size())
    {
        std::cerr
            << "'-' for vectors with different size (returned this->empty Vec)"
            << std::endl;
        return newv;
    }
    newv.setEntries(size(), data());
    FORV(i) newv[i] -= other[i];
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator-(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned argument)"
                  << std::endl;
        return std::move(other);
    }
    FORV(i) other[i] = (*this)[i] - other[i];
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator-(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] -= other[i];
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator-(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] -= other[i];
    return std::move(*this);
}

template <class T> inline Vec<T> Vec<T>::operator-(const T &k) const &
{
    return (*this) + (-k);
}
template <class T> inline Vec<T> &&Vec<T>::operator-(const T &k) &&
{
    return std::move(((Vec<T> &&)(*this)) + (-k));
}
template <class T> inline Vec<T> operator-(const T &k, const Vec<T> &other)
{
    return k + (-other);
}
template <class T> inline Vec<T> &&operator-(const T &k, Vec<T> &&other)
{
    return std::move((-((Vec<T> &&) other)) + k);
}
template <class T> inline Vec<T> &operator-=(const T &k, Vec<T> &other)
{
    other = k - other;
    return other;
}

//############# *
template <class T> Vec<T> &Vec<T>::operator*=(const Vec<T> &other)
{
    if (size() != other.size())
    {
        std::cerr << "'*=' for vectors with diferent size (nothing done)"
                  << std::endl;
        return *this;
    }
    FORV(i)(*this)[i] *= other[i];
    return *this;
}

template <class T> Vec<T> &Vec<T>::operator*=(const T &k)
{
    FORV(i)(*this)[i] *= k;
    return *this;
}
template <class T> Vec<T> Vec<T>::operator*(const Vec<T> &other) const &
{
    Vec<T> newv;
    if (size() != other.size())
    {
        std::cerr
            << "'*' for vectors with different size (returned this->empty Vec)"
            << std::endl;
        return newv;
    }
    newv.setEntries(size(), data());
    FORV(i) newv[i] *= other[i];
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator*(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned argument)"
                  << std::endl;
        return std::move(other);
    }
    FORV(i) other[i] *= (*this)[i];
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator*(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] *= other[i];
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator*(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] *= other[i];
    return std::move(*this);
}

template <class T> Vec<T> Vec<T>::operator*(const T &k) const &
{
    Vec<T> newv(size(), data());
    FORV(i) newv[i] *= k;
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator*(const T &k) &&
{
    FORV(i)(*this)[i] *= k;
    return std::move(*this);
}
template <class T> inline Vec<T> operator*(const T &k, const Vec<T> &other)
{
    return other * k;
}
template <class T> inline Vec<T> &&operator*(const T &k, Vec<T> &&other)
{
    return std::move(((Vec<T> &&) other) * k);
}
template <class T> inline Vec<T> &operator*=(const T &k, Vec<T> &other)
{
    other *= k;
    return other;
}

//############# /
template <class T> Vec<T> &Vec<T>::operator/=(const Vec<T> &other)
{
    if (size() != other.size())
    {
        std::cerr << "'/=' for vectors with diferent size (nothing done)"
                  << std::endl;
        return *this;
    }
    FORV(i)(*this)[i] /= other[i];
    return *this;
}
// needed to redefine it instead of doing *(1./k) since for T=int it wouldn't
// work
template <class T> Vec<T> &Vec<T>::operator/=(const T &k)
{
    FORV(i)(*this)[i] /= k;
    return *this;
}
template <class T> Vec<T> Vec<T>::operator/(const Vec<T> &other) const &
{
    Vec<T> newv;
    if (size() != other.size())
    {
        std::cerr
            << "'/' for vectors with different size (returned this->empty Vec)"
            << std::endl;
        return newv;
    }
    newv.setEntries(size(), data());
    FORV(i) newv[i] /= other[i];
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator/(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned argument)"
                  << std::endl;
        return std::move(other);
    }
    FORV(i) other[i] = (*this)[i] / other[i];
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator/(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] /= other[i];
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator/(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "'+' for vectors with different size (returned *this)"
                  << std::endl;
        return std::move(*this);
    }
    FORV(i)(*this)[i] /= other[i];
    return std::move(*this);
}

// needed to redefine it instead of doing *(1./k) since for T=int it wouldn't
// work
template <class T> Vec<T> Vec<T>::operator/(const T &k) const &
{
    Vec<T> newv(size(), data());
    FORV(i) newv[i] /= k;
    return newv;
}
template <class T> Vec<T> &&Vec<T>::operator/(const T &k) &&
{
    FORV(i)(*this)[i] /= k;
    return std::move(*this);
}
template <class T> Vec<T> operator/(const T &k, const Vec<T> &other)
{
    Vec<T> newv(other);
    FORV(i, other.size()) newv[i] = k / newv[i];
    return newv;
}
template <class T> Vec<T> &&operator/(const T &k, Vec<T> &&other)
{
    FORV(i, other.size()) other[i] = k / other[i];
    return std::move(other);
}
template <class T> Vec<T> &operator/=(const T &k, Vec<T> &other)
{
    FORV(i, other.size()) other[i] = k / other[i];
    return other;
}

template <class T> Vec<T> Vec<T>::operator>(const Vec<T> &other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) out[i] = (*this)[i] > other[i] ? 1 : 0;
    return out;
}
template <class T> Vec<T> &&Vec<T>::operator>(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i) other[i] = (*this)[i] > other[i] ? 1 : 0;
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator>(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i)(*this)[i] = (*this)[i] > other[i] ? 1 : 0;
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator>(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i)(*this)[i] = (*this)[i] > other[i] ? 1 : 0;
    return std::move(*this);
}

template <class T> Vec<T> Vec<T>::operator>=(const Vec<T> &other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) out[i] = (*this)[i] >= other[i] ? 1 : 0;
    return out;
}
template <class T> Vec<T> &&Vec<T>::operator>=(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i) other[i] = (*this)[i] >= other[i] ? 1 : 0;
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator>=(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i)(*this)[i] = (*this)[i] >= other[i] ? 1 : 0;
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator>=(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    FORV(i)(*this)[i] = (*this)[i] >= other[i] ? 1 : 0;
    return std::move(*this);
}

template <class T> Vec<T> Vec<T>::operator<(const Vec<T> &other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) out[i] = (*this)[i] < other[i] ? 1 : 0;
    return out;
}
template <class T> Vec<T> &&Vec<T>::operator<(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) other[i] = (*this)[i] < other[i] ? 1 : 0;
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator<(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i)(*this)[i] = (*this)[i] < other[i] ? 1 : 0;
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator<(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i)(*this)[i] = (*this)[i] < other[i] ? 1 : 0;
    return std::move(*this);
}

template <class T> Vec<T> Vec<T>::operator<=(const Vec<T> &other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) out[i] = (*this)[i] <= other[i] ? 1 : 0;
    return out;
}
template <class T> Vec<T> &&Vec<T>::operator<=(Vec<T> &&other) const &
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i) other[i] = (*this)[i] <= other[i] ? 1 : 0;
    return std::move(other);
}
template <class T> Vec<T> &&Vec<T>::operator<=(const Vec<T> &other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i)(*this)[i] = (*this)[i] <= other[i] ? 1 : 0;
    return std::move(*this);
}
template <class T> Vec<T> &&Vec<T>::operator<=(Vec<T> &&other) &&
{
    if (size() != other.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(size());
    FORV(i)(*this)[i] = (*this)[i] <= other[i] ? 1 : 0;
    return std::move(*this);
}

template <class T> inline bool Vec<T>::operator!=(const Vec<T> &other) const
{
    return !((*this) == other);
}
template <class T> bool Vec<T>::operator==(const Vec<T> &other) const
{
    if (size() != other.size())
        return false;
    FORV(i) if ((*this)[i] != other[i]) return false;
    return true;
}

template <class T> inline Vec<T> Vec<T>::operator&(const Vec<T> &v) const &
{
    Vec<T> newv(*this);
    return newv.insert(v, -1);
}
template <class T> inline Vec<T> &&Vec<T>::operator&(Vec<T> &&v) const &
{
    return std::move(((Vec<T> &&) v).insert(v, -1));
}
template <class T> inline Vec<T> &&Vec<T>::operator&(const Vec<T> &v) &&
{
    return std::move(((Vec<T> &&)(*this)).insert(v, -1));
}
template <class T> inline Vec<T> &&Vec<T>::operator&(Vec<T> &&v) &&
{
    return std::move(((Vec<T> &&)(*this)).insert(v, -1));
}

//###################

template <class T> inline Vec<T> Vec<T>::mapV(T (*f)(T)) const &
{
    Vec<T> out(size());
    std::transform(this->begin(), this->end(), out.begin(), f);
    // FORV(i)	out[i]=f((*this)[i]);
    return out;
}
template <class T> inline Vec<T> &&Vec<T>::mapV(T (*f)(T)) &&
{
    std::transform(this->begin(), this->end(), this->begin(), f);
    return std::move(*this);
}
template <class T2> inline Vec<T2> mapV(T2 (*f)(T2), const Vec<T2> &v)
{
    return v.mapV(f);
}
template <class T2> inline Vec<T2> &&mapV(T2 (*f)(T2), Vec<T2> &&v)
{
    return std::move(((Vec<T2> &&) v).mapV(f));
}
template <class T>
template <class T2>
inline Vec<T2> Vec<T>::mapV(T2 (*f)(T)) const
{
    Vec<T2> out(size());
    std::transform(this->begin(), this->end(), out.begin(), f);
    // FORV(i) out[i]=f((*this)[i]);
    return out;
}
template <class T2, class T3> inline Vec<T2> mapV(T2 (*f)(T3), const Vec<T3> &v)
{
    return v.mapV(f);
}

template <class T>
Vec<T> mapV2(T (*f)(T, T), const Vec<T> &v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "'map' for vectors with different size (returned "
                     "this->empty Vec)"
                  << std::endl;
        return Vec<T>();
    }
    Vec<T> out(v1.size());
    FORV(i, v1.size()) out[i] = f(v1[i], v2[i]);
    return out;
}
template <class T> Vec<T> &&mapV2(T (*f)(T, T), Vec<T> &&v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr
            << "'map' for vectors with different size (returned 1st vector)"
            << std::endl;
        return std::move(v1);
    }
    FORV(i, v1.size()) v1[i] = f(v1[i], v2[i]);
    return std::move(v1);
}
template <class T> Vec<T> &&mapV2(T (*f)(T, T), const Vec<T> &v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr
            << "'map' for vectors with different size (returned 2nd vector)"
            << std::endl;
        return std::move(v2);
    }
    FORV(i, v1.size()) v2[i] = f(v1[i], v2[i]);
    return std::move(v2);
}
template <class T> Vec<T> &&mapV2(T (*f)(T, T), Vec<T> &&v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr
            << "'map' for vectors with different size (returned 1st vector)"
            << std::endl;
        return std::move(v1);
    }
    FORV(i, v1.size()) v1[i] = f(v1[i], v2[i]);
    return std::move(v1);
}

template <class T> Vec<T> max(const Vec<T> &v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(v1.size());
    FORV(i, v1.size()) out[i] = v1[i] > v2[i] ? v1[i] : v2[i];
    return out;
}
template <class T> Vec<T> &&max(Vec<T> &&v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v1[i] = v1[i] > v2[i] ? v1[i] : v2[i];
    return std::move(v1);
}
template <class T> Vec<T> &&max(const Vec<T> &v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v2[i] = v1[i] > v2[i] ? v1[i] : v2[i];
    return std::move(v2);
}
template <class T> Vec<T> &&max(Vec<T> &&v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v1[i] = v1[i] > v2[i] ? v1[i] : v2[i];
    return std::move(v1);
}

template <class T> Vec<T> min(const Vec<T> &v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return Vec<T>();
    }
    Vec<T> out(v1.size());
    FORV(i, v1.size()) out[i] = v1[i] < v2[i] ? v1[i] : v2[i];
    return out;
}
template <class T> Vec<T> &&min(Vec<T> &&v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v1[i] = v1[i] < v2[i] ? v1[i] : v2[i];
    return std::move(v1);
}
template <class T> Vec<T> &&min(const Vec<T> &v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v2[i] = v1[i] < v2[i] ? v1[i] : v2[i];
    return std::move(v2);
}
template <class T> Vec<T> &&min(Vec<T> &&v1, Vec<T> &&v2)
{
    if (v1.size() != v2.size())
    {
        std::cerr << "Different size vectors\n" << std::endl;
        return std::move(Vec<T>());
    }
    FORV(i, v1.size()) v1[i] = v1[i] < v2[i] ? v1[i] : v2[i];
    return std::move(v1);
}

template <class T> T Vec<T>::dot(const Vec<T> &other) const
{
    T prod = 0;

    if (size() != other.size())
    {
        std::cerr << "dot for vectors with different size (return 0)"
                  << std::endl;
        return prod;
    }

    FORV(i) prod += (*this)[i] * other[i];
    return prod;
}

template <class T>
std::string Vec<T>::toString(const std::string title,
                             const unsigned precision) const
{
    std::stringstream str;
    if (title != "")
        str << title << "\t";
    str << std::scientific << std::setprecision(precision);
    FORV(i)
    {
        // std::cout.width(precision);
        str << std::left << (*this)[i] << "\t" << std::flush;
    }
    return str.str();
}

template <class T>
void Vec<T>::print(const std::string title, const unsigned precision) const
{
    std::cout << "\t";
    if (title != "")
        std::cout << title << "\t";
    std::cout << std::setprecision(precision);
    FORV(i)
    {
        // std::cout.width(precision);
        std::cout << std::left << (*this)[i] << "\t" << std::flush;
    }
    std::cout << std::endl;
}
template <class T>
int Vec<T>::print(const std::string name, const std::string title,
                  const std::string mode, const unsigned precision,
                  const std::string separator, const int warning) const
{
    if (name.size() >= 4 && name.substr(name.size() - 4, 4) == ".txt")
    {
        std::ofstream::openmode open =
            (mode == "append" || mode == "a" || mode == "app")
                ? std::ofstream::app
                : std::ofstream::out;
        std::ofstream file(name.c_str(), open);

        if (title != "")
            file << title;
        file << std::setprecision(precision);
        FORV(i, size() - 1)
        {
            // file.width(precision);
            file << (*this)[i] << separator;
        }
        if (size() > 0)
        {
            // file.width(precision);
            file << (*this)[size() - 1] << "\n";
        }
        file.close();
        if (warning)
            std::cout << "File " << name << " correctly written." << std::endl;
        return 1;
    }
    if (!warning)
        print(name, precision);
    else
        std::cout << "'" << name << "' is not a .txt file name" << std::endl;
    return 0;
}
// swap a and b indixes
template <class T> Vec<T> &Vec<T>::swap(const unsigned a, const unsigned b)
{
    if (a > size() - 1 || b > size() - 1 || a == b)
    {
        std::cerr << "error when changing positions " << a << " and " << b
                  << std::endl;
        return *this;
    }
    T temp = (*this)[a];
    (*this)[a] = (*this)[b];
    (*this)[b] = temp;
    return *this;
}

template <class T> Vec<T> Vec<T>::reduce(int start, int end) const &
{
    if (!size())
        return Vec<T>();

    while (start < 0)
        start += size();
    while (end < 0)
        end += size();

    if (start >= (int)size())
        start = size() - 1;
    if (end >= (int)size())
        end = size() - 1;
    if (start > end)
    {
        int temp = start;
        start = end;
        end = temp;
    }

    Vec<T> v(end - start + 1, 0.);
    FORV(i, start, end + 1) v[i - start] = (*this)[i];
    return v;
}
template <class T> Vec<T> &&Vec<T>::reduce(int start, int end) &&
{
    if (!size())
        return std::move(*this);

    while (start < 0)
        start += size();
    while (end < 0)
        end += size();

    if (start >= (int)size())
        start = size() - 1;
    if (end >= (int)size())
        end = size() - 1;
    if (start > end)
    {
        int temp = start;
        start = end;
        end = temp;
    }

    FORV(i, start, end + 1)(*this)[i - start] = (*this)[i];
    this->resize(end - start + 1); // resize only after setting all indices
    return std::move(*this);
}

template <class T> Vec<T> &Vec<T>::remove(int start, int end, bool force)
{
    if (!size())
        return *this;

    while (start < 0)
        start += size(); // or recursion "remove(start+N,end)"

    if (force)
    {
        if (end < start || end >= (int)size())
            end = start;
        else if (start > end)
        {
            int temp = start;
            start = end;
            end = temp;
        }
    }
    else
    {
        while (end < 0)
            end += size(); // or recursion "remove(start,end+N)"
        if (end < start)
            return *this;
    }

    FORV(i, start, size() - (end - start + 1))
    (*this)[i] = (*this)[i + end - start + 1];

    // this->resize(N-(end-start+1));
    this->resize(size() - (end - start + 1));

    return *this;
}

template <class T> Vec<T> &Vec<T>::removeV(const T &v)
{
    int i;
    while ((i = find(v)) >= 0)
        remove(i);

    return *this;
}

template <class T> Vec<T> &Vec<T>::insert(const Vec<T> &v, int index)
{
    if (v.size() == 0)
        return *this;
    if (index < 0)
    {
        if (size())
            while (index < 0)
                index += size();
        index++; // to place it after, as usual default is place before
    }
    if (index > (int)size())
        index = size();

    this->resize(size() + v.size()); // M changes to 'N+v.size()'

    for (unsigned i = size() - 1; i >= index + v.size(); i--)
        (*this)[i] = (*this)[i - v.size()];
    FORV(i, v.size())(*this)[i + index] = v[i];

    return *this;
}

template <class T> int Vec<T>::find(const T &value, unsigned Nocorr) const
{
    FORV(i) if ((*this)[i] == value && 1 == Nocorr--) return i;
    return -1;
}

template <class T> unsigned Vec<T>::findMax(int start, int end) const
{
    if (end < 0 || end >= (int)size())
        end = size() - 1;
    if (start > end)
    {
        int temp = start;
        start = end;
        end = temp;
    }
    if (start < 0)
        start = 0;

    T m = (*this)[start];
    int index = start;
    FORV(i, start + 1, end + 1)
    {
        if ((*this)[i] > m)
        {
            m = (*this)[i];
            index = i;
        }
    }
    return index;
}

template <class T> unsigned Vec<T>::findMin(int start, int end) const
{
    if (end < 0 || end >= (int)size())
        end = size() - 1;
    if (start > end)
    {
        int temp = start;
        start = end;
        end = temp;
    }
    if (start < 0)
        start = 0;

    T m = (*this)[start];
    int index = start;
    FORV(i, start + 1, end + 1)
    {
        if ((*this)[i] < m)
        {
            m = (*this)[i];
            index = i;
        }
    }
    return index;
}

template <class T> unsigned Vec<T>::findZero(int start, int end) const
{
    if (end < 0 || end >= (int)size())
        end = size() - 1;
    if (start > end)
    {
        int temp = start;
        start = end;
        end = temp;
    }
    if (start < 0)
        start = 0;

    unsigned n;
    int sign = (*this)[start] < 0 ? -1 : 1;
    for (n = start; (int)n <= end && (*this)[n] * sign > 0.; n++)
        ; // just iterate

    return n;
}

// norm p: (sum(v[i]^p))^(1/p)
// norm 0 uses norm infinity = max(Vec)
template <class T> T norm(const Vec<T> &v, const unsigned type)
{
    T res = 0.;
    if (v.size() == 0)
        return res;
    if (type == 0)
    {
        res = v[0];
        FORV(i, 1, v.size()) if (fabs(v[i]) > fabs(res)) res = v[i];
        return res;
    }
    else
    {
        FORV(i, v.size()) res += pow(v[i], type);
    }
    return pow(res, 1. / type);
}
template <class T> inline T Vec<T>::norm(const unsigned type) const
{
    return ::norm(*this, type);
}

// Standard Deviation of v1-v2
template <class T> T averageDifference(const Vec<T> &v1, const Vec<T> &v2)
{
    if (v1.size() != v2.size())
        return 0.;

    T med = 0., desv = 0.;
    FORV(i, v1.size())
    {
        med += fabs(v1[i] - v2[i]);
        desv += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sqrt((T)v1.size() * desv - med * med) / v1.size();
}

template <class T> bool descend(T i, T j)
{
    return (i > j);
} // to use in std::sort for descendent order
template <class T> Vec<T> Vec<T>::sort(bool down) const &
{
    Vec<T> vv(*this);
    if (down)
        std::sort(vv.begin(), vv.begin() + size(), descend<T>);
    else
        std::sort(vv.begin(), vv.begin() + size());
    return vv;
}
template <class T> Vec<T> &&Vec<T>::sort(bool down) &&
{
    if (down)
        std::sort(this->begin(), this->begin() + size(), descend<T>);
    else
        std::sort(this->begin(), this->begin() + size());
    return std::move(*this);
}

template <class T> inline Vec<T> Vec<T>::invert() const &
{
    Vec<T> out(size());
    FORV(i) out[i] = (*this)[size() - i - 1];
    return out;
}
template <class T> inline Vec<T> &&Vec<T>::invert() &&
{
    FORV(i)(*this)[i] = (*this)[size() - i - 1];
    return std::move(*this);
}

template <class T> inline Vec<T> &Vec<T>::indices()
{
    FORV(i)(*this)[i] = T(i);
    return *this;
}

template <class T> inline T Vec<T>::sum() const
{
    T out = 0;
    FORV(i) out += (*this)[i];
    return out;
}

template <class T> Vec<T> Vec<T>::shift(int s) const &
{
    if (s < 0)
    {
        if (size())
            while (s < 0)
                s += size();
    }
    else
        s = s % size();

    Vec<T> out(size());
    FORV(i, s) out[i] = (*this)[size() - s + i];
    FORV(i, s, size()) out[i] = (*this)[i - s];

    return out;
}
template <class T> Vec<T> &&Vec<T>::shift(int s) &&
{
    if (s < 0)
    {
        if (size())
            while (s < 0)
                s += size();
    }
    else
        s = s % size();

    FORV(i, s)(*this)[i] = (*this)[size() - s + i];
    FORV(i, s, size())(*this)[i] = (*this)[i - s];

    return std::move(*this);
}

template <class T> Vec<T> &Vec<T>::randomise(const T &a, const T &b)
{
    srand(time(NULL) + clock());
    FORV(i)(*this)[i] = T((double)rand() / RAND_MAX * (b - a) + a);
    return *this;
}

#endif