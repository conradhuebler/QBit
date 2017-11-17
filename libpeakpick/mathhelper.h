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


#include <cmath>
#include <iostream>
#include <vector>

typedef Eigen::VectorXd Vector;
static double pi = 3.14159265;

namespace PeakPick{

    inline double mean(const Vector &vector, int *min = NULL, int *max = NULL)
    {
        if(vector.size() == 0)
        {
            std::cout << "PeakPick::mean() vector is empty" << std::endl;
            return 0;
        }
        double sum = 0;
        double temp_min = vector(0);
        int pos_min = 0;
        double temp_max = vector(0);
        int pos_max = 0;
// #pragma omp parallel for shared(sum, temp_min, pos_min, temp_max, pos_max) reduction(+:sum, &:temp_min, &:pos_min, &:temp_max, &:pos_max)
        for(int i = 0; i < vector.size(); ++i)
        {
            sum += vector(i);
            if(vector(i) > temp_max)
            {
                temp_max = vector(i);
                pos_max = i;
            }
            if(vector(i) < temp_min)
            {
                temp_min = vector(i);
                pos_min = i;
            }
            
            if(max != NULL)
                *max = pos_max;
            
            if(min != NULL)
                *min = pos_min;
        }
        return sum/double(vector.size());
    }
    
    inline double meanThreshold(const Vector &vector, double threshold, int *min = NULL, int *max = NULL)
    {
        if(vector.size() == 0)
            return 0;
        
        double sum = 0;
        double temp_min = vector(0);
        int pos_min = 0;
        double temp_max = vector(0);
        int pos_max = 0;
        for(int i = 0; i < vector.size(); ++i)
        {
            if(std::abs(vector(i)) >= threshold)
                continue;
            sum += vector(i);
            if(vector(i) > temp_max)
            {
                temp_max = vector(i);
                pos_max = i;
            }
            if(vector(i) < temp_min)
            {
                temp_min = vector(i);
                pos_min = i;
            }
            
            if(max != NULL)
                *max = pos_max;
            
            if(min != NULL)
                *min = pos_min;
        }
        
        return sum/double(vector.size());
    }
    
    
    inline double stddev(const Vector &vector, double mean)
    {
        if(vector.size() == 0)
            return 0;
        
        double sum = 0;
#pragma omp parallel for reduction(+:sum)
        for(int i = 0; i < vector.size(); ++i)
        {
            sum += (vector(i) - mean)*(vector(i) - mean);
        }
        return sqrt(sum/double(vector.size()));
    }
    
    inline double stddevThreshold(const Vector &vector, double mean, double threshold)
    {
        if(vector.size() == 0)
            return 0;
        
        double sum = 0;
        
#pragma omp parallel for reduction(+:sum)
        for(int i = 0; i < vector.size(); ++i)
        {
            if(std::abs(vector(i)) >= threshold)
                continue;
            sum += (vector(i) - mean)* (vector(i) - mean);
        }
        return sqrt(sum/double(vector.size()));
    }

        
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
}

