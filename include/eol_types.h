#ifndef __EOL_TYPES__
#define __EOL_TYPES__
/*
    Copyright 2012 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_opengl.h"
#include <chipmunk/chipmunk_types.h>
#include <SDL.h>

/**
 * @purpose Since this engine is going to use openGL, SDL, and Chipmunk; which
 * all define their own numbering system, I will use this place to condense 
 * them into a common EOL space.
 */

/* the threshold for floating point accuracy*/
#define EOL_EPSILON   1e-6f

/* conversion factor for converting from radians to degrees*/
#define EOL_RADTODEG  57.295779513082

/* conversion factor for converting from degrees to radians*/
#define EOL_DEGTORAD  0.017453292519943295769236907684886
/* 
 * The Integers
 * the SDL suit handles cross platform integers very well
 */
typedef Uint8    eolUI8;
typedef Sint8    eolSI8;
typedef Uint32   eolUI32;
typedef Sint32   eolSI32;
typedef Uint64   eolUI64;
typedef Sint64   eolSI64;

typedef eolSI32  eolInt;
typedef eolUI32  eolUint;

typedef eolUI8   eolBool;

enum eolBoolEnum {
  eolTrue  = 1,
  eolFalse = 0
};

/*
 * The Floating point numbers 
*/
typedef GLfloat  eolFloat;
typedef GLdouble eolDouble;

/**
 * @brief Determines if the two floats are effectively equal within the
 * thresholds of floating point accuracry
 *
 * @param a double to compare
 * @param b other double to compare
 *
 * @return eolTrue is they are withing +/- of EOL_EPSILON of each other.
 * eolFalse otherwise.
 */
eolBool eol_equals(eolDouble a, eolDouble b);

/**
 * constant lenth character buffers.
 * These will be used when the convenience of GString is not needed
 */

/**
 * @brief 16 character buffer used for short tags
 */
typedef char eolWord[16];
#define EOLWORDLEN   16
/**
 * @brief 128 character buffer used for statements
 */
typedef char eolLine[128];
#define EOLLINELEN   128
/**
 * @brief 512 character buffer used for text blocks
 */
typedef char eolText[512];
#define EOLTEXTLEN   512

typedef SDL_Rect eolRect;

/**
 * @brief constructs and returns an eolRect
 * @param x component of the rect
 * @param y component of the rect
 * @param w component of the rect
 * @param h component of the rect
 */
eolRect eol_rect(
  eolInt  x,
  eolInt  y,
  eolUint w,
  eolUint h
);

typedef struct
{
	eolDouble x,y,w,h;
}eolRectFloat;
/*
 * The Vector Types
 * Not to be confused with the vector lists from STL
*/
typedef struct
{
	eolDouble x;
	eolDouble y;
}eolVec2D;

typedef struct
{
  eolDouble x;
  eolDouble y;
  eolDouble z;
}eolVec3D;

/**
 * @brief create and return an eolVec3D
 */
eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z);

/*
  Time Handling
*/
typedef eolUI32 eolTime;

/**
 * basic operations
 */
#ifndef MIN
#define MIN(a,b)          (a<=b?a:b)
#endif

#ifndef MAX
#define MAX(a,b)          (a>=b?a:b)
#endif

/**
 * 3D Vector Math
 * These macros handle most of the common operations for vector math.
 */
/**
 * @brief Macro to get the crossproduct from a vector.
 * varients ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D component of the dot product
 * @param b eolVect3D component of the dot product
 *
 * @return the calculated dot product
 */
#define eol_dot_product_3D(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z)
#define eol_dot_product_3D_p(a,b)     (a->x*b->x+a->y*b->y+a->z*b->z)
#define eol_dot_product_2D(a,b)      (a.x*b.x+a.y*b.y)
#define eol_dot_product_2D_p(a,b)     (a->x*b->x+a->y*b->y)

/**
 * @brief Macro to subtract two vectors
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D input
 * @param b eolVect3D input
 * @param c result eolVect3D output
 */
#define eol_vector_sub_3D(a,b,c)     (c.x=a.x-b.x,c.y=a.y-b.y,c.z=a.z-b.z)
#define eol_vector_sub_3D_p(a,b,c)     (c->x=a->x-b->x,c->y=a->y-b->y,c->z=a->z-b->z)
#define eol_vector_sub_2D(a,b,c)     (c.x=a.x-b.x,c.y=a.y-b.y)
#define eol_vector_sub_2D_p(a,b,c)     (c->x=a->x-b->x,c->y=a->y-b->y)

/**
 * @brief Macro to add two vectors
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D input
 * @param b eolVect3D input
 * @param c result eolVect3D output
 */
#define eol_vector_add_3D(a,b,c)     (c.x=a.x+b.x,c.y=a.y+b.y,c.z=a.z+b.z)
#define eol_vector_add_3D_p(a,b,c)     (c->x=a->x+b->x,c->y=a->y+b->y,c->z=a->z+b->z)
#define eol_vector_add_2D(a,b,c)     (c.x=a.x+b.x,c.y=a.y+b.y)
#define eol_vector_add_2D_p(a,b,c)     (c->x=a->x+b->x,c->y=a->y+b->y)

/**
 * @brief Macro to scale a vector by a scalar value
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D input
 * @Param b the scalar value to scale the vector by.
 * @param c result eolVect3D output
 */
#define eol_vector_scale_3D(a,b,c)  (c.x=a.x*b,c.y=a.y*b,c.z=a.z*b)
#define eol_vector_scale_3D_p(a,b,c)  (c->x=a->x*b,c->y=a->y*b,c->z=a->z*b)
#define eol_vector_scale_2D(a,b,c)  (c.x=a.x*b,c.y=a.y*b)
#define eol_vector_scale_2D_p(a,b,c)  (c->x=a->x*b,c->y=a->y*b)

/**
 * @brief Macro that sets vector to zero.
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D input
 */
#define eol_vector_clear_3D(a)       (a.x=a.y=a.z=0)
#define eol_vector_clear_3D_p(a)       (a->x=a->y=a->z=0)
#define eol_vector_clear_2D(a)       (a.x=a.y=0)
#define eol_vector_clear_2D_p(a)       (a->x=a->y=0)

/**
 * @brief Macro to get the negative of a vector
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolVect3D input
 * @param b eolVect3D negated output
 */
#define eol_vector_negate_3D(a,b)      (b.x=-a.x,b.y=-a.y,b.z=-a.z)
#define eol_vector_negate_3D_p(a,b)      (b->x=-a->x,b->y=-a->y,b->z=-a->z)
#define eol_vector_negate_2D(a,b)      (b.x=-a.x,b.y=-a.y)
#define eol_vector_negate_2D_p(a,b)      (b->x=-a->x,b->y=-a->y)

/**
 * @brief Macro to set the components of the vector
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param a eolFloat x component
 * @param b eolFloat y component
 * @param c eolFloat z componenta (only in 3D version)
 * @param v eolVect3D output
 */
#define eol_vector_set_3D(a, b, c, v)  (v.x=(a), v.y=(b), v.z=(c))
#define eol_vector_set_3D_p(a, b, c, v)  (v->x=(a), v->y=(b), v->z=(c))
#define eol_vector_set_2D(a, b, v)  (v.x=(a), v.y=(b))
#define eol_vector_set_2D_p(a, b, v)  (v->x=(a), v->y=(b))

/**
 * @brief normalizes the vector passed.  does nothing for a zero length vector.
 *
 * @param v pointer to the vector to be normalized.
 */
void eol_vector_normalize(eolVec3D *v);
eolFloat eol_vec3d_magnitude (eolVec3D V);
void eol_vec3d_normalize (eolVec3D *V);

#endif

