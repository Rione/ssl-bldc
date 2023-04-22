#ifndef _MATH_H_
#define _MATH_H_

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define Range(a, x, b) ((a) <= (x) && (x) <= (b))
#define Constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define Round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x)-0.5))
#define Radians(deg) ((deg)*DEG_TO_RAD)
#define Degrees(rad) ((rad)*RAD_TO_DEG)

#endif