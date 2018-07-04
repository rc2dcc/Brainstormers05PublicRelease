/*
Brainstormers 2D (Soccer Simulation League 2D)
PUBLIC SOURCE CODE RELEASE 2005
Copyright (C) 1998-2005 Neuroinformatics Group,
                        University of Osnabrueck, Germany

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Vector.h"
#include <math.h>


#define min(x,y) (x>y?y:x)
#define max(x,y) (x>y?x:y)

#define EPS 1.0e-10

/*
 *===================================================================
 *Part: Plane Vector
 *===================================================================
 */


Vector::Vector(const Value vx, const Value vy)
{
        x = vx ;
        y = vy ;
}

Vector::Vector(const ANGLE & ang) 
{ 
  x = cos(ang);
  y = sin(ang);
}


void Vector::init_polar(const Value& n, const Angle& a) {
   x= n*cos(a);
   y= n*sin(a);
}


void Vector::init_polar(const Value& n, const ANGLE& a) {
   x= n*cos(a.get_value());
   y= n*sin(a.get_value());
}

void Vector::operator +=(Vector v)
{
        x += v.x ;
        y += v.y ;
}

void Vector::operator -=(Vector v)
{
        x -= v.x ;
        y -= v.y ;
}

void Vector::operator *=(Value a) {
        x *= a ;
        y *= a ;
}

void Vector::operator /=(Value a)
{
        x /= a ;
        y /= a ;
}


Angle Vector::normalize_angle(const Angle& a) const {
   Value tmp= fmod(a,2*PI);
   if (tmp < 0.0) 
      return tmp+2*PI;
   else 
      return tmp;
}

Angle Vector::arg() const {
   if (0.0==x && 0.0==y) return 0.0;
   Value tmp= atan2(y,x);
   if (tmp < 0.0)
      return tmp+2*PI;
   else
      return tmp;
}

ANGLE Vector::ARG() const {
  return ANGLE(x,y);
}

std::ostream& operator<< (std::ostream& o, const Vector& v) 
{
  //return o << "#V[" << v.x << "," << v.y << "]" ;
  return o << "(" << v.x << "/" << v.y << ")" ;
}

Vector operator +(const Vector& a, const Vector& b)
{
        return Vector((a.x + b.x), (a.y + b.y)) ;
}

Vector operator -(const Vector& a, const Vector& b)
{
        return Vector((a.x - b.x), (a.y - b.y)) ;
}

Vector operator *(const Value& a, const Vector& b) {
        return Vector((a * b.x), (a * b.y)) ;
}

void Vector::normalize(Value l) 
{
        *this *= (l/max(norm(),EPS)) ;
}

Value Vector::distance(const Vector& orig) const
{
        return (*this - orig).norm() ;
}

Value Vector::sqr_distance(const Vector& orig) const
{
          return (*this - orig).sqr_norm() ;
}

Angle Vector::angle(const Vector& dir) const {
   Angle ang = dir.arg() - arg() ;
   return normalize_angle(ang) ;
}

ANGLE Vector::ANGLE_to(const Vector& dir) const {
   return dir.ARG() - ARG() ;
}
  
void Vector::rotate(const Angle& ang) {
#if 0 //old code very inefficient
   Value r1 = norm() ;
   Angle th1 = arg() ;

   x = r1 * cos(th1 + ang) ;
   y = r1 * sin(th1 + ang) ;
#else //new code (already tested) art 28.11.2002
  Value c= cos( ang );
  Value s= sin( ang );

  /* Rotation Matrix (counterclockwise)
  [ cos    -sin]
  [ sin     cos]
  */
  Value old_x= x;
  x = c*x - s*y;
  y = s*old_x + c*y;
#endif
}

void Vector::ROTATE(const ANGLE & ang) {
  rotate(ang.get_value());
}

