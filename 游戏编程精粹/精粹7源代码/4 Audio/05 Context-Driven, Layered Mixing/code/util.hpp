//----------------------------------------------------------------------------
// File: util.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef UTIL_HPP
#define UTIL_HPP

//----------------------------------------------------------------------------
// Define a simple assert
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define ASSERT( x ) if ( !( x ) ) { __asm { int 3 } }
#else
#define ASSERT( x ) ( ( void ) 0 )
#endif


#endif // UTIL_HPP
