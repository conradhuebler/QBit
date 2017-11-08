
/*
 * <Math containing Header file.>
 * Copyright (C) 2017  Conrad Hübler <Conrad.Huebler@gmx.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <unsupported/Eigen/NonLinearOptimization>

#include <cmath>
#include <iostream>
#include <vector>

#include "analyse.h"
#include "spectrum.h"

typedef Eigen::VectorXd Vector;

static double pi = 3.14159265;

namespace PeakPick{
    
    inline double Gaussian(double x, double a, double x_0, double c)
    {
        return 1/(a*sqrt(2*pi))*exp(-pow((x-x_0),2)/(2*pow(c,2)));
    }
    
    inline double Lorentzian(double x, double x_0, double gamma)
    {
        return 1/pi*(0.5*gamma)/(pow(x-x_0,2)+pow(0.5*gamma,2));
    }
    
    inline double Signal(double x, Vector parameter, int functions, double ratio = 0.9)
    {
        double signal = 0;
        for(int i = 0; i < functions; ++i)
        {
            double gaussian = Gaussian(x, parameter(1+i*5), parameter(0+i*5), parameter(2+i*5));
            double lorentzian = Lorentzian(x, parameter(0+i*5), parameter(3+i*5));
            signal += ((1-ratio)*gaussian + ratio*lorentzian)*parameter(4+i*5);
        }
        return signal;
    }
    
    template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
    
    struct Functor
    {
        typedef _Scalar Scalar;
        enum {
            InputsAtCompileTime = NX,
            ValuesAtCompileTime = NY
        };
        typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
        typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
        typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;
        
        int m_inputs, m_values;
        
        inline Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}
        
        int inputs() const { return m_inputs; }
        int values() const { return m_values; }
        
    };
    
    struct MyFunctor : Functor<double>
    {
        inline MyFunctor(int inputs, int values) : Functor(inputs, values), no_parameter(inputs),  no_points(values)
        {
            
        }
        inline ~MyFunctor() { }
        inline int operator()(const Eigen::VectorXd &parameter, Eigen::VectorXd &fvec) const
        {
            int j = 0;
            double err = 0;
//             for(int i = 0; i < guess.size(); ++i)
            {
//                 err += (guess(i)-parameter(0+i*5))*(guess(i)-parameter(0+i*5));
//                 std::cout << err << " " << guess(i) << " " << parameter(0+i*5) << std::endl;
            }
            for(int i = start; i <= end; ++i)
            {
                double x = spec->X(i);
                fvec(j) =  Signal(x, parameter, functions, ratio) - spec->Y(i) + err;
                ++j;
            }
            return 0;
        }
        int no_parameter;
        int no_points;
        int start, end;
        int functions;
        const spectrum *spec;
        Vector guess;
        int inputs() const { return no_parameter; } 
        int values() const { return no_points; } 
        double ratio;
    };
    
    struct MyFunctorNumericalDiff : Eigen::NumericalDiff<MyFunctor> {};
    
    
    Vector Deconvulate(const spectrum *spec, double start, double end, double ratio, const Vector &guess)
    {
        MyFunctor functor(5*guess.size(), end-start+1);
        functor.start = start;
        functor.end = end;
        functor.spec = spec;
        functor.ratio = ratio;
        functor.functions = guess.size(); 
        functor.guess = guess;
        Vector parameter(5*guess.size());
        for(int i = 0; i < guess.size(); ++i)
        {
            parameter(0+i*5) = guess(i);
            parameter(1+i*5) = 1;
            parameter(2+i*5) = 10;
            parameter(3+i*5) = 1/double(50);
            parameter(4+i*5) = 1/double(30);
        }
        
        Eigen::NumericalDiff<MyFunctor> numDiff(functor);
        Eigen::LevenbergMarquardt<Eigen::NumericalDiff<MyFunctor> > lm(numDiff);
        Eigen::LevenbergMarquardtSpace::Status status = lm.minimizeInit(parameter);
        lm.minimize(parameter);
        return parameter;
    }
}

