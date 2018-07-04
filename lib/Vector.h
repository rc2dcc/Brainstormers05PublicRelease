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

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <iostream>

#include "globaltypes.h"

#define PI M_PI

#include "angle.h"
class Vector {
public:
   Value   x ;
   Value   y ;

   /** the following constructor may cause demage if not stated as explicit */
   explicit Vector(const Value vx = 0.0, const Value vy = 0.0) ; 
   explicit Vector(const ANGLE & ang);


   void init_polar(const Value& n, const Angle& a);
   void init_polar(const Value& n, const ANGLE& a);
   
   void    operator +=(Vector v) ;
   void    operator -=(Vector v) ;
   void    operator *=(Value a) ;
   void    operator /=(Value a) ;

   Value norm() const { return sqrt(square(x)+square(y)) ;}
   Value sqr_norm() const { return (square(x)+square(y));}
   Angle angle() { return arg(); }

   /** berechnet das Argument der komplexen Zahl (x,y) auf das Intervall
      [0,2*PI) normalisiert. Das Argument von (0,0) wird auf 0 gesetzt*/
   Angle arg() const;
   ANGLE ARG() const;

   /** berechnet eine reelle Zahl modulo 2*PI, so dass der normalisierte
      Winkel im Intervall [0,2*PI) liegt */
   Angle normalize_angle(const Angle& a) const;

   friend Vector operator *(const Value& a, const Vector& b);
   friend Vector operator +(const Vector& a, const Vector& b) ;
   friend Vector operator -(const Vector& a, const Vector& b) ;

   /** */
   void normalize(Value l = 1.0) ;

   /** gibt den euklidischen Abstand zwischen den Vektoren */
   Value distance(const Vector& orig) const;
   
   /** gibt den quadratische euklidischen Abstand zwischen den Vektoren.
    * Diese Routine is effizienter als distance(...), da das Wurzelziehen entfaellt!!! 
    */
   Value sqr_distance(const Vector& orig) const;
    
   /** gibt den Winkel zwischen dem Vector selbst und dem Vector dir */
   Angle angle(const Vector& dir) const;
   ANGLE ANGLE_to(const Vector& dir) const;

   /** rotiert um den angegebenen Winkel */
   void rotate(const Angle& ang) ;
   void ROTATE(const ANGLE& ang) ;

   /** berechnet das Skalarprodukt Zwischen 2 Vektoren */
   Value dot_product(const Vector& orig) const {return x*orig.x + y*orig.y;}

private:
   Value square(Value v) const { return v * v; }
} ;


extern std::ostream& operator<< (std::ostream& o, const Vector& v) ;


#endif
