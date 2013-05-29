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

#define EOL_2PI 6.283185308

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

/**
* @brief converts a string represention of true/false to the value
* accepts "true", "TRUE" and "True" / "false", "FALSE" and "False"
* @param truth the string to check for "true" or "false"
* @return eolTrue if "true" or eolFalse if "false",-1 if nothing
*/
eolInt eol_bool_from_string(char * truth);

/**
* @brief checks if a string says "true"
* accepts "true", "TRUE" and "True"
* @param truth the string to check for "true"
* @return eolTrue if "true" or eolFalse if anything else
*/
eolUint eol_true_from_string(char * truth);

/**
* @brief checks if a string says "false"
* accepts "false", "FALSE" and "False"
* @param truth the string to check for "false"
* @return eolFalse if "false" eolTrue otherwise
*/
eolUint eol_false_from_string(char * truth);

char *eol_string_from_bool(eolBool b);

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
#define eol_word_clear(a)  (memset(a,0,sizeof(char)*EOLWORDLEN))
/**
 * @brief 128 character buffer used for statements
 */
typedef char eolLine[128];
#define EOLLINELEN   128
#define eol_line_cmp(a,b) (strncmp(a,b,EOLLINELEN))
#define eol_line_cpy(dst,src) (strncpy(dst,src,EOLLINELEN))
#define eol_line_clear(a)  (memset(a,0,sizeof(char)*EOLLINELEN))
/**
 * @brief 512 character buffer used for text blocks
 */
typedef char eolText[512];
#define EOLTEXTLEN   512
#define eol_text_cmp(a,b) (strncmp(a,b,EOLTEXTLEN))
#define eol_text_cpy(dst,src) (strncpy(dst,src,EOLTEXTLEN))
#define eol_text_clear(a)  (memset(a,0,sizeof(char)*EOLTEXTLEN))

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

void eol_rectf_set(
    eolRectFloat * d,
    eolFloat       x,
    eolFloat       y,
    eolFloat       w,
    eolFloat       h
  );

/**
 * @brief checks if the two rects are overlapping
 * @param a one of the rects to check
 * @param b the other rect to check
 * @return eolTrue if they overlap, eolFalse otherwise
 */
eolBool eol_rect_lap_rect(eolRect a, eolRect b);

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

typedef struct
{
  eolDouble x;
  eolDouble y;
  eolDouble z;
  eolDouble w;
}eolVec4D;

#define eol_vec_in_rect(v, r)  ((v.x >= r.x)&&(v.x < (r.x + r.w))&&(v.y >= r.y)&&(v.y < (r.y + r.h)))

/**
 * @brief create and return an eolVec2D
 */
eolVec2D eol_vec2d(eolDouble x, eolDouble y);

/**
 * @brief create and return an eolVec3D
 */
eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z);

/**
 * @brief create and return an eolVec4D
 */
eolVec4D eol_vec4d(eolDouble x, eolDouble y, eolDouble z, eolDouble w);

/**
 * @brief sets the outvector to a unit vector pointing at the angle specified
 * @param out output.  Vector will be set to a unit vector rotated by radians
 * @param radians specify the angle of the vector to be set.
 */
void eol_vec3d_set_angle_by_radians(eolVec3D *out,eolFloat radians);

void eol_vec2d_set_angle_by_radians(eolVec2D *out,eolFloat radians);

/**
 * @brief returns the angle of a vector in 2D space.
 * @param x the x component of the vector
 * @param y the y component of the vector
 * @return the angle of the vector in radians.
 */
eolFloat eol_vector_angle(eolFloat x,eolFloat y);

/**
 * @brief returns the angle between the two angles.
 * Takes into account wraping around 0 and 360
 * units are degrees
 * @Param a one of the angles in question
 * @Param b one of the angles in question
 * @return the angle, in degrees, between the two angles
 */
eolFloat eol_angle_between_degrees(eolFloat a, eolFloat b);

void eol_angle_clamp_degrees(eolFloat *a);

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

/**
 * @brief Adds the two input orientations together.
 * useful for applying vector to position
 * @param out the output orientation.  If NULL, nothing happens.
 * @param in1 one component
 * @param in2 the other component
 */
void eol_orientation_add(eolOrientation * out,
                         eolOrientation   in1,
                         eolOrientation   in2);

/**
 * @brief Merges the two.  Add position and rotation, but multiplies scale, color and alpha.
 * useful for applying layered  transformations.
 * @param out the output orientation.  If NULL, nothing happens.
 * @param in1 one component
 * @param in2 the other component
 */
void eol_orientation_mix(eolOrientation * out,
                         eolOrientation   in1,
                         eolOrientation   in2);

/**
 * @brief sets the passed orientation to a default set up.
 * sets alpha and scale to 1, all else to zero
 */
void eol_orientation_clear(eolOrientation *out);

void eol_orientation_copy(eolOrientation *dst,eolOrientation src);
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

eolUint eol_trail_get_length(eolTrail *trail);

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

#define eol_vec4d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z,ar[3] = vec.w)
#define eol_vec3d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y,ar[2] = vec.z)
#define eol_vec2d_into_array(ar,vec)   (ar[0] = vec.x,ar[1] = vec.y)

/**
 * @brief calculates the cross product of the two input vectors
 * @param out the resultant cross product
 * @param v1 one of the components
 * @param v2 one of the components
 */
void eol_vec3d_cross_product(eolVec3D *out, eolVec3D v1, eolVec3D v2);

/**
 * @brief Macro to get the dot product from a vector.
 *
 * @param a eolVect3D component of the dot product
 * @param b eolVect3D component of the dot product
 *
 * @return the calculated dot product
 */
#define eol_vec4d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w)
#define eol_vec3d_dot_product(a,b)      (a.x*b.x+a.y*b.y+a.z*b.z)
#define eol_vec2d_dot_product(a,b)      (a.x*b.x+a.y*b.y)

/**
 * @brief copies the data from one vector into another
 *
 * @param dst the destination vector
 * @param src the source vector
 */
#define eol_vec2d_copy(dst,src)  (dst.x = src.x,dst.y = src.y)
#define eol_vec3d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z)
#define eol_vec4d_copy(dst,src)  (dst.x = src.x,dst.y = src.y,dst.z = src.z,dst.w=src.w)

/**
 * @brief checks if vectors are exactly matches of each other
 *
 * @param a one vector to check
 * @param b other vector to check
 */
#define eol_vec2d_equal(a,b) ((a.x == b.x)&&(a.y == b.y))
#define eol_vec3d_equal(a,b) ((a.x == b.x)&&(a.y == b.y)&&(a.z == b.z))
#define eol_vec4d_equal(a,b) ((a.x == b.x)&&(a.y == b.y)&&(a.z == b.z)&&(a.w == b.w))

/**
 * @brief Macro to subtract two vectors
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result eolVect3D output
 * @param a eolVect3D input
 * @param b eolVect3D input
 */
#define eol_vec2d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y)
#define eol_vec3d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z)
#define eol_vec4d_sub(dst,a,b)     (dst.x=a.x-b.x,dst.y=a.y-b.y,dst.z=a.z-b.z,dst.w=a.w-b.w)

/**
 * @brief Macro to add two vectors
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result eolVect3D output
 * @param a eolVect3D input
 * @param b eolVect3D input
 */
#define eol_vec2d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y)
#define eol_vec3d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z)
#define eol_vec4d_add(dst,a,b)   (dst.x = a.x+b.x,dst.y = a.y+b.y,dst.z = a.z+b.z,dst.w = a.w+b.w)

/**
 * @brief Macro to scale a vector by a scalar value
 * varient ending in p takes a pointer to eolVect3D instead.
 * Varients ending with 2D only operate on the x an y components of vectors
 *
 * @param dst result eolVect3D output
 * @param src eolVect3D input
 * @Param factpr the scalar value to scale the vector by.
 */
#define eol_vec2d_scale(dst,src,factor) (dst.x = src.x *factor,\
                                         dst.y = src.y *factor)
#define eol_vec3d_scale(dst,src,factor) (dst.x = src.x *factor,\
                                         dst.y = src.y *factor,\
                                         dst.z = src.z *factor)
#define eol_vec4d_scale(dst,src,factor) (dst.x = src.x *factor,\
                                         dst.y = src.y *factor,\
                                         dst.z = src.z *factor,\
                                         dst.w = src.w *factor)
/**
 * @brief Macro that sets vector to zero.
 * @param a eolVect[2D|3D|4D] input
 */

#define eol_vec2d_clear(a)       (a.x=a.y=0)
#define eol_vec3d_clear(a)       (a.x=a.y=a.z=0)
#define eol_vec4d_clear(a)       (a.x=a.y=a.z=a.w=0)

/**
 * @brief Macro to get the negative of a vector
 *
 * @param src eolVect[2D|3D|4D] input
 * @param dst eolVect[2D|3D|4D] negated output
 */
#define eol_vec2d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y)
#define eol_vec3d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z)
#define eol_vec4d_negate(dst,src)      (dst.x = -src.x,dst.y = -src.y,dst.z = -src.z,dst.w = -src.w)

/**
 * @brief Macro to set the components of the vector
 *
 * @param v eolVect3D output
 * @param a eolFloat x component
 * @param b eolFloat y component
 * @param c eolFloat z component (only in 3D & 4D version)
 * @param d eolFloat w component (only in 4D version)
 */
#define eol_vec2d_set(v, a, b)  (v.x=(a), v.y=(b))
#define eol_vec3d_set(v, a, b, c)  (v.x=(a), v.y=(b), v.z=(c))
#define eol_vec4d_set(v, a, b, c,d)  (v.x=(a), v.y=(b), v.z=(c), v.w=(d))


eolVec2D *eol_vec2d_new();
eolVec3D *eol_vec3d_new();
eolVec4D *eol_vec4d_new();

eolVec2D *eol_vec2d_dup(eolVec2D old);
eolVec3D *eol_vec3d_dup(eolVec3D old);
eolVec4D *eol_vec4d_dup(eolVec4D old);

/**
 * @brief reflect a vector about a normal.  Simulates "Bounce"
 * @param out the resultant vector
 * @param normal the normal vector for the surface that is providing the bounce
 * @param in the input vector that will bounce off of the normal
 */
void eol_vec2d_reflect(eolVec2D *out, eolVec2D normal,eolVec2D in);
void eol_vec3d_reflect(eolVec3D *out, eolVec3D normal,eolVec3D in);
void eol_vec4d_reflect(eolVec4D *out, eolVec4D normal,eolVec4D in);


/**
 * @brief normalizes the vector passed.  does nothing for a zero length vector.
 *
 * @param v pointer to the vector to be normalized.
 */
eolFloat eol_vec2d_magnitude(eolVec2D V);
eolFloat eol_vec3d_magnitude(eolVec3D V);
eolFloat eol_vec4d_magnitude(eolVec4D V);

void eol_vec2d_normalize (eolVec2D *V);
void eol_vec3d_normalize (eolVec3D *V);
void eol_vec4d_normalize (eolVec4D *V);

/**
 * @brief returns the magnitude squared, which is faster than getting the magnitude
 * which would involve taking the square root of a floating point number.
 * @param V the vector to get the magnitude for
 * @return the square of the magnitude of V
 */
eolFloat eol_vec2d_magnitude_squared(eolVec2D V);
eolFloat eol_vec3d_magnitude_squared(eolVec3D V);
eolFloat eol_vec4d_magnitude_squared(eolVec4D V);

/**
 * @brief checks if the magnitude of V is less than size.  It does this without
 * doing square roots, which are costly.  It will still do floating point multiplication
 * @param V the vector to check
 * @param size the magnitude to check against
 * @return eolTrue if the magnitude of V is less than size or eolFalse otherwise
 */
eolBool  eol_vec2d_magnitude_less_than(eolVec2D V,eolFloat size);
eolBool  eol_vec3d_magnitude_less_than(eolVec3D V,eolFloat size);
eolBool  eol_vec4d_magnitude_less_than(eolVec4D V,eolFloat size);

/**
 * @brief scales the vector to the specified length without changing direction
 * No op is magnitude is 0 or V is NULL
 * @param V a pointer to the vector to scale
 * @param magnitude the new length for the vector
 */
void eol_vec2d_set_magnitude(eolVec2D * V,eolFloat magnitude);
void eol_vec3d_set_magnitude(eolVec3D * V,eolFloat magnitude);
void eol_vec4d_set_magnitude(eolVec4D * V,eolFloat magnitude);

/**
 * @brief checks if the distance between the two points provided is less than size.
 * @param p1 one point for the distance check
 * @param p2 another point for the distance check
 * @param size the value to check against
 * @return eolTrue if the distance between P1 and P2 is less than size, false otherwise
 */
eolBool eol_distance_between_less_than2d(eolVec3D p1,eolVec3D p2,eolFloat size);
eolBool eol_distance_between_less_than3d(eolVec3D p1,eolVec3D p2,eolFloat size);
eolBool eol_distance_between_less_than4d(eolVec3D p1,eolVec3D p2,eolFloat size);

#endif

