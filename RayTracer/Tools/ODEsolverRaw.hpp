#ifndef _ODESOLVER_H_
#define _ODESOLVER_H_

#include "kokkos_macros.hpp"

// Class for resolution of systems of diferential equations
template <class Vec>
class ODEsolver
{
public:
    // list of available methods
    enum ODEmethod {
        Euler,
        Heun,
        RK2,
        RK4,
        RK45
    };

    // void *ptr last argument to be used as some object that needs to be passed
    // and recast inside the RHS function
    inline ODEsolver(Vec(*_RHS)(const Vec& vars, const Vec& params,
                                void* ptr),
                     unsigned _dim)
        : dim(_dim), RHS(_RHS), stopCriteria(nullptr), constraints(nullptr),
          N_params(0), params(nullptr), ptr(nullptr)
    {
        setMethod(RK4);
    }
    ~ODEsolver()
    {
        if(params!=nullptr)
            delete params;
    }

    // dt is changed in RK45, derr is only for RK45
    Vec step(const Vec& init, double& dt, double derr = 0.) const;

    // returns full history
    // derr is only for RK45
    MatrixD evolve(const Vec& init, double T, double dt, double derr = 0.) const;
    MatrixD evolve(ODEmethod met, const Vec& init, double T, double dt,
                   double derr = 0.01)
    {
        setMethod(met);
        return evolve(init, T, dt, derr);
    }
    // returns just last step
    Vec solve(const Vec& init, double T, double dt, double derr = 0.) const;
    Vec solve(ODEmethod met, const Vec& init, double T, double dt,
              double derr = 0.01)
    {
        setMethod(met);
        return solve(init, T, dt, derr);
    }

    void setMethod(ODEmethod met);
    inline void setStopCriteria(bool (*a_stopCriteria)(const Vec& vars,
                                const Vec& params,
                                void* ptr))
    {
        stopCriteria = a_stopCriteria;
    }

    inline void setConstraints(void (*a_constraints)(Vec& vars,
                               const Vec& params,
                               void* ptr))
    {
        constraints = a_constraints;
    }

    inline void setParams(const double* _params, unsigned N) {
        if(params != nullptr)
            delete params;
        
        params = new double[N];
        for (unsigned i = 0; i < N; ++i)
            params[i] = _params[i];
        N_params = N;
    }
    inline void setPointer(void* a_ptr) { ptr = a_ptr; }

    // third argument irrelevant, just applicable to RK45
    double* stepEuler(const double* i, double& dt, double derr = 0.) const;
    double* stepHeun(const double* i, double& dt, double derr = 0.) const;
    double* stepRK2(const double* i, double& dt, double derr = 0.) const;
    double* stepRK4(const double* i, double& dt, double derr = 0.) const;
    double* stepRK45(const double* i, double& dt, double derr) const;

    bool checkStop(double t, double T, double dt, const Vec& lastX) const;

private:
    unsigned dim;
    double* (*RHS)(const double* vars, const double* params, void* ptr);
    bool (*stopCriteria)(const double* vars, const double* params, void* ptr);
    void (*constraints)(double* vars, const double* params, void* ptr);

    unsigned N_params;
    double* params;
    void* ptr;

    typedef double* (ODEsolver::*mbFunc)(const double* init, double& dt,
                                    double derr) const;
    mbFunc stepFunc;
    ODEmethod method;
};

#endif