/*!
 * \file
 * \brief Definition of classes for random number generators
 * \author Tony Ottosson and Adam Piatyszek
 * 
 * $Date$
 * $Revision$
 *
 * -------------------------------------------------------------------------
 *
 * IT++ - C++ library of mathematical, signal processing, speech processing,
 *        and communications classes and functions
 *
 * Copyright (C) 1995-2007  (see AUTHORS file for a list of contributors)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * -------------------------------------------------------------------------
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <itpp/base/operators.h>
#include <ctime>


namespace itpp {

  //! \addtogroup randgen

  /*! 
   * \brief Base class for random (stochastic) sources.
   * \ingroup randgen
   *
   * The Random_Generator class is based on the MersenneTwister MTRand
   * class code in version 1.0 (15 May 2003) by Richard J. Wagner. See
   * http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html
   * for details.
   *
   * Here are the original release notes copied from the
   * \c MersenneTwister.h file:
   *
   * \verbatim
   * Mersenne Twister random number generator -- a C++ class MTRand Based on
   * code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus Richard J.
   * Wagner v1.0 15 May 2003 rjwagner@writeme.com
   *
   * The Mersenne Twister is an algorithm for generating random numbers. It
   * was designed with consideration of the flaws in various other generators.
   * The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
   * are far greater. The generator is also fast; it avoids multiplication and
   * division, and it benefits from caches and pipelines. For more information
   * see the inventors' web page at
   * http://www.math.keio.ac.jp/~matumoto/emt.html

   * Reference:
   * M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
   * Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions
   * on Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp. 
   * 3-30.
   *
   * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   * Copyright (C) 2000 - 2003, Richard J. Wagner
   * All rights reserved.                          
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions
   * are met:
   *
   *   1. Redistributions of source code must retain the above copyright
   *      notice, this list of conditions and the following disclaimer.
   *
   *   2. Redistributions in binary form must reproduce the above copyright
   *      notice, this list of conditions and the following disclaimer in the
   *      documentation and/or other materials provided with the distribution.
   *
   *   3. The names of its contributors may not be used to endorse or promote
   *      products derived from this software without specific prior written
   *      permission.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The original code included the following notice:
   *
   *     When you use this, send an email to: matumoto@math.keio.ac.jp with an
   *     appropriate reference to your work.
   *
   * It would be nice to CC: rjwagner@writeme.com and
   * Cokus@math.washington.edu when you write.
   * \endverbatim
   */
  class Random_Generator {
  public:
    //! Construct a new Random_Generator.    
    Random_Generator();
    //! Set the seed to a semi-random value (based on hashed time and clock).
    void randomize();
    //! Reset the source. The same sequance will be generated as the last time.
    void reset() { initialize(lastSeed); reload(); }
    //! Reset the source after setting the seed to seed.
    void reset(unsigned int seed) { lastSeed = seed; initialize(lastSeed); reload(); }
    //! Return a uniformly distributed [0,2^32-1] integer.
    unsigned int random_int()
    {
      if( left == 0 ) reload();
      --left;
		
      register unsigned int s1;
      s1 = *pNext++;
      s1 ^= (s1 >> 11);
      s1 ^= (s1 <<  7) & 0x9d2c5680U;
      s1 ^= (s1 << 15) & 0xefc60000U;
      return ( s1 ^ (s1 >> 18) );
    }

    //! Return a uniformly distributed (0,1) value.
    double random_01() { return (double(random_int()) + 0.5) * (1.0/4294967296.0); }
    //! Return a uniformly distributed [0,1) value.
    double random_01_lclosed() { return double(random_int()) * (1.0/4294967296.0); }
    //! Return a uniformly distributed [0,1] value.
    double random_01_closed() { return double(random_int()) * (1.0/4294967295.0); }
    //! Save current full state of generator in memory
    void get_state(ivec &out_state);
    //! Resume the state saved in memory. Clears memory.
    void set_state(ivec &new_state);
  protected:
  private:
    //! seed used for initialisation
    unsigned int lastSeed;
    //! internal state
    static unsigned int state[624];
    //! next value to get from state
    static unsigned int *pNext;
    //! number of values left before reload needed
    static int left;

    //!
    void initialize( unsigned int seed )
    {
      // Initialize generator state with seed
      // See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
      // In previous versions, most significant bits (MSBs) of the seed affect
      // only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
      register unsigned int *s = state;
      register unsigned int *r = state;
      register int i = 1;
      *s++ = seed & 0xffffffffU;
      for( ; i < 624; ++i ) 
	{
	  *s++ = ( 1812433253U * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffU;
	  r++;
	}
    }
    //!
    void reload()
    {
      // Generate N new values in state
      // Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
      register unsigned int *p = state;
      register int i;
      for( i = 624 - 397; i--; ++p )
	*p = twist( p[397], p[0], p[1] );
      for( i = 397; --i; ++p )
	*p = twist( p[397-624], p[0], p[1] );
      *p = twist( p[397-624], p[0], state[0] );
      
      left = 624, pNext = state;
    }
    //!
    unsigned int hiBit( const unsigned int& u ) const { return u & 0x80000000U; }
    //!
    unsigned int loBit( const unsigned int& u ) const { return u & 0x00000001U; }
    //!
    unsigned int loBits( const unsigned int& u ) const { return u & 0x7fffffffU; }
    //!
    unsigned int mixBits( const unsigned int& u, const unsigned int& v ) const
    { return hiBit(u) | loBits(v); }

    /*
     * ----------------------------------------------------------------------
     * --- ediap - 2007/01/17 ---
     * ----------------------------------------------------------------------
     * Richard's implementation of the twist() function is was follows:
     *  { return m ^ (mixBits(s0,s1)>>1) ^ (-loBit(s1) & 0x9908b0dfU); }
     * However, this code caused a warning/error under MSVC++, because
     * unsigned value loBit(s1) is being negated with `-' (c.f. bug report
     * [1635988]). I changed this to the same implementation as is used in
     * original C sources of Mersenne Twister RNG:
     *  #define MATRIX_A 0x9908b0dfUL
     *  #define UMASK 0x80000000UL
     *  #define LMASK 0x7fffffffUL
     *  #define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
     *  #define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL)) 
     * ----------------------------------------------------------------------
     */
    //!
    unsigned int twist( const unsigned int& m, const unsigned int& s0,
			const unsigned int& s1 ) const
    { return m ^ (mixBits(s0,s1)>>1) ^ (loBit(s1) ? 0x9908b0dfU : 0U); }
    //!
    unsigned int hash( time_t t, clock_t c );
  };


  //! \addtogroup randgen
  //!@{

  //! Set the seed of the Global Random Number Generator
  void RNG_reset(unsigned long seed);
  //! Set the seed of the Global Random Number Generator to the same as last reset/init
  void RNG_reset();
  //! Set a random seed for the Global Random Number Generator.
  void RNG_randomize();
  //! Save current full state of generator in memory
  void RNG_get_state(ivec &state);
  //! Resume the state saved in memory
  void RNG_set_state(ivec &state);
  //!@}

  /*! 
    \brief Bernoulli distribution
    \ingroup randgen
  */
  class Bernoulli_RNG {
  public:
    //! Binary source with probability prob for a 1
    Bernoulli_RNG(double prob) { setup(prob); }
    //! Binary source with probability prob for a 1
    Bernoulli_RNG() { p=0.5; }
    //! set the probability
    void setup(double prob)
    {
      it_assert(prob>=0.0 && prob<=1.0, "The bernoulli source probability must be between 0 and 1");
      p = prob;
    }
    //! return the probability
    double get_setup() const { return p; }
    //! Get one sample.
    bin operator()() { return sample(); }
    //! Get a sample vector.
    bvec operator()(int n) { bvec temp(n); sample_vector(n, temp); return temp; }
    //! Get a sample matrix.
    bmat operator()(int h, int w) { bmat temp(h, w); sample_matrix(h, w, temp); return temp; }
    //! Get a sample
    bin sample() { return bin( RNG.random_01() < p ? 1 : 0 ); }
    //! Get a sample vector.
    void sample_vector(int size, bvec &out)
    {
      out.set_size(size, false);
      for (int i=0; i<size; i++) out(i) = sample(); 
    }
    //! Get a sample matrix.
    void sample_matrix(int rows, int cols, bmat &out)
    {
      out.set_size(rows, cols, false);
      for (int i=0; i<rows*cols; i++) out(i) = sample();
    }
  protected:
  private:
    //!
    double p;
    //!
    Random_Generator RNG;
  };

  /*! 
    \brief Integer uniform distribution
    \ingroup randgen
  
    Example: Generation of random uniformly distributed integers in the interval [0,10].
    \code
    #include "itpp/sigproc.h"
  
    int main() {
  
    I_Uniform_RNG gen(0, 10);
  
    cout << gen() << endl; // prints a random integer
    cout << gen(10) << endl; // prints 10 random integers
    }
    \endcode
  */
  class I_Uniform_RNG {
  public:
    //! constructor. Sets min and max values.
    I_Uniform_RNG(int min=0, int max=1);
    //! set min and max values
    void setup(int min, int max);
    //! get the parameters
    void get_setup(int &min, int &max) const;
    //! Get one sample.
    int operator()() { return sample(); }
    //! Get a sample vector.
    ivec operator()(int n);
    //! Get a sample matrix.
    imat operator()(int h, int w);
    //! Return a single value from this random generator
    int sample() { return ( floor_i(RNG.random_01() * (hi - lo + 1)) + lo ); }
  protected:
  private:
    //!
    int lo;
    //!
    int hi;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief Uniform distribution
    \ingroup randgen
  */
  class Uniform_RNG {
  public:
    //! Constructor. Set min, max and seed.
    Uniform_RNG(double min=0, double max=1.0);
    //! set min and max
    void setup(double min, double max);
    //! get parameters
    void get_setup(double &min, double &max) const;
    //! Get one sample.
    double operator()() { return ( sample()* (hi_bound - lo_bound) + lo_bound ); }
    //! Get a sample vector.
    vec operator()(int n)
    { vec temp(n); sample_vector(n, temp); return (temp *(hi_bound - lo_bound) + lo_bound); }
    //! Get a sample matrix.
    mat operator()(int h, int w)
    { mat temp(h,w); sample_matrix(h,w, temp); return (temp *(hi_bound - lo_bound) + lo_bound); }
    //! Get a Uniformly distributed (0,1) sample 
    double sample() {  return RNG.random_01(); }
    //! Get a Uniformly distributed (0,1) vector
    void sample_vector(int size, vec &out)
    {
      out.set_size(size, false);
      for (int i=0; i<size; i++) out(i) = sample(); 
    }
    //! Get a Uniformly distributed (0,1) matrix
    void sample_matrix(int rows, int cols, mat &out)
    {
      out.set_size(rows, cols, false);
      for (int i=0; i<rows*cols; i++) out(i) = sample(); 
    }
  protected:
  private:
    //!
    double lo_bound, hi_bound;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief Exponential distribution
    \ingroup randgen
  */
  class Exponential_RNG {
  public:
    //! constructor. Set lambda.
    Exponential_RNG(double lambda=1.0);
    //! Set lambda
    void setup(double lambda) { l=lambda; }
    //! get lambda
    double get_setup() const;
    //! Get one sample.
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);
  protected:
  private:
    //!
    double sample() {  return ( -std::log(RNG.random_01()) / l ); }
    //!
    double l;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief Normal distribution
    \ingroup randgen
  */
  class Normal_RNG {
  public:
    //! Constructor. Set mean and variance.
    Normal_RNG(double meanval, double variance) { setup(meanval, variance); };
    //! Constructor. Set mean and variance.
    Normal_RNG() { mean = 0.0; sigma = 1.0; odd = false; }
    //! Set mean, and variance
    void setup(double meanval, double variance)
    { mean = meanval; sigma = std::sqrt(variance); odd = false; }
    //! Get mean and variance
    void get_setup(double &meanval, double &variance) const;
    //! Get one sample.
    double operator()() { return (sigma*sample()+mean); }
    //! Get a sample vector.
    vec operator()(int n) { vec temp(n); sample_vector(n, temp); return (sigma*temp+mean); }
    //! Get a sample matrix.
    mat operator()(int h, int w) { mat temp(h,w); sample_matrix(h,w, temp); return (sigma*temp+mean); }
    //! Get a Normal distributed (0,1) sample
    double sample() // Box-Mueller, Polar version, See Knuth v2, 3rd ed, p122 
    {
      double x, r2;
      if (odd) {
	odd = 0;
	return y;
      }
      do { x = 2. * RNG.random_01() - 1.; y = 2. * RNG.random_01() - 1.; r2 = x*x + y*y; } while (r2 >= 1. || r2 < 1E-30);
      r2 = std::sqrt(std::log(r2)*(-2./r2)); x *= r2;  y *= r2; odd = 1;
      return x;
    }
 
    //! Get a Normal distributed (0,1) vector
    void sample_vector(int size, vec &out)
    {
      out.set_size(size, false);
      for (int i=0; i<size; i++) out(i) = sample(); 
    }

    //! Get a Normal distributed (0,1) matrix
    void sample_matrix(int rows, int cols, mat &out)
    {
      out.set_size(rows, cols, false);
      for (int i=0; i<rows*cols; i++) out(i) = sample(); 
    }
  protected:
  private:
    //!
    double mean, sigma, y;
    //!
    int odd;
    //!
    Random_Generator RNG;
  };

  /*! 
    \brief Laplacian distribution
    \ingroup randgen
  */
  class Laplace_RNG {
  public:
    //! Constructor. Set mean and variance.
    Laplace_RNG(double meanval=0.0, double variance=1.0);
    //! Set mean and variance
    void setup(double meanval, double variance);
    //! Get mean and variance
    void get_setup(double &meanval, double &variance) const;
    //! Get one sample.
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);
    //! Returns a single sample
    double sample()
    {
      u=RNG.random_01();
      if(u<0.5)
	l=std::log(2.0*u);
      else
	l=-std::log(2.0*(1-u));
      
      l *= std::sqrt(var/2.0);
      l += mean;
      
      return l;
    }
  protected:
  private:
    //!
    double mean, var, u, l;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief A Complex Normal Source
    \ingroup randgen
  */
  class Complex_Normal_RNG {
  public:
    //! Constructor. Set mean and variance.
    Complex_Normal_RNG(std::complex<double> mean, double variance) { setup(mean, variance); }
    //! Constructor. Set mean and variance.
    Complex_Normal_RNG() { m = 0.0; sigma=1.0; }
    //! Set mean and variance
    void setup(std::complex<double> mean, double variance) { m = mean; sigma = std::sqrt(variance); }
    //! Get mean and variance
    void get_setup(std::complex<double> &mean, double &variance) { mean = m; variance = sigma*sigma; }
    //! Get one sample.
    std::complex<double> operator()() { return sigma*sample()+m; }
    //! Get a sample vector.
    cvec operator()(int n) { cvec temp(n); sample_vector(n, temp); return (sigma*temp+m); }
    //! Get a sample matrix.
    cmat operator()(int h, int w) { cmat temp(h,w); sample_matrix(h,w, temp); return (sigma*temp+m); }
    //! Get a Complex Normal (0,1) distributed sample
    std::complex<double> sample()
    {
      double a = m_2pi * RNG.random_01();
      double b = std::sqrt(-std::log(RNG.random_01()));
      return std::complex<double>(b * std::cos(a), b * std::sin(a));
    }

    //! Get a Complex Normal (0,1) distributed vector
    void sample_vector(int size, cvec &out)
    {
      out.set_size(size, false);
      for (int i=0; i<size; i++) out(i) = sample(); 
    }

    //! Get a Complex Normal (0,1) distributed matrix
    void sample_matrix(int rows, int cols, cmat &out)
    {
      out.set_size(rows, cols, false);
      for (int i=0; i<rows*cols; i++) out(i) = sample(); 
    }
  protected:
  private:
    //!
    std::complex<double> m;
    //!
    double sigma;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief Filtered normal distribution
    \ingroup randgen
  */
  class AR1_Normal_RNG {
  public:
    //! Constructor. Set mean, variance, and correlation.
    AR1_Normal_RNG(double meanval=0.0, double variance=1.0, double rho=0.0);
    //! Set mean, variance, and correlation
    void setup(double meanval, double variance, double rho);
    //! Get mean, variance and correlation
    void get_setup(double &meanval, double &variance, double &rho) const;
    //! Set memory contents to zero
    void reset();
    //! Get a single random sample
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);

  protected:
  private: 
    //!
    double sample();
    //!
    double my_mean, mem, r, factr, mean, var, r1, r2;
    //!
    bool odd;
    //!
    Random_Generator RNG;
  };

  /*!
    \brief Gauss_RNG is the same as Normal Source
    \ingroup randgen
  */
  typedef Normal_RNG Gauss_RNG;

  /*! 
    \brief AR1_Gauss_RNG is the same as AR1_Normal_RNG
    \ingroup randgen
  */
  typedef AR1_Normal_RNG AR1_Gauss_RNG;

  /*!
    \brief Weibull distribution
    \ingroup randgen
  */
  class Weibull_RNG {
  public:
    //! Constructor. Set lambda and beta.
    Weibull_RNG(double lambda=1.0, double beta=1.0);

    //! Set lambda, and beta
    void setup(double lambda, double beta);
    //! Get lambda and beta
    void get_setup(double &lambda, double &beta) { lambda=l; beta=b; }
    //! Get one sample.
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);
  protected:
  private:
    //!
    double sample();
    //!
    double l, b;
    //!
    double mean, var;
    //!
    Random_Generator RNG;
  };

  /*! 
    \brief Rayleigh distribution
    \ingroup randgen
  */
  class Rayleigh_RNG {
  public:
    //! Constructor. Set sigma.
    Rayleigh_RNG(double sigma=1.0);

    //! Set sigma
    void setup(double sigma) { sig=sigma; }
    //! Get sigma
    double get_setup() { return sig; }
    //! Get one sample.
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);
  protected:
  private:
    //!
    double sample();
    //!
    double sig;
    //!
    Random_Generator RNG;
  };

  /*! 
    \brief Rice distribution
    \ingroup randgen
  */
  class Rice_RNG {
  public:
    //! Constructor. Set sigma, and s.
    Rice_RNG(double sigma=1.0, double _s=1.0);
    //! Set sigma, and s
    void setup(double sigma, double _s) { sig=sigma; s=_s; }
    //! Get parameters
    void get_setup(double &sigma, double &_s) { sigma=sig; _s=s; }
    //! Get one sample.
    double operator()() { return sample(); }
    //! Get a sample vector.
    vec operator()(int n);
    //! Get a sample matrix.
    mat operator()(int h, int w);
  protected:
  private:
    //!
    double sample();
    //!
    double sig, s;
    //!
    Random_Generator RNG;
  };

  //! \addtogroup randgen
  //!@{

  //! Generates a random bit (equally likely 0s and 1s)
  inline bin randb(void) { Bernoulli_RNG src; return src.sample(); }
  //! Generates a random bit vector (equally likely 0s and 1s)
  inline void randb(int size, bvec &out) { Bernoulli_RNG src; src.sample_vector(size, out); }
  //! Generates a random bit vector (equally likely 0s and 1s)
  inline bvec randb(int size) { bvec temp; randb(size, temp); return temp; }
  //! Generates a random bit matrix (equally likely 0s and 1s)
  inline void randb(int rows, int cols, bmat &out) { Bernoulli_RNG src; src.sample_matrix(rows, cols, out); }
  //! Generates a random bit matrix (equally likely 0s and 1s)
  inline bmat randb(int rows, int cols){ bmat temp; randb(rows, cols, temp); return temp; }

  //! Generates a random uniform (0,1) number
  inline double randu(void) { Uniform_RNG src; return src.sample(); }
  //! Generates a random uniform (0,1) vector
  inline void randu(int size, vec &out) { Uniform_RNG src; src.sample_vector(size, out); }
  //! Generates a random uniform (0,1) vector
  inline vec randu(int size){ vec temp; randu(size, temp); return temp; }
  //! Generates a random uniform (0,1) matrix
  inline void randu(int rows, int cols, mat &out) { Uniform_RNG src; src.sample_matrix(rows, cols, out); }
  //! Generates a random uniform (0,1) matrix
  inline mat randu(int rows, int cols) { mat temp; randu(rows, cols, temp); return temp; }

  //! Generates a random integer in the interval [low,high]
  inline int randi(int low, int high) { I_Uniform_RNG src; src.setup(low, high); return src(); }
  //! Generates a random ivec with elements in the interval [low,high] 
  inline ivec randi(int size, int low, int high) { I_Uniform_RNG src; src.setup(low, high); return src(size); }
  //! Generates a random imat with elements in the interval [low,high] 
  inline imat randi(int rows, int cols, int low, int high) { I_Uniform_RNG src; src.setup(low, high); return src(rows, cols); }

  //! Generates a random Rayleigh vector
  inline vec randray(int size, double sigma = 1.0) { Rayleigh_RNG src; src.setup(sigma); return src(size); }

  //! Generates a random Rice vector (See J.G. Poakis, "Digital Communications, 3rd ed." p.47)
  inline vec randrice(int size, double sigma = 1.0, double s = 1.0) { Rice_RNG src; src.setup(sigma, s); return src(size); }

  //! Generates a random complex Gaussian vector
  inline vec randexp(int size, double lambda = 1.0) { Exponential_RNG src; src.setup(lambda); return src(size); }

  //! Generates a random Gaussian (0,1) variable
  inline double randn(void) { Normal_RNG src; return src.sample(); }
  //! Generates a random Gaussian (0,1) vector
  inline void randn(int size, vec &out) { Normal_RNG src; src.sample_vector(size, out); }
  //! Generates a random Gaussian (0,1) vector
  inline vec randn(int size) { vec temp; randn(size, temp); return temp; }
  //! Generates a random Gaussian (0,1) matrix
  inline void randn(int rows, int cols, mat &out)  { Normal_RNG src; src.sample_matrix(rows, cols, out); }
  //! Generates a random Gaussian (0,1) matrix
  inline mat randn(int rows, int cols){ mat temp; randn(rows, cols, temp); return temp; }

  /*! \brief Generates a random complex Gaussian (0,1) variable
    
  The real and imaginary parts are independent and have variances equal to 0.5
  */
  inline std::complex<double> randn_c(void) { Complex_Normal_RNG src; return src.sample(); }
  //! Generates a random complex Gaussian (0,1) vector
  inline void randn_c(int size, cvec &out)  { Complex_Normal_RNG src; src.sample_vector(size, out); }
  //! Generates a random complex Gaussian (0,1) vector
  inline cvec randn_c(int size){ cvec temp; randn_c(size, temp); return temp; }
  //! Generates a random complex Gaussian (0,1) matrix
  inline void randn_c(int rows, int cols, cmat &out) { Complex_Normal_RNG src; src.sample_matrix(rows, cols, out); }
  //! Generates a random complex Gaussian (0,1) matrix
  inline cmat randn_c(int rows, int cols) { cmat temp; randn_c(rows, cols, temp); return temp; }

  //!@}

  // -------------------- INLINES ----------------------------------------------

  inline double AR1_Normal_RNG::sample()
  {
    double s;

    if (odd) {
      r1 = RNG.random_01();
      r2 = RNG.random_01();
      s = std::sqrt(factr * std::log(r2)) * std::cos(m_2pi * r1);
    } else
      s = std::sqrt(factr * std::log(r2)) * std::sin(m_2pi * r1);

    odd = !odd;
  
    mem = s + r * mem;
    s = mem + mean;
  
    return s;
  }

  inline double Weibull_RNG::sample()
  {
    return ( std::pow(-std::log(RNG.random_01()), 1.0/b) / l );
  }

  inline double Rayleigh_RNG::sample()
  {
    double r1, r2;
    double s1, s2, samp;

    r1 = RNG.random_01();
    r2 = RNG.random_01();
    s1 = std::sqrt(-2.0 * std::log(r2)) * std::cos(m_2pi * r1);
    s2 = std::sqrt(-2.0 * std::log(r2)) * std::sin(m_2pi * r1);
    // s1 and s2 are N(0,1) and independent
    samp = sig * std::sqrt(s1*s1 + s2*s2);
     
    return samp;
  }

  inline double Rice_RNG::sample()
  {
    double r1, r2;
    double s1, s2, samp;
    double m1 = 0.0;
    double m2 = std::sqrt(s*s - m1*m1);

    r1 = RNG.random_01();
    r2 = RNG.random_01();
    s1 = std::sqrt(-2.0 * std::log(r2)) * std::cos(m_2pi * r1);
    s2 = std::sqrt(-2.0 * std::log(r2)) * std::sin(m_2pi * r1);
    // s1 and s2 are N(0,1) and independent
    samp = std::sqrt((sig*s1+m1) * (sig*s1+m1) + (sig*s2+m2) * (sig*s2+m2));
      
    return samp;
  }

} // namespace itpp

#endif // #ifndef RANDOM_H
