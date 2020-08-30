// By Tiago Franca
/*
.hpp for group of useful functions
Main:
 - universal printing functions that receive arbitrary number of arguments of arbitrary types
    print: print all arguments and \n at end
    printOS: same as print, but for a different ostream other than std::cout
    printn: print all arguments with \n between all (same for printnOS)
    printt: print all arguments with \t betwwen all and \n at end (same for printtOS)
    printno: print all arguments with nothing else
 - errorMsg (print error message, generate exception and exit)
 - file operations
*/
#pragma once

#include <iostream>
#include <string>
#include <ostream>

#ifndef FANCY_FORS
    #define FANCY_FORS

    #define FOR0(N) FOR1(i,N)
    #define FOR1(IDX, DIM) FOR1_I(IDX, 0, DIM)
    #define FOR1_I(IDX, INIT, DIM) for(unsigned IDX = INIT; IDX < DIM; ++IDX)
    #define FOR2(IDX1, IDX2, DIM1, DIM2) FOR2(IDX1, IDX2, 0, 0, DIM1, DIM2)
    #define FOR2_2(IDX1, IDX2, DIM1, DIM2, AUX) XXXXXX //not defined
    #define FOR2_I(IDX1, IDX2, INIT1, INIT2, DIM1, DIM2) FOR1_I(IDX1, INIT1, DIM1) FOR1_I(IDX2, INIT2, DIM2)

    #define GET_MACRO(_1,_2,_3,_4,_5,_6, NAME,...) NAME
    #define FOR(...) GET_MACRO(__VA_ARGS__, FOR2_I, FOR2_2, FOR2, FOR1_I, FOR1, FOR0)(__VA_ARGS__)

#endif

template < typename ... ARGS >
std::ostream& printOS(std::ostream& os, ARGS&& ... args)
{
    (void)std::initializer_list< int > { (os << args, 0) ... };
    os << std::endl;
    return os;
}

template < typename ... ARGS >
void printno(ARGS&& ... args)
{
    (void)std::initializer_list< int > { (std::cout << args, 0) ... };
}

template < typename ... ARGS >
void print(ARGS&& ... args)
{
    printno(args...);
    std::cout << std::endl;
}

template < typename ... ARGS >
std::ostream& printnOS(std::ostream& os, ARGS&& ... args)
{
    (void)std::initializer_list< int > { (os << args << std::endl, 0) ... };
    return os;
}

template < typename ... ARGS >
void printn(ARGS&& ... args)
{
    (void)std::initializer_list< int > { (std::cout << args << std::endl, 0) ... };
}

template < typename ... ARGS >
std::ostream& printtOS(std::ostream& os, ARGS&& ... args)
{
    (void)std::initializer_list< int > { (os << args << "\t", 0) ... };
    os << std::endl;
    return os;
}

template < typename ... ARGS >
void printt(ARGS&& ... args)
{
    (void)std::initializer_list< int > { (std::cout << args << "\t", 0) ... };
    std::cout << std::endl;
}

template < typename ... ARGS >
void errorMsg(const std::string& msg, ARGS&& ... args)
{
    print(msg, args...);
    throw std::runtime_error(msg.c_str());
    exit(1);
}
void errorMsg(const std::string& msg);

//self explanatory...
int createFolder(std::string path);
bool fileExists(const std::string& path);
unsigned fileSize(const std::string& file);
unsigned fileSize(std::ifstream&
                  file);        //receives file directly and puts the pointer back to where it was after calculating size
unsigned fileSize_Beg(std::ifstream&
                      file);    //receives file directly and puts the pointer in the beginning after calculating size
std::string checkFileName(const std::string& name, const std::string& extension);
