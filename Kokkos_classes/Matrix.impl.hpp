#if !defined(_MATRIX_)
#error "This file should only be included through Vec.hpp"
#endif

#ifndef _MATRIX_IMPL_
#define _MATRIX_IMPL_

#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <cstdio>
#include <cmath>
 

template <class T>
Matrix<T>::Matrix(const unsigned l,const unsigned c, const T& value){
// Matrix<T>::Matrix(const unsigned l,const unsigned c, const T& value): Vec<Vec<T>>(l,Vec<T>(c,value)){
	Vec<T> temp(c,value);
	this->setEntries(l,temp);
	// this->setEntries(l,Vec<T>(c,value));
}

template <class T>
Matrix<T>::Matrix(const T** fM,const unsigned l,const unsigned c){
	this->setEntries(l,Vec<T>(c));
	for (unsigned i=0; i<l; i++)
		(*this)[i].setEntries(c, fM[i]);
}

template <class T>
Matrix<T>::Matrix(const T* fM,const unsigned l,const unsigned c){
	this->setEntries(l,Vec<T>(c));
	for (unsigned i=0; i<l; i++)
		(*this)[i].setEntries(c, fM+i*c);
}

template <class T>
Matrix<T>::Matrix(const Vec<T>& v, const unsigned l, const unsigned c){
	if(v.size()<l*c){
		std::cerr << "Vector of insufficient size for " << l << "x" << c << " matrix"<< std::endl;
		return;
	}
	this->setEntries(l,Vec<T>(c,0.));

	unsigned i;
	for(i=0; i<l; ++i)
		(*this)[i] = v.reduce(i*c,i*c+c-1);
}
template <class T>
Matrix<T>::Matrix(const Vec<T>& v, unsigned c){
	if(c==0)
		c = v.size();
	else if(v.size()<c){
		std::cerr << "Vector of insufficient size for matrix with " << c << " columns"<< std::endl;
		return;
	}
	unsigned l = ((v.size()-1)/c+1);
	this->setEntries(l,Vec<T>(c));
	unsigned i;
	for(i=0; i<l-1; ++i){
		(*this)[i] = v.reduce(i*c,i*c+c-1);
	}
	//insert last line, that may not be complete
	(*this)[i]=(v.reduce(i*c,-1).insert(0,-1,l*c-v.size())); //insert last elements extend with 0's until the end of the column
}



template <class T>
Matrix<T>::Matrix(const char* str){*this = str;}
template <class T>
Matrix<T>::Matrix(const std::string& str){*this = str;}
template <class T>
Matrix<T>::Matrix(const std::vector<char*> str){*this = str;}
template <class T>
Matrix<T>::Matrix(const std::vector<std::string> str){*this = str;}

template <class T>
Matrix<T> DimError(const Matrix<T>& mat1, const Matrix<T>& mat2, std::string type){
	std::cerr << "  method " << type << " called for matrixes of different size |";
	std::cerr << mat1.getNL() << "," << mat1.getNC() << "| e |";
	std::cerr << mat2.getNL() << "," << mat2.getNC() << "| ";
	std::cerr << "(returns empty Matrix)" << std::endl;
	return Matrix<T>();
}

template <class T>
template <class T2>
Matrix<T>::operator Matrix<T2>() const{
	Matrix<T2> newv(this->M,(*this)[0].capacity());
	for(unsigned i=0; i<this->N; i++)
		newv[i]=(Vec<T2>)((*this)[i]);
	return newv;
}

template <class T>
Matrix<T>& Matrix<T>::operator=(const char* str){
	//first count how many numbers the string contains, and then saves it 'entries'

	std::vector<std::string> out;
	std::string s(str);
	//detect if string is file name
	if(s.substr(s.size()-4,4)==".txt"){
		//save the whole file in vector out divided by lines
		std::ifstream ifs (str);
		if(!ifs.is_open()){
			printf("Couldn't open file");
			*this = Matrix<T>();
			return *this; 
		}
		while(getline(ifs,s,(char)'\n'))
			out.push_back(s);
	}
	else{
		int pos;
		while((pos=s.find('\n'))!=(int)(std::string::npos)){
			out.push_back(s.substr(0, pos));
    		s.erase(0, pos + 1 );//pos + 1 = lenght
		}
		out.push_back(s);
	}
	*this = out;//call the operator=(std::vector<std::string>)
	return *this;
}
template <class T>
Matrix<T>& Matrix<T>::operator=(const std::string& str){
	*this = str.c_str();//call the operator=(char*)
	return *this;
}
template <class T>
Matrix<T>& Matrix<T>::operator=(const std::vector<char*> str){
	std::vector<std::string> s;
	for(unsigned i=0; i<str.size();i++)
		s.push_back(std::string(str[i]));
	*this = s;//call the operator=(std::vector<std::string>)
	return *this;
}
template <class T>
Matrix<T>& Matrix<T>::operator=(const std::vector<std::string> str){
	this->clear();
	for (unsigned i=0; i < str.size(); ++i){
		Vec<T> temp = Vec<T>(str[i]);//call the Vec constructor with strings
		if(temp.size()!=0) this->push_back(temp);
	}
	return *this;
}

//###################

template <class T>
Matrix<T>& Matrix<T>::operator+=(const T& lambda){
	for (unsigned i = 0; i < getNL(); ++i)
		this->entries[i]+=lambda;
	return *this;
}
template <class T>
Matrix<T>  Matrix<T>::operator+ (const T& lambda) const{
	Matrix<T> newm(*this);
	for (unsigned i = 0; i < getNL(); ++i)
		newm[i]+=lambda;
	return newm;
}
template <class T2> Matrix<T2> operator+ (const T2& lambda, const Matrix<T2>& m){ return m+lambda; }
template <class T2> Matrix<T2>& operator+=(const T2& lambda, Matrix<T2>& m){
	m += lambda;
	return m;
}

template <class T>
Matrix<T>& Matrix<T>::operator-=(const T& lambda){
	for (unsigned i = 0; i < getNL(); ++i)
		this->entries[i]-=lambda;
	return *this;
}
template <class T>
Matrix<T>  Matrix<T>::operator- (const T& lambda) const{
	Matrix<T> newm(*this);
	for (unsigned i = 0; i < getNL(); ++i)
		newm[i]-=lambda;
	return newm;
}
template <class T2> Matrix<T2> operator- (const T2& lambda, const Matrix<T2>& m){ return lambda + (-m); }
template <class T2> Matrix<T2>& operator-=(const T2& lambda, Matrix<T2>& m){
	m=lambda-m;
	return m;
}

template <class T>
Matrix<T>& Matrix<T>::operator*=(const T& lambda){
	for (unsigned i = 0; i < getNL(); ++i)
		this->entries[i]*=lambda;
	return *this;
}
template <class T2> Matrix<T2>& operator*=(const T2& lambda, Matrix<T2>& m){
	m *= lambda;
	return m;
}
template <class T2> Matrix<T2>  operator* (const T2& k, const Matrix<T2>& m){ return prod(k,m); }


template <class T>
Matrix<T>& Matrix<T>::operator/=(const T& lambda){
	for (unsigned i = 0; i < getNL(); ++i)
		this->entries[i]/=lambda;
	return *this;
}
template <class T>
Matrix<T>  Matrix<T>::operator/ (const T& lambda) const{
	Matrix<T> newm(*this);
	for (unsigned i = 0; i < getNL(); ++i)
		newm[i]/=lambda;
	return newm;
}

template <class T>
Matrix<T> Matrix<T>::mapM(T (*f)(T)) const{
	Matrix<T> out(getNL(),getNC());
	for(unsigned i=0; i<getNL(); i++)
		out[i]=mapV(f,this->entries[i]);
	return out;
}
template <class T2> Matrix<T2> map2M(T2 (*f)(T2,T2), const Matrix<T2>& m1, const Matrix<T2>& m2){
	if(m1.getNL() != m2.getNL() || m1.getNC() != m2.getNC()){
		std::cerr << "'map' for matrices with different size (returned empty Matrix)" << std::endl;
		return Matrix<T2>();
	}
	Matrix<T2> out(m1.getNL(), m1.getNC());
	for(unsigned i=0; i<m1.getNL(); i++)
		out[i]=mapV2(f,m1[i],m2[i]);
	return out;
}

//###################


template <class T>
Matrix<T> Matrix<T>::prod(const Matrix<T>& other) const{
	if(getNC() != other.getNL())
		return DimError(*this, other, "*");

	//more efficient than crating Vec's
	T matrix [getNL()*other.getNC()];
	for(unsigned i = 0; i < getNL(); i++){
		for(unsigned j = 0; j < other.getNC(); j++){
			matrix[i*other.getNC()+j]=0;
			for (unsigned k = 0; k < getNC(); ++k){
				matrix[i*other.getNC() + j] += (*this)[i][k] * other[k][j];
			}
		}
	}
	return Matrix<T>(matrix, getNL(), other.getNC());//constructor
}

template <class T>
Matrix<T> Matrix<T>::prod(const T& lambda) const{
	Matrix<T> newm(*this);
	for (unsigned i = 0; i < getNL(); ++i)
		newm[i]*=lambda;
	return newm;
}

template <class T>
Matrix<T> prod(const T& lambda, const Matrix<T>& mat){
	return mat*lambda;
}

template <class T>
Vec<T> Matrix<T>::prod(const Vec<T>& v) const{
	if(getNC() != v.size()){
		std::cerr << "  operador* called for matrix and vector of incompatible size |";
		std::cerr << getNL() << "," << getNC() << "| e |";
		std::cerr << v.size() << "| ";
		std::cerr << "(returns empty Vec)" << std::endl;
		return Vec<T>();
	}

	Vec<T> newv(getNL());
	for (unsigned i = 0; i < getNL(); ++i)
		newv[i]=(*this)[i].dot(v);
	return newv;

}

template <class T>
Matrix<T> prod(const Vec<T>& v1, const Vec<T>& v2){
	if(v1.size() != v2.size()){
		std::cerr << "  operador* called between vectors of incompatible sizes |";
		std::cerr << v1.size() << "| e |";
		std::cerr << v2.size() << "| ";
		std::cerr << "(returns empty Matrix)" << std::endl;
		return Matrix<T>();
	}

	T matriz [v1.size()*v2.size()];
	for(unsigned i = 0; i < v1.size(); ++i){
		for(unsigned j = 0; j < v2.size(); ++j){
				matriz[i*v2.size() + j] = v1[i] * v2[j];
		}
	}
	Matrix<T> newm(matriz, v1.size(), v2.size());
	return newm;
}


///////////////////////////////////////////////////

template <class T>
T& Matrix<T>::geti(const Vec<unsigned> v){
	if(v.size()==0) return this->entries[0][0];
	if(v.size()==1) return this->get(v[0])[0];
	return (this->get(v[0])).get(v[1]);
}

template <class T>
const T& Matrix<T>::geti(const Vec<unsigned> v) const{
	if(v.size()==0) return this->entries[0][0];
	if(v.size()==1) return this->get(v[0])[0];
	return (this->get(v[0])).get(v[1]);
}

template <class T>
Vec<T>    Matrix<T>::getCol(int j) const{
	while(j<0) j+=getNC();
	Vec<T> newv(getNL());
	for (unsigned i = 0; i < getNL(); ++i){
		newv[i] = ((*this)[i])[j];
	}
	return newv;
}

template <class T>
unsigned Matrix<T>::getNC() const{
	if(this->N==0)
		return 0;
	else
		return (*this)[0].size();
}

//returns the matrix without line l and column c
template <class T>
Matrix<T> Matrix<T>::reduceM(const unsigned l,const unsigned c) const{
	if(l>=getNL() || c>=getNC()){
		std::cerr << "Wrong dimensions " << l << " and " << c << std::endl;
		return Matrix<T>(0,0);
	}

	T matriz [(getNL()-1)*(getNC()-1)];
	for(unsigned k=0, i = 0; i < getNL(); ++i){
		//i,j iterate the total matrix; k defines how many elements have alerady been copied to the reduced matrix
		for(unsigned j = 0; j < getNC(); ++j)
			if(i!=l && j!=c) matriz[k++]=(*this)[i][j];
	}
	Matrix temp = Matrix<T>(matriz, getNL()-1, getNC()-1);//constructor
	return temp;
}


//Lagrange's Method (maybe after this i'll make it with Gauss Elimination and pivot multiplciation)
template <class T>
T Matrix<T>::determinant() const{
	if(getNL()!=getNC()){
		std::cerr << "Non-square matrix" << std::endl;
		return 0;
	}
	else if(getNL()==1)
		return (*this)[0][0];

	T det=0;
	for(unsigned j=0; j<getNL(); j++)
		det+=(-2*(int)(j%2)+1)*(*this)[0][j]*reduceM(0,j).determinant();
	//Note: be careful that (j%2) was being recognised as unsigned so '-1' was transformed to positive
	//(-2*(int)(j%2)+1) retorna o coeficiente (-1)^j
	return det;
}

//if(s), it calculates the sum over the column j, to then calculate the maximum from a relative point of view
//example: matrix {{1,2,3},{1,4,5}} has a maximum of 3 on row 1 (index 0), but from a relative point of view the row is {1/(1+1),2/(2+4),3/(3+5)}={1/2,1/3,3/8}
//so the maximum relative value is 1
//useful for the gauss elimination algorithm, where lines with the maximum relative value are placed on the top for efficiency purposes
//Note: leave 'max' and 's' with double or replace by T?
template <class T>
int Matrix<T>::getRowMax(const unsigned i,const unsigned from,const bool s) const{
	if(i>=getNL()){
		std::cerr << "Wrong dimensions " << i << ">=" << getNL() << std::endl;
		return -1.;
	}
	double max = 0.;
	unsigned pos_max = from;
	for (unsigned j = from; j < getNC(); ++j){
		double scale = 1.;
		if(s){
			scale=0.;
			for(unsigned k=0; k< getNL(); ++k)
				scale+=fabs((*this)[k][j]);//changed at 18/11/17 - had no fabs
		}
		if(fabs((*this)[i][j])/scale > max){
			max = fabs((*this)[i][j])/scale;
			pos_max = j;
		}
	}
	return  pos_max;
}
template <class T>
int Matrix<T>::getRowMin(const unsigned i,const unsigned from,const bool s) const{
	if(i>=getNL()){
		std::cerr << "Wrong dimensions " << i << ">=" << getNL() << std::endl;
		return -1.;
	}
	double min = 0.;
	unsigned pos_min = from;
	for (unsigned j = from; j < getNC(); ++j){
		double scale = 1.;
		if(s){
			scale=0.;
			for(unsigned k=0; k< getNL(); ++k)
				scale+=fabs((*this)[k][j]);
		}
		if(fabs((*this)[i][j])/scale < min){
			min = fabs((*this)[i][j])/scale;
			pos_min = j;
		}
	}
	return  pos_min;
}
//similar to the previous one, but over columns instead of rows
template <class T>
int Matrix<T>::getColMax(const unsigned j,const unsigned from,const bool s) const{
	if(j>=getNC()){
		std::cerr << "Wrong dimensions " << j << ">=" << getNC() << std::endl;
		return -1.;
	}
	double max = 0.;
	unsigned pos_max = from;
	for (unsigned i = from; i < getNL(); ++i){
		double scale = 1.;
		if(s){
			scale=0.;
			for(unsigned k=0; k< getNC(); ++k)
				scale+=fabs((*this)[i][k]);
		}
		if(fabs((*this)[i][j])/scale > max){
			max = fabs((*this)[i][j])/scale;
			pos_max = i;
		}
	}
	return  pos_max;
}
template <class T>
int Matrix<T>::getColMin(const unsigned j,const unsigned from,const bool s) const{
	if(j>=getNC()){
		std::cerr << "Wrong dimensions " << j << ">=" << getNC() << std::endl;
		return -1.;
	}
	double min = 0.;
	unsigned pos_min = from;
	for (unsigned i = from; i < getNL(); ++i){
		double scale = 1.;
		if(s){
			scale=0.;
			for(unsigned k=0; k< getNC(); ++k)
				scale+=fabs((*this)[i][k]);
		}
		if(fabs((*this)[i][j])/scale < min){
			min = fabs((*this)[i][j])/scale;
			pos_min = i;
		}
	}
	return  pos_min;
}

//insert row 'r1' on index 'r2' (moving all rows in between one row up)
template <class T>
Matrix<T>&   Matrix<T>::moveRow(const unsigned r1,const unsigned r2){
	int sign = r1>r2 ? -1 : 1;
	for(unsigned i=r1; i!=r2; i+=sign)
		this->swap(i,i+sign);
	return *this;
}

template <class T>
Matrix<T> Matrix<T>::transpose() const{
	std::vector<Vec<T> > FM;
	FM.reserve(getNC());
	for(unsigned i=0; i<getNC(); i++){
		Vec<T> temp(getNL());
		for(unsigned j=0; j<getNL(); j++)
			temp[j] = (*this)[j][i];
		FM.push_back(temp);
	}
	return Matrix<T>(FM);
}

template <class T>
void Matrix<T>::print(const std::string title,const unsigned precision) const{
	if(title!="") std::cout << "\t" << title << "\n";
	for (unsigned i=0; i<getNL(); i++)
		(*this)[i].Vec<T>::print("",precision);
	std::cout << std::endl;
}
template <class T>
int Matrix<T>::print(const std::string name, const std::string title, const std::string mode, const unsigned precision, const std::string separator, const int warning, const T& reject) const{
	if(name.size()>=4 && name.substr(name.size()-4,4)==".txt"){
		int warns=0;
		std::string title2 = "";
		if(title!="") title2 = title+"\n";
		if(getNL()>0)
			warns+=(this->entries[0]).print(name,title2,mode,precision,separator,0,reject);
		for(unsigned i=1;i<getNL();i++)
			warns+=(this->entries[i]).print(name,"","a",precision,separator,0,reject);

		if(warns && warning){
  			std::cout << "File " << name << " correctly written." << std::endl;
		}
		return 1;
	}
	if(!warning) print(name,precision);
 	else std::cout << "'" << name <<  "' is not a .txt file name" << std::endl;
	return 0;
}


template <class T>
Vec<int> Matrix<T>::find(const T& value, unsigned Nocorr) const{
	Vec<int> index(2,-1);
	for(unsigned i=0; i<getNL(); i++)
		for(unsigned j=0; j<=getNC(); j++)
			if((*this)[i][j]==value && 1==Nocorr--){
				index[0]=i;
				index[1]=j;
				return index;
			}
	return index;
}

template <class T>
Vec<int> Matrix<T>::findMax() const{
	Vec<int> index(2);
	T max=(*this)[0][0];
	for(unsigned i=0; i<getNL(); i++){
		unsigned j = (*this)[i].findMax();
		if((*this)[i][j]>max){
			max=(*this)[i][j];
			index[0]=i;
			index[1]=j;
		}
	}
	return index;
}

template <class T>
Vec<int> Matrix<T>::findMin() const{
	Vec<int> index(2);
	T min=(*this)[0][0];
	for(unsigned i=0; i<getNL(); i++){
		unsigned j = (*this)[i].findMin();
		if((*this)[i][j]>min){
			min=(*this)[i][j];
			index[0]=i;
			index[1]=j;
		}
	}
	return index;
}

//creates matrix with both the L and the U incorporated, where the L is transformed in order to have only 1's in the diagonal, and the lower part is presented in the lower part of the returned matrix.
//Diagonal and upper part of returned matrix correspond to matrix U.
template <class T>
Matrix<T> Matrix<T>::LUdecomposition() const
{
	Matrix<T> mat(*this);

	if(mat.getNL()!=mat.getNC())
	{
		std::cout << "[EqSolver::LUdecomposition] matriz nao quadrada: linhas:" << mat.getNL() << "colunas:" << mat.getNC() << std::endl;
		return mat;
	}

	for (unsigned k = 0; k < mat.getNL(); ++k)
	{
		for (unsigned j = k; j < mat.getNL(); ++j)
		{
			T sum = 0;
			for (unsigned r = 0; r < k; ++r)
			{
				sum += mat[k][r]*mat[r][j];
			}
			mat[k][j] = mat[k][j] - sum;
		}

		for (unsigned i = k+1; i < mat.getNL(); ++i)
		{
			T sum = 0;
			for (unsigned r = 0; r < k; ++r)
			{
				sum += mat[i][r]*mat[r][k];
			}
			mat[i][k] = (mat[i][k] - sum)/mat[k][k];
		}
	}
	return mat;
}


template <class T>
Matrix<T> Matrix<T>::GaussElimination(Vec<T>& v) const
{
	Matrix<T> newm(*this);
	if(!(v.size()==0 || (v.size()==getNC() && getNL()==getNC()))){
		std::cerr << "Invalid matriz or vector size" << std::endl;
		v=Vec<T>();
		return newm;
	}

	unsigned in_line = 0;
	for (unsigned j = 0; j < newm.getNC(); ++j)
	{
		T x;
		//get column with the biggest relative value to the top 
		unsigned index = newm.getColMax(j,j,1);
		if(index != in_line){
			newm.swapRows(in_line,index);
			if(v.size()!=0) v.swap(in_line,index);
		}

		if(newm[in_line][j]!=0)
		{
			for(unsigned i1 = in_line+1; i1 < newm.getNL(); ++i1)
			{
				x = newm[i1][j]/newm[in_line][j];
				newm[i1]-=x*newm[in_line];
				if(v.size()!=0) v[i1]-=x*v[in_line];
			}
			++in_line;
		}
		if(in_line>newm.getNL()-1) break;
	}
	return newm;
}
template <class T>
Matrix<T> Matrix<T>::GaussElimination(Matrix<T>& other) const
{
	Matrix<T> newm(*this);
	if(getNL() != other.getNL() || getNC() != other.getNC()){
		std::cerr << "Matrizes com tamanhos diferentes (nada feito)" << std::endl;
		other=Matrix<T>();
		return *this;
	}

	unsigned in_line = 0;
	for (unsigned j = 0; j < newm.getNC(); ++j)
	{
		T x;
		//get column with the biggest relative value to the top 
		unsigned index = newm.getColMax(j,j,1);
		if(index != in_line){
			newm.swapRows(in_line,index);
			other.swapRows(in_line,index);
		}
		
		if(newm[in_line][j]!=0)
		{
			for(unsigned i1 = in_line+1; i1 < newm.getNL(); ++i1)
			{
				x = newm[i1][j]/newm[in_line][j];
				newm[i1]-=x*newm[in_line];
				other[i1]-=x*other[in_line];
			}
			++in_line;
		}
		if(in_line>newm.getNL()-1) break;
	}
	return newm;
}

template <class T>
Vec<T> Matrix<T>::solveGauss(const Vec<T>& v) const{
	if(v.size()!=getNC() || getNL()!=getNC()){
		std::cerr << "Sistema sem solução" << std::endl;
		return Vec<T>(v.size());
	}

	//Gauss Elimination
	Vec<T> b(v);
	Matrix<T> newm=GaussElimination(b);
	if(b.size()==0){
		return Vec<T>(v.size());
	}

	for(unsigned i=0; i<b.size() ;i++){
		if(newm[i][i]==0){
			std::cout << "[SolveGauss] m matriz singular: (pivot " << i << ")==0" << std::endl;
			return Vec<T>(b.size());
		}
	}

	//With Gauss Elimination done and non singularity checked, the algorithm proceeds
	Vec<T> ans(newm.getNL());

	for (int i = newm.getNL()-1; i >= 0; --i)//NOTE: CANNOT BE unsigned BECAUSE OF UNDERFLOW
	{
		T subtract = 0;
		for (unsigned j = i+1; j < newm.getNC(); ++j)
		{
			subtract += (ans[j]* newm[i][j]);
		}
		ans[i] = (b[i]-subtract)/newm[i][i];
	}
	return ans;
}

//similar to previuos
template <class T>
Matrix<T> Matrix<T>::solveGauss(const Matrix<T>& other) const{
	if(getNL()!=getNC() || getNL()!=other.getNL() || getNC()!=other.getNC()){
		std::cerr << "Sistema sem solução" << std::endl;
		return Matrix<T>(getNL(),getNC());
	}

	Matrix<T> b(other);
	Matrix<T> newm=GaussElimination(b);
	if(other.getNL()==0){
		return Matrix<T>(getNL(),getNC());
	}

	for(unsigned i=0; i<b.getNL() ;i++){
		if(newm[i][i]==0){
			std::cout << "[SolveGauss] m matriz singular: (pivot " << i << ")==0" << std::endl;
			return Matrix<T>(getNL(),getNC());
		}
	}

	Matrix<T> ans(newm.getNL(),newm.getNC());

	for (int i = newm.getNL()-1; i >= 0; --i)
	{
		Vec<T> subtract(b.getNC());
		for (unsigned j = i+1; j < newm.getNC(); ++j)
		{
			subtract += (ans[j]* newm[i][j]);
		}
		ans[i] = (b[i]-subtract)/newm[i][i];
	}
	return ans;
}


template <class T>
Vec<T> Matrix<T>::solveGaussSeidel(const Vec<T>& b, const T& tol, const int iter_max) const	{return iterativeSolver(b,1,tol,iter_max);}
template <class T>
Vec<T> Matrix<T>::solveJacobi(const Vec<T>& b, const T& tol, const int iter_max) const		{return iterativeSolver(b,0,tol,iter_max);}
//type==0 - jacobi; else - gauss-seidel
template <class T>
Vec<T> Matrix<T>::iterativeSolver(const Vec<T>& b, const int type, const T& tol, const int iter_max) const
{
	if(getNL()!=getNC() || b.size()!=getNL())
	{
		std::cout << "[Matrix::IterativeSolve] M matriz nao quadrada: linhas:" << getNL() << "colunas:" << getNC() << std::endl;
		Vec<T> err(b.size());
		return err;
	}
// linear system of m unknowns
	Vec<T> x(b.size()); //zero’s
	Vec<T> x_aux(b.size()); //zero’s


	bool btol = false;
	int it = 0.;
	while (!btol && (it++ < iter_max))
	{
		x_aux = x;
		for (unsigned i=0; i<b.size(); i++)
		{
			x[i] = 0.;
			for (unsigned j=0; j<b.size(); j++)
				
				if (i != j) x[i] += -(((*this)[i])[j]) * (type==0 ? x_aux[j] : x[j]);
			x[i] += b[i];
			x[i] /= ((*this)[i])[i];
//guarantee that all vector entries are converging equally
			if (fabs(x[i]-x_aux[i]) < tol && (i==0 || btol==true)) btol = true;
			else btol = false;
		}
	}
	if(it>=iter_max)
		std::cout << "[Matrix::IterativeSolver] Max Iterations reached" << std::endl;

	return x;
}

template <class T>
Matrix<T> Matrix<T>::solveGaussSeidel(const Matrix<T>& b, const T& tol, const int iter_max) const	{return iterativeSolver(b,1,tol,iter_max);}
template <class T>
Matrix<T> Matrix<T>::solveJacobi(const Matrix<T>& b, const T& tol, const int iter_max) const		{return iterativeSolver(b,0,tol,iter_max);}
//type==0 - jacobi; else - gauss-seidel
template <class T>
Matrix<T> Matrix<T>::iterativeSolver(const Matrix<T>& b, const int type, const T& tol, int const iter_max) const
{
	if(getNL()!=getNC() || getNL()!=b.getNL()){
		std::cerr << "[Matrix::IterativeSolve] Wrong dimensions" << std::endl;
		return Matrix(getNL(),getNC());
	}
	// linear system of m unknowns
	Matrix<T> x(b.getNL(),b.getNC()); //zero’s
	Matrix<T> x_aux(b.getNL(),b.getNC()); //zero’s

	bool btol = false;
	int it = 0.;
	while (!btol && (it++ < iter_max))
	{
		x_aux = x;
		for(unsigned col=0; col<b.getNC(); col++){
			for (unsigned i=0; i<b.getNL(); i++)
			{
				x[i][col] = 0.;
				for (unsigned j=0; j<b.getNL(); j++)
				
					if (i != j) x[i][col] += -(((*this)[i])[j]) * (type==0 ? x_aux[j][col] : x[j][col]);
				x[i][col] += b[i][col];
				x[i][col] /= ((*this)[i])[i];
				//guarantee that all matrix entries are converging equally
				if (fabs(x[i][col]-x_aux[i][col]) < tol && ((i==0 && col==0) || btol==true)) btol = true;
				else btol = false;
			}
		}
	}
	if(it>=iter_max)
		std::cout << "[Matrix::IterativeSolver] Max Iterations reached" << std::endl;

	return x;
}






template <class T>
Matrix<T> identity(const unsigned n){
	Matrix<T> mat(n,n);
	for(unsigned i=0; i<n;i++)
		mat[i][i]=1;
	return mat;
}

//done by solving Ax=I, which as a solution of x=A^-1
template <class T>
Matrix<T> Matrix<T>::inverse() const{
	if(getNL()!=getNC()){
		std::cout << "Non-square matrix - not invertible" << std::endl;
		return *this;
	}
	return solveGauss(identity<T>(getNL()));
}

template <class T>
Matrix<T> powM(const Matrix<T>& m, const T& power){
	Matrix<T> mat = identity(m.getNC());
	for(unsigned i=0; i<power; ++i)
		mat *= m;
	return mat;
}


template <class T>
Matrix<T> max  (const Matrix<T>& mat1, const Matrix<T>& mat2){
	if(mat1.getNL() != mat2.getNL() || mat1.getNC() != mat2.getNC())
		return DimError(mat1, mat2, "max");

	Matrix<T> out(mat1.getNL(),mat1.getNC());
	for(unsigned i=0; i<mat1.getNL();i++)
		out[i] = max(mat1[i],mat2[i]);
	return out;
}
template <class T>
Matrix<T> min  (const Matrix<T>& mat1, const Matrix<T>& mat2){
	if(mat1.getNL() != mat2.getNL() || mat1.getNC() != mat2.getNC())
		return DimError(mat1, mat2, "min");

	Matrix<T> out(mat1.getNL(),mat1.getNC());
	for(unsigned i=0; i<mat1.getNL();i++)
		out[i] = min(mat1[i],mat2[i]);
	return out;
}

/*template <class T>
Vec<Vec<T> >::operator Matrix<T>() const{
	Matrix<T> newm(this->M,Vec<T>((*this)[0].M));
	for(unsigned i=0; i<this->N; i++)
		newm[i]=(*this)[i];
	return newm;
}*/

#endif