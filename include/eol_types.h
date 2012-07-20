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
#include <chipmunk/chipmunk.h>
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

#define random()  ((rand () & 0x7fff) / ((eolFloat)0x7fff))
#define crandom() (2.0 * (random() - 0.5))

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
#define eol_word_cmp(a,b) (strncmp(a,b,EOLWORDLEN))
#define eol_word_cpy(dst,src) (strncpy(dst,src,EOLWORDLEN))
/**
 * @brief 128 character buffer used for statements
 */
typedef char eolLine[128];
#define EOLLINELEN   128
#define eol_line_cmp(a,b) (strncmp(a,b,EOLLINELEN))
#define eol_line_cpy(dst,src) (strncpy(dst,src,EOLLINELEN))
/**
 * @brief 512 character buffer used for text blocks
 */
typedef char eolText[512];
#define EOLTEXTLEN   512
#define eol_text_cmp(a,b) (strncmp(a,b,EOLTEXTLEN))
#define eol_text_cpy(dst,src) (strncpy(dst,src,EOLTEXTLEN))

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

/**
 * @brief copies rect data from s into d
 *
 * @param d pointer to destination rect
 * @param s source rect
 */
void eol_rect_copy(
    eolRect * d,
    eolRect   s
  );

typedef struct
{
	eolDouble x,y,w,h;
}eolRectFloat;

/**
 * @brief constructs and returns an eolRectFloat
 * @param x component of the rect
 * @param y component of the rect
 * @param w component of the rect
 * @param h component of the rect
 */
eolRectFloat eol_rectf(
  eolDouble  x,
  eolDouble  y,
  eolDouble  w,
  eolDouble  h
);


/**
* @brief copies float rect data from s into d
*
* @param d pointer to destination rect
* @param s source rect
*/
void eol_rectf_copy(
    eolRectFloat * d,
    eolRectFloat   s
  );

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

#define eol_vec_in_rect(v, r)  ((v.x >= r.x)&&(v.x < (r.x + r.w))&&(v.y >= r.y)&&(v.y < (r.y + r.h)))

/**
 * @brief create and return an eolVec3D
 */
eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z);

/*
  Time Handling
*/
typedef eolUI32 eolTime;

/*
  3d orientation
*/

typedef struct
{
  eolVec3D position;
  eolVec3D scale;
  eolVec3D rotation;
  eolVec3D color;
  eolFloat alpha;
}eolOrientation;

void eol_orientation_add(eolOrientation * out,
                         eolOrientation   in1,
                         eolOrientation   in2);

/**
 * @brief sets the passed orientation to a default set up.
 * sets alpha and scale to 1, all else to zero
 */
void eol_orientation_clear(eolOrientation *out);

/*
  3d trails.
*/
typedef struct
{
  eolUint         maxLen;
  eolUint         len;
  eolOrientation *trail;
  eolOrientation *head;
  eolOrientation *tail;
}eolTrail;

/**
 * @brief allocated trail data for the trail provided
 *        does not allocate the structure itself, just the data used by it.
 * @param trail a pointer to a trail struct.  if NULL, nothing happens
 * @param maxLen how large a trail is allocated.
 */
void eol_trail_new(eolTrail *trail,eolUint maxLen);

/**
 * @brief frees allocated trail data.  resets the trail struct to zero
 *        does not free the trail structure iself, just the data allocated within it.
 * @param trail a pointer to a trail struct.  if NULL, it does nothing
 */
void eol_trail_free(eolTrail *trail);

/**
 * @brief adds a new orientation to the head of the trail, pushing all others back.
 *
 * @param trail the trail to add an orientation to
 * @param orientation the new orientation to add.
 */
void eol_trail_append(eolTrail *trail,eolOrientation orientation);

/**
* @brief retrieves the orientation n spots behind the head.
*
* @param trail the trail to search through
* @param n how many positions behind head to retrieve.  if n is beyond maxLen, it will fail
* @param ori output paramater.  Must provide a valid orientation pointer or function does nothing
* @return eolTrue if it found the nth orientation or eolFalse on error.
*/
eolBool eol_trail_get_nth(eolTrail *trail, eolUint n, eolOrientation *ori);

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

#define eol_vec3d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z)
#define eol_vec2d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y)

/**
 * @brief Macro to get the crossproduct from a vector.
 *
 * @param a eolVect3D component of the dot product
 * @param b eolVect3D component of the dot product
 *
 * @return the calculated dot product
 */
#define eol_vec3d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z)
#define eol_vec2d_dot_product(a,b)      (a.x*b.x+a.y*b.y)

/**
 * @brief copies the data from one vector into another
 *
 * @param dst the destination vector
 * @param src the source vector
 */
#define eol_vec3d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z)

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
 * @param v eolVect3D output
 * @param a eolFloat x component
 * @param b eolFloat y component
 * @param c eolFloat z componenta (only in 3D version)
 */
#define eol_vec3d_set(v, a, b, c)  (v.x=(a), v.y=(b), v.z=(c))
#define eol_vector_set_3D(v, a, b, c)  (v.x=(a), v.y=(b), v.z=(c))
#define eol_vector_set_2D(v, a, b)  (v.x=(a), v.y=(b))

/**
 * @brief normalizes the vector passed.  does nothing for a zero length vector.
 *
 * @param v pointer to the vector to be normalized.
 */
void eol_vector_normalize(eolVec3D *v);
eolFloat eol_vec3d_magnitude(eolVec3D V);
void eol_vec3d_normalize (eolVec3D *V);

/**
 * @brief returns the magnitude squared, which is faster than getting the magnitude
 * which would involve taking the square root of a floating point number.
 * @param V the vector to get the magnitude for
 * @return the square of the magnitude of V
 */
eolFloat eol_vec3d_magnitude_squared(eolVec3D V);

/**
 * @brief checks if the magnitude of V is less than size.  It does this without
 * doing square roots, which are costly.  It will still do floating point multiplication
 * @param V the vector to check
 * @param size the magnitude to check against
 * @return eolTrue if the magnitude of V is less than size or eolFalse otherwise
 */
eolBool  eol_vec3d_magnitude_less_than(eolVec3D V,eolFloat size);

/**
 * @brief checks if the distance between the two points provided is less than size.
 * @param p1 one point for the distance check
 * @param p2 another point for the distance check
 * @param size the value to check against
 * @return eolTrue if the distance between P1 and P2 is less than size, false otherwise
 */
eolBool eol_distance_between_less_than(eolVec3D p1,eolVec3D p2,eolFloat size);

#endif

