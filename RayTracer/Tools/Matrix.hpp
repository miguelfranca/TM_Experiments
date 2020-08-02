#ifndef _MATRIX_
#define _MATRIX_

#include <vector>
#include <string>

#include "Vec.hpp"

#define FORM(IDX) FORV(IDX, getNL())
#define FORM2(IDX, IDX2) FORV(IDX, getNL()) FORV(IDX2, getNC())


template <class T>
class Matrix: public Vec<Vec<T> >{
public:
// constructors
	inline Matrix(const unsigned l=0,const unsigned c=0, const T& value=0);
	inline Matrix(const T** fM,const unsigned l,const unsigned c); //matrix l x c (initializes with 0 if fM is not indicated)
	inline Matrix(const T* fM,const unsigned l,const unsigned c);
	Matrix(const Vec<T>& v, const unsigned l, const unsigned c);
	Matrix(const Vec<T>& v, unsigned c=0); //finds 'l' by filling until the end
	inline Matrix(unsigned n_copies, const Vec<T>& v);
	inline Matrix(const std::vector<Vec<T> >& v): Vec<Vec<T> >(v) {}
	inline Matrix(const Vec<Vec<T>>& v): Vec<Vec<T>>(v){}
	//example: Matrix v("1 2 3\n4.1 5.2 6.3") or "1,2,3\n4 5 6" or "1asd2 p!?3\n4.1-5.2-6.3" are all accepted
	inline Matrix(const char* str);
	inline Matrix(const std::string& str);
	inline Matrix(const std::vector<char*> str);
	inline Matrix(const std::vector<std::string> str);
	inline Matrix(const std::initializer_list<const std::initializer_list<T> > list);
// operators

	template <class T2> operator Matrix<T2>() const;

	inline Matrix   operator- () const&	{ return Vec<Vec<T>>::operator-(); }
	inline Matrix&& operator- () &&  	{ return std::move(((Matrix&&)(*this)).Vec<Vec<T>>::operator-()); }
	inline Matrix   operator+ () const&	{ return Vec<Vec<T>>::operator+(); }
	inline Matrix&& operator+ () &&		{ return std::move(((Matrix&&)(*this)).Vec<Vec<T>>::operator+()); }

	Matrix& operator=(const char* str);
	inline Matrix& operator=(const std::string& str);
	Matrix& operator=(const std::vector<char*> str);
	Matrix& operator=(const std::vector<std::string> str);

	inline Matrix& operator+=(const Matrix& m){ Vec<Vec<T>>::operator+=(m); return *this; }
	inline Matrix& operator+=(const T&);
	inline Matrix   operator+(const Matrix& m) 	const&	{ return Vec<Vec<T>>::operator+(m); }
	inline Matrix&& operator+(Matrix&& m) 		const&	{ Vec<Vec<T>>::operator+((Matrix&&)m); 						return std::move(m); }
	inline Matrix&& operator+(const Matrix& m) 	&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator+(m); 			return std::move(*this); }
	inline Matrix&& operator+(Matrix&& m) 		&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator+((Matrix&&)m); 	return std::move(*this); }
	inline Matrix   operator+(const T&) const&;
	inline Matrix&& operator+(const T&) &&;
	template <class T2> inline friend Matrix<T2>   operator+ (const T2&, const Matrix<T2>&);
	template <class T2> inline friend Matrix<T2>&& operator+ (const T2&, Matrix<T2>&&);
	template <class T2> inline friend Matrix<T2>&  operator+=(const T2&, Matrix<T2>&);

	inline Matrix& operator-=(const Matrix& m){ Vec<Vec<T>>::operator-=(m); return *this; }
	inline Matrix& operator-=(const T&);
	inline Matrix   operator-(const Matrix& m) 	const&	{ return Vec<Vec<T>>::operator-(m); }
	inline Matrix&& operator-(Matrix&& m) 		const&	{ Vec<Vec<T>>::operator-((Matrix&&)m); 						return std::move(m); }
	inline Matrix&& operator-(const Matrix& m) 	&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator-(m); 			return std::move(*this); }
	inline Matrix&& operator-(Matrix&& m) 		&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator-((Matrix&&)m); 	return std::move(*this); }
	inline Matrix   operator-(const T&) const&;
	inline Matrix&& operator-(const T&) &&;
	template <class T2> inline friend Matrix<T2>   operator- (const T2&, const Matrix<T2>&);
	template <class T2> inline friend Matrix<T2>&& operator- (const T2&, Matrix<T2>&&);
	template <class T2> inline friend Matrix<T2>& operator-=(const T2&, Matrix<T2>&);

	inline Matrix& operator*=(const Matrix& m){ Vec<Vec<T>>::operator*=(m); return *this; }
	inline Matrix& operator*=(const T&);
	inline Matrix   operator*(const Matrix& m) 	const&	{ Vec<Vec<T>>::operator*(m); return *this; }
	inline Matrix&& operator*(Matrix&& m) 		const&	{ Vec<Vec<T>>::operator*((Matrix&&)m); 						return std::move(m); }
	inline Matrix&& operator*(const Matrix& m) 	&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator*(m); 			return std::move(*this); }
	inline Matrix&& operator*(Matrix&& m) 		&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator*((Matrix&&)m); 	return std::move(*this); }
	inline Matrix   operator*(const T& k) const& 		{ return prod(k); }
	inline Matrix&& operator*(const T& k) &&	 		{ return ((Matrix&&)(*this)).prod(k); }
	template <class T2> inline friend Matrix<T2>   operator*(const T2& k, const Matrix<T2>& m);
	template <class T2> inline friend Matrix<T2>&& operator*(const T2& k, Matrix<T2>&& m);
	template <class T2> inline friend Matrix<T2>& operator*=(const T2&, Matrix<T2>&);

	inline Matrix& operator/=(const Matrix& m){ Vec<Vec<T>>::operator/=(m); return *this; }
	inline Matrix& operator/=(const T&);
	inline Matrix   operator/(const Matrix& m) 	const&	{ return Vec<Vec<T>>::operator/(m); }
	inline Matrix&& operator/(Matrix&& m) 		const&	{ Vec<Vec<T>>::operator/((Matrix&&)m); 						return std::move(m); }
	inline Matrix&& operator/(const Matrix& m) 	&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator/(m); 			return std::move(*this); }
	inline Matrix&& operator/(Matrix&& m) 		&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator/((Matrix&&)m); 	return std::move(*this); }
	inline Matrix   operator/ (const T&) const&;
	inline Matrix&& operator/ (const T&) &&;

	inline Matrix   operator&(const Matrix& m) 	const&	{ return Vec<Vec<T>>::operator&(m); }
	inline Matrix&& operator&(Matrix&& m) 		const&	{ Vec<Vec<T>>::operator&((Matrix&&)m); 						return std::move(m); }
	inline Matrix&& operator&(const Matrix& m) 	&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator&(m); 			return std::move(*this); }
	inline Matrix&& operator&(Matrix&& m) 		&&		{ ((Matrix&&)(*this)).Vec<Vec<T>>::operator&((Matrix&&)m); 	return std::move(*this);; }

	inline Matrix   mapM(T (*f)(T)) const&;
	inline Matrix&& mapM(T (*f)(T)) &&;
	inline friend Matrix   mapM(T (*f)(T), const Matrix& m)	{ return m.mapM(f);}
	inline friend Matrix&& mapM(T (*f)(T), Matrix&& m)	 	{ return std::move(((Matrix&&)m).mapM(f));}
	template <class T2> friend Matrix<T2>   mapM2(T2 (*f)(T2,T2), const Matrix<T2>& m1, const Matrix<T2>& m2);
	template <class T2> friend Matrix<T2>&& mapM2(T2 (*f)(T2,T2), Matrix<T2>&& m1, const Matrix<T2>& m2);
	template <class T2> friend Matrix<T2>&& mapM2(T2 (*f)(T2,T2), const Matrix<T2>& m1, Matrix<T2>&& m2);
	template <class T2> friend Matrix<T2>&& mapM2(T2 (*f)(T2,T2), Matrix<T2>&& m1, Matrix<T2>&& m2);

	Matrix   prod(const Matrix&) const; // multiply 2 matrices of any kind (this one can't really have a && version, by it's algorithm)
	inline friend Matrix prod(const Matrix& mat1, const Matrix& mat2){return mat1.prod(mat2);}
	inline Matrix   prod(const T&) const&; // multiply matrix of any kind by scalar
	inline Matrix&& prod(const T&) &&; // multiply matrix of any kind by scalar
	template <class T2> inline friend Matrix<T2>   prod(const T2&, const Matrix<T2>&);
	template <class T2> inline friend Matrix<T2>&& prod(const T2&, Matrix<T2>&&);
	Vec<T>   prod(const Vec<T>&)      const; // multiply matrix by vector (this one can't really have a && version, by it's algorithm)
	inline friend Vec<T>   prod(const Matrix& mat, const Vec<T>& vec){return mat.prod(vec);} // multiply matrix by vector
	inline Vec<T>    operator*(const Vec<T>& v)      const{return prod(v);} // multiply matrix by vector
	// friend Matrix prod(const Vec&, const Vec&); //multiply two Vec's (vertical*horizontal=matrix) -- see below!

	inline T& 			get(int,int);//return the element itself, allowing for negative indexes as "counting from end"
	inline const T& 	get(int,int) const;
	inline T&  			operator()(int,int);
	inline const T&  	operator()(int,int) const;

	inline T& get(const Vec<unsigned>&);	//recieves a Vec = {i,j} with the position
	inline const T& get(const Vec<unsigned>&) const;
	inline Vec<T> getRow(int i) const{return this->get(i);} // retrieve row i (a copy)
	Vec<T> getCol(int i) const; // retrieve column i
	inline unsigned getNL() const {return this->size();}	//number of lines
	inline unsigned getNC() const;									//number of columns

	//joins/concatenate matrixes, storing the new one under the first one
	inline Matrix   joinV(const Matrix& mat) const&						{ return (*this)&mat; }
	inline Matrix&& joinV(Matrix&& mat) 	 const&						{ return std::move(((Matrix&&)(*this))&mat); }
	inline Matrix&& joinV(const Matrix& mat) &&							{ return std::move((*this)&((Matrix&&)mat)); }
	inline Matrix&& joinV(Matrix&& mat) 	 &&							{ return std::move(((Matrix&&)(*this))&((Matrix&&)mat)); }
	inline friend Matrix joinV(const Matrix& mat1, const Matrix& mat2)	{ return mat1.joinV(mat2); }
	inline friend Matrix joinV(Matrix&& mat1	 , const Matrix& mat2)	{ return std::move(((Matrix&&)mat1).joinV(mat2)); }
	inline friend Matrix joinV(const Matrix& mat1, Matrix&& mat2	 )	{ return std::move(mat1.joinV(((Matrix&&)mat2))); }
	inline friend Matrix joinV(Matrix&& mat1	 , Matrix&& mat2	 )	{ return std::move(((Matrix&&)mat1).joinV(((Matrix&&)mat2))); }

	inline Matrix joinH(const Matrix& mat) const&						{ return ((*this).transpose()&mat.transpose()).transpose(); }
	inline friend Matrix joinH(const Matrix& mat1, const Matrix& mat2)	{ return std::move(mat1.joinH(mat2)); }

	T determinant() const;
	//get index of the maximum value of row 'i', starting at column 'from'
	//Note: leave 'max' and 's' with double or replace by T?
	int getRowMax(const unsigned i=0,const unsigned from=0,const bool s=false) const; //s !=false if the comparison is to be made with the relative maximum between lines, and not the absolute maximum
	int getRowMin(const unsigned i=0,const unsigned from=0,const bool s=false) const;
	int getColMax(const unsigned j=0,const unsigned from=0,const bool s=false) const; //s is useful for sorting by the RELATIVE maximum (ex: Gauss Elimination algorithm)
	int getColMin(const unsigned j=0,const unsigned from=0,const bool s=false) const;
	inline Matrix& swapRows(const unsigned i,const unsigned j){this->swap(i,j); return *this;}
	inline Matrix& swapCols(const unsigned i,const unsigned j){Matrix temp = transpose().swap(i,j); *this=temp.transpose(); return *this;}
	Matrix& moveRow(const unsigned r1,const unsigned r2);	//insert row 'r1' on index 'r2' (moving all rows in between one row up)
	inline Matrix& moveCol(const unsigned c1,const unsigned c2){Matrix temp = transpose().moveRow(c1,c2); *this = temp.transpose(); return *this;}

	Matrix   transpose() const;
	Matrix   transposeSQ() const&{ return transpose(); } //for square matrices (so that I can easily define a && method)
	Matrix&& transposeSQ() &&;
	//PRITING - Print() or Print(text) prints normal text in the command line (it goes to the "file Print" function first, but leaves after detecting no '.txt')
	std::string toString(const std::string title="", const unsigned precision=3) const;
	void print(const std::string title, const unsigned precision) const;
	inline friend std::ostream& operator<<(std::ostream& os, const Matrix& mat){mat.print();return os;} //cout overloaded (example: std::cout << mat1 << mat2 << std::endl;)
	//print to file
	//mode - change to "a", "app" or "append" to append instead of rewriting
	//'separator' between numbers
	//warning for prints
	//reject if some number should be excluded (for example, 0's may be excluded if wanted)
	int print(const std::string name="", const std::string title="", const std::string mode="", const unsigned precision=4, const std::string separator=" ", const int warning =0) const;

	//original std::vector<> functions
	// void reserve(const unsigned N)	{M.reserve(N);}
	// void resize (const unsigned N)	{M.resize (N);}
	// void push_back(const Vec<T> v)	{M.push_back(v);}
	// Vec<T>& back()					{return M.back();}
	// const Vec<T>& back() const 		{return M.back();}

	inline Matrix   reduceRow(int start=0, int end=-1) const&	{return this->reduce(start,end);} //matrix from row 'start' to 'end' (end=-1 for end)
	inline Matrix&& reduceRow(int start=0, int end=-1) &&	   	{return std::move(((Matrix&&)(*this)).reduce(start,end));} //matrix from row 'start' to 'end' (end=-1 for end)
	inline Matrix   reduceCol(int start=0, int end=-1) const{Matrix temp = transpose().reduce(start,end); return temp.transpose();}
	Vec<int> find(const T& value, unsigned Nocorr=1) const; //returns index {i,j} (the 'Nocorr''s occurence) that has the value 'value' or {-1,-1} if it doesn't exist
	Vec<int> findMax() const; //index of maximum value
	inline T max() const{return (*this).get(findMax());} //maximum value
	Vec<int> findMin() const;
	inline T min() const{return (*this).get(findMin());}

	inline Matrix& removeRow(int start=-1, int end=-1, bool force=true){this->remove(start,end,force); return *this;}//erase row 'row'
	inline Matrix& removeCol(int start=-1, int end=-1, bool force=true){Matrix temp = transpose().remove(start,end,force); *this = temp.transpose(); return *this;}
	inline Matrix& eraseRow(int start=-1, int end=-1){return removeRow(start,end,false);}
	inline Matrix& eraseCol(int start=-1, int end=-1){return removeCol(start,end,false);}
	inline Matrix& insertRow(const Vec<T>& v, const int row=-1){this->insert(v,row); return *this;}//add row at row 'row'
	inline Matrix& insertCol(const Vec<T>& v, const int col=-1){Matrix temp = transpose().insert(v,col); *this = temp.transpose(); return *this;}
	inline Matrix& insertRow(const T& value=0, const int row=-1){return insertRow(Vec<T>(getNC(),value),row);} //add row with constant value 'value' at row 'row'
	inline Matrix& insertCol(const T& value=0, const int col=-1){return insertCol(Vec<T>(getNL(),value),col);}

	//Like with the Transpose() method, the methods below could be overloaded with now that would change the matrix itself, while the ones below would be called when the object was a "const Matrix"
	//Example: a "Matrix& LUdecomposition();"
	Matrix   LUdecomposition() const&;
	Matrix&& LUdecomposition() &&;
	//creates matrix with both the L and the U incorporated, where the L is transformed in order to have only 1's in the diagonal, and the lower part is presented in the lower part of the returned matrix.
	//Diagonal and upper part of returned matrix correspond to matrix U.
	inline Matrix   GaussElimination()   const& {Vec<T> v=Vec<T>(); return GaussElimination(v);}//Gauss Elimination of matrix
	inline Matrix&& GaussElimination()   &&		{Vec<T> v=Vec<T>(); return ((Matrix&&)(*this)).GaussElimination(v);}
	Matrix   GaussElimination(Vec<T>& v) const&;//Gauss Elimination with vector on the right of the equality, applying transformations to it too
	Matrix&& GaussElimination(Vec<T>& v) &&;
	Matrix   GaussElimination(Matrix&) 	 const&;
	Matrix&& GaussElimination(Matrix&) 	 &&;
	Vec<T>   solveGauss(const Vec<T>&) 	 const&;//Solve system Ax=b, with b being the given vector
	Vec<T>&& solveGauss(Vec<T>&&) 	   	 const&;
	Vec<T>   solveGauss(const Vec<T>&) 	 &&;
	Vec<T>&& solveGauss(Vec<T>&&) 	 	 &&;
	Matrix   solveGauss(const Matrix&) 	 const&;//solve system Ax=B, with B being the given matrix
	Matrix&& solveGauss(Matrix&&) 	   	 const&;
	Matrix&& solveGauss(const Matrix&) 	 &&;
	Matrix&& solveGauss(Matrix&&) 	 	 &&;

	//solve numerically linear systems Ax=b
	inline Vec<T> solveGaussSeidel(const Vec<T>& v, const T& tol=1e-4, const int iter_max=1000) const; //tolerance
	inline Vec<T> solveJacobi	  (const Vec<T>& v, const T& tol=1e-4, const int iter_max=1000) const;
	inline Matrix solveGaussSeidel(const Matrix& v, const T& tol=1e-4, const int iter_max=1000) const;
	inline Matrix solveJacobi	  (const Matrix& v, const T& tol=1e-4, const int iter_max=1000) const;

	// static Matrix Identity(const unsigned n);	//identity matrix of size 'n' -- see below!
	Matrix   inverse() const&; //inverse matrix
	Matrix&& inverse() &&; 

	inline friend Matrix   pow(const Matrix& m, const T& power)	{return mapM2(pow,m,Matrix(m.getNL(),m.getNC(),power));}
	inline friend Matrix&& pow(Matrix&& m 	, const T& power)	{return std::move(mapM2(pow,(Matrix&&)m,Matrix(m.getNL(),m.getNC(),power)));}
	inline friend Matrix   pow(const T& base, const Matrix& m)	{return mapM2(pow,Matrix(m.getNL(),m.getNC(),base),m);}
	inline friend Matrix&& pow(const T& base, Matrix&& m)		{return std::move(mapM2(pow,Matrix(m.getNL(),m.getNC(),base),(Matrix&&)m));}

	template <class T2> friend Matrix<T2>   max(const Matrix<T2>&	, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>&& max(Matrix<T2>&&		, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>&& max(const Matrix<T2>&	, Matrix<T2>&&);
	template <class T2> friend Matrix<T2>&& max(Matrix<T2>&&		, Matrix<T2>&&);
	template <class T2> friend Matrix<T2>   min(const Matrix<T2>&	, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>&& min(Matrix<T2>&&		, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>&& min(const Matrix<T2>&	, Matrix<T2>&&);
	template <class T2> friend Matrix<T2>&& min(Matrix<T2>&&		, Matrix<T2>&&);

protected:
	Matrix reduceM(const unsigned l, const unsigned c) const; //matrix without row 'l' and column 'c' (used in the determinant for recorrence)
	Vec<T> iterativeSolver(const Vec<T>&, const int type, const T& tol=1e-4, const int iter_max=1000) const; //type==0 - jacobi; else - gauss-seidel
	Matrix iterativeSolver(const Matrix&, const int type, const T& tol=1e-4, const int iter_max=1000) const; //type==0 - jacobi; else - gauss-seidel
};

template <class T>
Matrix<T> prod(const Vec<T> &, const Vec<T> &); 	//v1*(v2^T) multiply two Vec's (vertical*horizontal=matrix)
template <class T>
inline Matrix<T> identity(const unsigned n);				//identity matrix of size 'n'
template <class T>
Matrix<T> powM(const Matrix<T>& m, const T& power); //real operation m*m*m*...*m

/*template <class T>
class Vec<Vec<T> >{
public:
	operator Matrix<T>() const;
};
*/

typedef Matrix<double> MatrixD;

#include "Matrix.impl.hpp"

#endif