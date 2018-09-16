/***************************************************************************
 *   Copyright (C) 2004 by Jacques Gasselin                                *
 *   jgasseli@student.rmit.edu.au                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LINEARPOLYNOMIAL_H
#define LINEARPOLYNOMIAL_H

/**
@author Jacques Gasselin
*/
#include <vector>
#include <valarray>

namespace mathglpp
{

template <typename T>
inline bool LP_almost_zero (T arg, T threshold) { return (std::abs(arg) <  std::abs(threshold)); }

template <typename T>
inline void LP_new_sample (T& arg1) { arg1+=T(1); }

template <typename T = double >
class LinearPolynomial
{
public:
    LinearPolynomial(size_t size)
    :fx(size),remainder(0)
    { }

    LinearPolynomial(const T& val, size_t size)
    :fx(val,size),remainder(0)
    { }

    LinearPolynomial(const T* p, size_t size)
    :fx(p,size),remainder(0)
    { }

    LinearPolynomial(const LinearPolynomial& lp)
    :fx(lp.fx),remainder(0)
    { }

    ~LinearPolynomial()
    { }

    inline T rem() const { return remainder; }
    inline size_t size() const { return fx.size(); }
    inline T& operator[] (size_t ind) { return fx[ind]; }

    inline T evaluate(T x) const
    {
        T sum = 0;
        for(register size_t i = 0; i < size(); ++i)
            sum += fx[i] * std::pow(x,i);
        return sum;
    }

    T operator() (T x) const
    {
        return evaluate(x);
    }


    LinearPolynomial& operator= (const LinearPolynomial& lp)
    {
        fx.upto_resize(lp.size());
        fx = lp.fx;
        return *this;
    }

    LinearPolynomial operator+ (const LinearPolynomial& lp) const
    {
        LinearPolynomial retval(*this);
        retval += lp;
        return retval;
    }

    LinearPolynomial& operator+= (const LinearPolynomial& lp)
    {
        assert(&lp);
        fx.upto_resize(lp.fx); //resize
        assert(fx.size() >= lp.size());
        fx += lp.fx;
        return *this;
    }

    LinearPolynomial operator- (const LinearPolynomial& lp) const
    {
        LinearPolynomial retval(*this);
        retval -= lp;
        return retval;
    }

    LinearPolynomial& operator-= (const LinearPolynomial& lp)
    {
        std::valarray<T> fx_old(fx);
        fx.resize(lp.fx);
        fx = fx_old;
        
        fx -= lp.fx;
        return *this;
    }

    inline LinearPolynomial operator* (T val) const
    {
        LinearPolynomial retval(*this);
        retval.fx *= val;
        return retval;
    }

    inline LinearPolynomial& operator*= (T val)
    {
        fx *= val;
        return *this;
    }

    LinearPolynomial operator* (const LinearPolynomial& lp) const
    {
        LinearPolynomial retval(T(0), (lp.size() + size() - 1));

        std::valarray<T> fx_old(fx);
        fx.resize(retval.size());
        fx = fx_old;


        for(register int i = 0; i < lp.size(); ++i)
        {
            retval.fx += fx.shift(-i) * lp.fx[i];
            //std::cout<<retval;
        };

        fx.resize(fx_old.size());
        fx = fx_old;
        
        return retval;
    }

    LinearPolynomial& operator*= (const LinearPolynomial& lp)
    {
    
        std::valarray<T> fx_old(fx);
        fx.resize(size()+lp.size()-1);
        fx = fx_old;

        LinearPolynomial temp(*this);

        fx = 0; //clear

        for(register size_t i = 0; i < lp.size(); ++i)
            fx += (temp.fx.shift(-i) * lp.fx[i]);

        return *this;
    }

    inline LinearPolynomial operator/ (T val) const
    {
        LinearPolynomial retval(*this);
        retval /= val;
        return val;
    }

    inline LinearPolynomial& operator/= (T val)
    {
        fx /= val;
        return *this;
    }

    LinearPolynomial operator/ (const LinearPolynomial& divisor) const
    {
        LinearPolynomial div(divisor);
        LinearPolynomial quot(*this);
        size_t diff = size() - div.size();
        size_t old_divSize = div.size();
        LinearPolynomial retval(T(0), 1 + diff);

        std::valarray<T> div_fx_old(div.fx);
        div.fx.resize(quot.fx);
        div.fx = div_fx_old;
        T res;

        for(register size_t i = 0; i <= diff; ++i)
        {
            res = quot.fx[quot.size()-1-i]/div.fx[old_divSize-1];
            quot.fx -= (div.fx.shift(i-diff) * res);
            retval.fx[diff-i] = res;
        };

        retval.remainder = quot[0];

        return retval;
    }

    LinearPolynomial& operator/= (const LinearPolynomial& divisor)
    {
        LinearPolynomial div(divisor);
        LinearPolynomial quot(*this);
        size_t diff = size() - div.size();
        size_t old_divSize = div.size();
        
        std::valarray<T> fx_old(fx);
        fx.resize(1+diff);
        fx = fx_old;

        
        std::valarray<T> div_fx_old(div.fx);
        div.fx.resize(quot.fx);
        div.fx = div_fx_old;

        T res;

        for(register size_t i = 0; i <= diff; ++i)
        {
            res = quot.fx[quot.size()-1-i]/div.fx[old_divSize-1];
            //std::cout << "res: "<<res<<", q.fx[]: "<<quot.fx[quot.size()-1-i]<<", d.fx[]: "<<div.fx[old_divSize-1]<<endl;
            std::valarray<T> sub(div.fx.shift(i-diff));
            //std::cout<< "sub: "<<sub;
            //std::cout<< "quot.fx: "<<quot.fx;
            sub*= res;
            //std::cout<< "sub * res: "<<sub;
            quot.fx -=  sub;
            //std::cout<< "quot.fx: "<<quot.fx;
            fx[diff-i] = res;
        }

        remainder = quot[0];

        return *this;
    }

    //!returns true if the root is found
    bool Newton_Raphson(const LinearPolynomial& derivative, T& sample, T threshold = 0.00000000001, int cutoff = 40)
    {
        //choose a sample point
        T root;
        T gradient = T(1);
        int i;

        for(i = 0; i < cutoff; ++i)
        {
            root = evaluate(sample);
            //std::cout << "root: "<< root <<", \tat: "<< sample << ", threshold: " << threshold << endl;
            if( LP_almost_zero(root,threshold) )
                break;//root found

            //find a guaranteed root as long as the gradient is not close to 0
            gradient = derivative.evaluate(sample);

            while( LP_almost_zero(gradient, T(0.0000001)) )
            {
                LP_new_sample(sample);
                gradient = derivative.evaluate(sample);
            }

            //recalculate sample position
            sample = sample - root/gradient;
        }

        sample = -(sample);

        if(i == cutoff)//complex roots, probably...
            return false;
        else
            return true;
    }

    //!returns false if the roots are complex, then root1 will be the median and root2 the discriminant
    //!otherwise the positive root is root1 and the negative root is root2, that is positive and negative
    //!translation from the median -b/2a.
    bool quadratic_formula(T& root1, T& root2)
    {
        //x = (-b +/- sqrt(b^2 - 4ac))/2a
        bool non_complex = true;
        T b = fx[1];
        T a2 = 2*fx[2];
        T c = fx[0];
        T discriminant = b*b - 2*a2*c;
        if( discriminant < T(0) )
        {
            non_complex = false;
            root1 = -b/a2;
            root2 = discriminant;
        }
        else
        {
            T sqrtDisc = sqrt(discriminant);
            root1 = -b/a2 + sqrtDisc;
            root2 = -b/a2 - sqrtDisc;
        }

        return non_complex;
    }

    std::vector<T> findRoots(void)
    {
        LinearPolynomial temp(*this);
        LinearPolynomial derivative(*this);

        std::vector<T> retval;
        retval.resize(size()-1);
        T root[] = { 0, 1 };
        register int i;

        for(i = 0; temp.size() > 3; ++i)
        {
            derivative = temp.derivate();
            temp.Newton_Raphson(derivative,root[0]);
            retval[i] = -root[0];
            //std::cout<<"root[]: "<<root[0]<<", "<<root[1]<<endl;
            //std::cout<<"temp, before: "<<temp;
            LinearPolynomial lp(root,2);
            //std::cout<<"divisor: "<<lp;
            temp /= lp;
            //std::cout<<"temp, after: "<<temp;
        };

        if(temp.size() > 2)
        {
            temp.quadratic_formula(retval[i], retval[i+1]);
            //!we should really do some complex root checking here
        }
        else
            retval[i] = -temp[0];

        return retval;
    }

    LinearPolynomial derivate(void)
    {//analytical derivative
        LinearPolynomial retval(T(0), (size() > 1 ? size()-1 : 1));
        for(register size_t i = 0; i < size()-1; ++i)
            retval.fx[i] = fx[i+1]*T((i+1));
        return retval;
    }

    LinearPolynomial integrate_with_root(T x_root)
    {//analytical integral
        LinearPolynomial retval(T(0), size() + 1);
        for(register int i = 1; i < size()+1; ++i)
            retval.fx[i] = fx[i-1]/T(i);
        //get the constant
        retval.fx[0] = -retval(x_root);

        return retval;
    }

    LinearPolynomial integrate_with_origin(T y_origin)
    {//analytical integral
        LinearPolynomial retval(T(0), size() + 1);
        for(register size_t i = 1; i < size()+1; ++i)
            retval.fx[i] = fx[i-1]/T(i);
        //get the constant
        retval.fx[0] = y_origin-retval(0);

        return retval;
    }

    template <class _CharT, class _Traits>
    friend std::basic_ostream<_CharT, _Traits>& operator <<
      ( std::basic_ostream<_CharT, _Traits>& out, const LinearPolynomial& lp)
    {
        out<<"y = ";
        for(register size_t i = lp.size()-1; i > 0; --i)
        {
            out<<lp.fx[i]<<"x^"<<i<<" + ";
        };
        if(lp.remainder == T(0))
            out<<lp.fx[0]<<std::endl;
        else
            out<<lp.fx[0]<<" & Remainder: "<<lp.remainder<<std::endl;

        return out;
    }

protected:
    std::valarray<T> fx;
    T remainder;
};

};
#endif
