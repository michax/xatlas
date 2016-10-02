// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef NV_MATH_RANDOM_H
#define NV_MATH_RANDOM_H

#include <time.h>
#include "nvmath.h"
#include "nvcore/Utils.h" // nextPowerOfTwo


namespace nv
{
/// Mersenne twister random number generator.
class MTRand
{
public:

	enum time_e { Time };
	enum { N = 624 };       // length of state vector
	enum { M = 397 };

	/// Constructor that uses the current time as the seed.
	MTRand( time_e )
	{
		seed((uint)time(NULL));
	}

	/// Constructor that uses the given seed.
	MTRand( uint s = 0 )
	{
		seed(s);
	}

	/// Provide a new seed.
	void seed( uint s )
	{
		initialize(s);
		reload();
	}

	/// Get a random number between 0 - 65536.
	uint get()
	{
		// Pull a 32-bit integer from the generator state
		// Every other access function simply transforms the numbers extracted here
		if ( left == 0 ) {
			reload();
		}
		left--;
		uint s1;
		s1 = *next++;
		s1 ^= (s1 >> 11);
		s1 ^= (s1 <<  7) & 0x9d2c5680U;
		s1 ^= (s1 << 15) & 0xefc60000U;
		return ( s1 ^ (s1 >> 18) );
	};

	/// Get a random number on [0, max] interval.
	uint getRange( uint max )
	{
		if (max == 0) return 0;
		if (max == NV_UINT32_MAX) return get();
		const uint np2 = nextPowerOfTwo( max + 1 ); // @@ This fails if max == NV_UINT32_MAX
		const uint mask = np2 - 1;
		uint n;
		do {
			n = get() & mask;
		} while ( n > max );
		return n;
	}


private:

	void initialize( uint32 seed )
	{
		// Initialize generator state with seed
		// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
		// In previous versions, most significant bits (MSBs) of the seed affect
		// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
		uint32 *s = state;
		uint32 *r = state;
		int i = 1;
		*s++ = seed & 0xffffffffUL;
		for ( ; i < N; ++i ) {
			*s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
			r++;
		}
	}

	void reload()
	{
		// Generate N new values in state
		// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
		uint32 *p = state;
		int i;
		for ( i = N - M; i--; ++p )
			*p = twist( p[M], p[0], p[1] );
		for ( i = M; --i; ++p )
			*p = twist( p[M - N], p[0], p[1] );
		*p = twist( p[M - N], p[0], state[0] );
		left = N, next = state;
	}

	uint hiBit( uint u ) const
	{
		return u & 0x80000000U;
	}
	uint loBit( uint u ) const
	{
		return u & 0x00000001U;
	}
	uint loBits( uint u ) const
	{
		return u & 0x7fffffffU;
	}
	uint mixBits( uint u, uint v ) const
	{
		return hiBit(u) | loBits(v);
	}
	uint twist( uint m, uint s0, uint s1 ) const
	{
		return m ^ (mixBits(s0, s1) >> 1) ^ ((~loBit(s1) + 1) & 0x9908b0dfU);
	}

private:

	uint state[N];	// internal state
	uint *next;	// next value to get from state
	int left;		// number of values left before reload needed

};
} // nv namespace

#endif // NV_MATH_RANDOM_H
