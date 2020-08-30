#ifndef _MATRIX_
#define _MATRIX_

#include <vector>
#include <string>

#include "Vec.hpp"

template <class T>
class Matrix: public Vec<Vec<T> >{
public:
// constructors
	Matrix(const unsigned l=0,const unsigned c=0, const T& value=0);
	Matrix(const T** fM,const unsigned l,const unsigned c); //matrix l x c (initializes with 0 if fM is not indicated)
	Matrix(const T* fM,const unsigned l,const unsigned c);
	Matrix(const Vec<T>& v, const unsigned l, const unsigned c);
	Matrix(const Vec<T>& v, unsigned c=0); //finds 'l' by filling until the end
	Matrix(const std::vector<Vec<T> >& v): Vec<Vec<T> >(v) {}
	Matrix(const Vec<Vec<T>>& v): Vec<Vec<T>>(v){}
	//example: Matrix v("1 2 3\n4.1 5.2 6.3") or "1,2,3\n4 5 6" or "1asd2 p!?3\n4.1-5.2-6.3" are all accepted
	Matrix(const char* str);
	Matrix(const std::string& str);
	Matrix(const std::vector<char*> str);
	Matrix(const std::vector<std::string> str);
// operators

	template <class T2> operator Matrix<T2>() const;

	Matrix& operator=(const char* str);
	Matrix& operator=(const std::string& str);
	Matrix& operator=(const std::vector<char*> str);
	Matrix& operator=(const std::vector<std::string> str);

	Matrix& operator+=(const Matrix& m){ return this->Vec<Vec<T>>::operator+=(m); }
	Matrix& operator+=(const T&);
	Matrix  operator+ (const Matrix& m) const{ return this->Vec<Vec<T>>::operator+(m); }
	Matrix  operator+ (const T&) const;
	template <class T2> friend Matrix<T2>  operator+ (const T2&, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>& operator+=(const T2&, Matrix<T2>&);

	Matrix& operator-=(const T&);
	Matrix  operator- (const T&) const;
	template <class T2> friend Matrix<T2>  operator- (const T2&, const Matrix<T2>&);
	template <class T2> friend Matrix<T2>& operator-=(const T2&, Matrix<T2>&);

	Matrix& operator*=(const T&);
	Matrix  operator* (const T& k)   const { return prod(k); }
	template <class T2> friend Matrix<T2>& operator*=(const T2&, Matrix<T2>&);
	template <class T2> friend Matrix<T2>  operator* (const T2& k, const Matrix<T2>& m);

	Matrix& operator/=(const T&);
	Matrix  operator/ (const T&) const;

	Matrix operator&(const Matrix& m) const{ return this->Vec<Vec<T>>::operator&(m); }

	Matrix mapM(T (*f)(T)) const;
	friend Matrix mapM(T (*f)(T), const Matrix& m){return m.mapV(f);}
	template <class T2> friend Matrix<T2> map2M(T2 (*f)(T2,T2), const Matrix<T2>& m1, const Matrix<T2>& m2);

	Matrix prod(const Matrix&) const; // multiply 2 matrices of any kind
	friend Matrix prod(const Matrix& mat1, const Matrix& mat2){return mat1.prod(mat2);}
	Matrix prod(const T&)   const; // multiply matrix of any kind by scalar
	template <class T2> friend Matrix<T2> prod(const T2&, const Matrix<T2>&);
	Vec<T>    prod(const Vec<T>&)      const; // multiply matrix by vector
	friend Vec<T>   prod(const Matrix& mat, const Vec<T>& vec){return mat.prod(vec);} // multiply matrix by vector
	Vec<T>    operator*(const Vec<T>& v)      const{return prod(v);} // multiply matrix by vector
	// friend Matrix prod(const Vec&, const Vec&); //multiply two Vec's (vertical*horizontal=matrix) -- see below!

	T& geti(const Vec<unsigned>);	//recieves a Vec = {i,j} with the position
	const T& geti(const Vec<unsigned>) const;
	Vec<T> getRow(int i) const{return this->get(i);} // retrieve row i (a copy)
	Vec<T> getCol(int i) const; // retrieve column i
	unsigned getNL() const {return this->size();}	//number of lines
	unsigned getNC() const;					//number of columns

	//joins/concatenate matrixes, storing the new one under the first one
	Matrix join(const Matrix& mat, const bool doSide = false) const{return doSide ? ((*this).transpose()&mat.transpose()).transpose() : (*this)&mat;}
	friend Matrix join(const Matrix& mat1, const Matrix& mat2, const bool doSide = false){return mat1.join(mat2,doSide);}

	T determinant() const;
	//get index of the maximum value of row 'i', starting at column 'from'
	//Note: leave 'max' and 's' with double or replace by T?
	int getRowMax(const unsigned i=0,const unsigned from=0,const bool s=false) const; //s !=false if the comparison is to be made with the relative maximum between lines, and not the absolute maximum
	int getRowMin(const unsigned i=0,const unsigned from=0,const bool s=false) const;
	int getColMax(const unsigned j=0,const unsigned from=0,const bool s=false) const; //s is useful for sorting by the RELATIVE maximum (ex: Gauss Elimination algorithm)
	int getColMin(const unsigned j=0,const unsigned from=0,const bool s=false) const;
	Matrix& swapRows(const unsigned i,const unsigned j){this->swap(i,j); return *this;}
	Matrix& swapCols(const unsigned i,const unsigned j){Matrix temp = transpose().swap(i,j); *this=temp.transpose(); return *this;}
	Matrix& moveRow(const unsigned r1,const unsigned r2);	//insert row 'r1' on index 'r2' (moving all rows in between one row up)
	Matrix& moveCol(const unsigned c1,const unsigned c2){Matrix temp = transpose().moveRow(c1,c2); *this = temp.transpose(); return *this;}

	//Matrix& Transpose();//Defining a non-const that would change the matrix itself when the matrix in not a 'const Matrix' 
	Matrix transpose() const;
	//PRITING - Print() or Print(text) prints normal text in the command line (it goes to the "file Print" function first, but leaves after detecting no '.txt')
	void print(const std::string title, const unsigned precision) const;
	friend std::ostream& operator<<(std::ostream& os, const Matrix& mat){mat.print();return os;} //cout overloaded (example: std::cout << mat1 << mat2 << std::endl;)
	//print to file
	//mode - change to "a", "app" or "append" to append instead of rewriting
	//'separator' between numbers
	//warning for prints
	//reject if some number should be excluded (for example, 0's may be excluded if wanted)
	int print(const std::string name="", const std::string title="", const std::string mode="", const unsigned precision=4, const std::string separator=" ", const int warning =0, const T& reject=1.e8) const;

	//original std::vector<> functions
	// void reserve(const unsigned N)	{M.reserve(N);}
	// void resize (const unsigned N)	{M.resize (N);}
	// void push_back(const Vec<T> v)	{M.push_back(v);}
	// Vec<T>& back()					{return M.back();}
	// const Vec<T>& back() const 		{return M.back();}

	Matrix reduceRow(int start=0, int end=-1) const{return this->reduce(start,end);} //matrix from row 'start' to 'end' (end=-1 for end)
	Matrix reduceCol(int start=0, int end=-1) const{Matrix temp = transpose().reduce(start,end); return temp.transpose();}
	Vec<int> find(const T& value, unsigned Nocorr=1) const; //returns index {i,j} (the 'Nocorr''s occurence) that has the value 'value' or {-1,-1} if it doesn't exist
	Vec<int> findMax() const; //index of maximum value
	T max() const{return (*this).geti(findMax());} //maximum value
	Vec<int> findMin() const;
	T min() const{return (*this).geti(findMin());}

	Matrix& removeRow(int start=-1, int end=-1){this->remove(start,end); return *this;}//erase row 'row'
	Matrix& removeCol(int start=-1, int end=-1){Matrix temp = transpose().remove(start,end); *this = temp.transpose(); return *this;}
	Matrix& eraseRow(int start=-1, int end=-1){return removeRow(start,end);}
	Matrix& eraseCol(int start=-1, int end=-1){return removeCol(start,end);}
	Matrix& insertRow(const Vec<T>& v, const int row=-1){this->insert(v,row); return *this;}//add row at row 'row'
	Matrix& insertCol(const Vec<T>& v, const int col=-1){Matrix temp = transpose().insert(v,col); *this = temp.transpose(); return *this;}
	Matrix& insertRow(const T& value=0, const int row=-1){return insertRow(Vec<T>(getNC(),value),row);} //add row with constant value 'value' at row 'row'
	Matrix& insertCol(const T& value=0, const int col=-1){return insertCol(Vec<T>(getNL(),value),col);}

	//Like with the Transpose() method, the methods below could be overloaded with now that would change the matrix itself, while the ones below would be called when the object was a "const Matrix"
	//Example: a "Matrix& LUdecomposition();"
	Matrix LUdecomposition() const;
	//creates matrix with both the L and the U incorporated, where the L is transformed in order to have only 1's in the diagonal, and the lower part is presented in the lower part of the returned matrix.
	//Diagonal and upper part of returned matrix correspond to matrix U.
	Matrix GaussElimination() const{Vec<T> v=Vec<T>(); return GaussElimination(v);}//Gauss Elimination of matrix
	Matrix GaussElimination(Vec<T>& v) const;//Gauss Elimination with vector on the right of the equality, applying transformations to it too
	Matrix GaussElimination(Matrix&) const;//Gauss Elimination with matrix on the right of the equality, applying transformations to it too
	Vec<T> solveGauss(const Vec<T>&) const;//Solve system Ax=b, with b being the given vector
	Matrix solveGauss(const Matrix&) const;//solve system Ax=B, with B being the given matrix

	//solve numerically linear systems Ax=b
	Vec<T> solveGaussSeidel(const Vec<T>& v, const T& tol=1e-4, const int iter_max=1000) const; //tolerance
	Vec<T> solveJacobi(const Vec<T>& v, const T& tol=1e-4, const int iter_max=1000) const;
	Matrix solveGaussSeidel(const Matrix& v, const T& tol=1e-4, const int iter_max=1000) const;
	Matrix solveJacobi(const Matrix& v, const T& tol=1e-4, const int iter_max=1000) const;

	// static Matrix Identity(const unsigned n);	//identity matrix of size 'n' -- see below!
	Matrix inverse() const; //inverse matrix

	friend Matrix pow  (const Matrix& m, const T& power=1)	{return map2(pow,m,Matrix(getNL(),getNC(),power));}
	friend Matrix pow  (const T& base, const Matrix& m)		{return map2(pow,Matrix(getNL(),getNC(),base),m);}

	template <class T2> friend Matrix<T2> max  (const Matrix<T2>&, const Matrix<T2>&);
	template <class T2> friend Matrix<T2> min  (const Matrix<T2>&, const Matrix<T2>&);

protected:
	Matrix reduceM(const unsigned l, const unsigned c) const; //matrix without row 'l' and column 'c' (used in the determinant for recorrence)
	Vec<T> iterativeSolver(const Vec<T>&, const int type, const T& tol=1e-4, const int iter_max=1000) const; //type==0 - jacobi; else - gauss-seidel
	Matrix iterativeSolver(const Matrix&, const int type, const T& tol=1e-4, const int iter_max=1000) const; //type==0 - jacobi; else - gauss-seidel
};

template <class T>
Matrix<T> prod(const Vec<T> &, const Vec<T> &); 	//v1*(v2^T) multiply two Vec's (vertical*horizontal=matrix)
template <class T>
Matrix<T> identity(const unsigned n);				//identity matrix of size 'n'
template <class T>
Matrix<T> powM(const Matrix<T>& m, const T& power=1); //real operation m*m*m*...*m

/*template <class T>
class Vec<Vec<T> >{
public:
	operator Matrix<T>() const;
};
*/

typedef Matrix<double> MatrixD;

#include "Matrix.impl.hpp"

#endif