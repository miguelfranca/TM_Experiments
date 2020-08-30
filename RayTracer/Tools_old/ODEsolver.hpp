#ifndef _ODESOLVER_H_
#define _ODESOLVER_H_

#include "Matrix.hpp"

// Class for resolution of systems of diferential equations
class ODEsolver
{
  public:
    // list of available methods
    enum ODEmethod
    {
        Euler,
        Heun,
        RK2,
        RK4,
        RK45
    };

    // void *ptr last argument to be used as some object that needs to be passed
    // and recast inside the RHS function
    inline ODEsolver(VecD (*_RHS)(const VecD &vars, const VecD &params,
                                  void *ptr),
                     unsigned _dim)
        : dim(_dim), RHS(_RHS), stopCriteria(nullptr), constraints(nullptr),
          params(0, 0.), ptr(nullptr)
    {
        setMethod(RK4);
    }

    bool checkData(const VecD &init) const;
    // dt is changed in RK45, derr is only for RK45
    VecD step(const VecD &init, double &dt, double derr = 0.) const;

    // returns full history
    // derr is only for RK45
    MatrixD evolve(const VecD &init, double T, double dt, double derr = 0.) const;
    MatrixD evolve(ODEmethod met, const VecD &init, double T, double dt,
                   double derr = 0.01)
    {
        setMethod(met);
        return evolve(init, T, dt, derr);
    }
    // returns just last step
    VecD solve(const VecD &init, double T, double dt, double derr = 0.) const;
    VecD solve(ODEmethod met, const VecD &init, double T, double dt,
               double derr = 0.01)
    {
        setMethod(met);
        return solve(init, T, dt, derr);
    }

    void setMethod(ODEmethod met);
    inline void setStopCriteria(bool (*a_stopCriteria)(const VecD &vars,
                                                       const VecD &params,
                                                       void *ptr))
    {
        stopCriteria = a_stopCriteria;
    }

    inline void setConstraints(void (*a_constraints)(VecD &vars,
                                                     const VecD &params,
                                                     void *ptr))
    {
        constraints = a_constraints;
    }

    inline void setParams(const VecD &_params) { params = _params; }
    inline void setPointer(void *a_ptr) { ptr = a_ptr; }

    // third argument irrelevant, just applicable to RK45
    VecD stepEuler(const VecD &i, double &dt, double derr = 0.) const;
    VecD stepHeun(const VecD &i, double &dt, double derr = 0.) const;
    VecD stepRK2(const VecD &i, double &dt, double derr = 0.) const;
    VecD stepRK4(const VecD &i, double &dt, double derr = 0.) const;
    VecD stepRK45(const VecD &i, double &dt, double derr) const;

    bool checkStop(double t, double T, double dt, const VecD &lastX) const;

  private:
    unsigned dim;
    VecD (*RHS)(const VecD &vars, const VecD &params, void *ptr);
    bool (*stopCriteria)(const VecD &vars, const VecD &params, void *ptr);
    void (*constraints)(VecD &vars, const VecD &params, void *ptr);
    VecD params;
    void *ptr;

    typedef VecD (ODEsolver::*mbFunc)(const VecD &init, double &dt,
                                      double derr) const;
    mbFunc stepFunc;
    ODEmethod method;
};

#endif