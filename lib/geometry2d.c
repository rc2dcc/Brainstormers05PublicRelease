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

#include "geometry2d.h"
//#include "macro_msg.h"

std::ostream& operator<< (std::ostream& o, Set2d const & set) {
  set.draw(o);
  return o;
}

/******************************************************************************
  Circle2d
*******************************************************************************/

bool Circle2d::inside(Vector const & point) const  {
  return center.sqr_distance(point) <= SQUARE( radius );
}

void Circle2d::draw(std::ostream & o) const { 
  o << "CIRCLE col=ff0000 (" << center.x << "," << center.y << "," << radius << ");";
}

/******************************************************************************
  XYRectangle2d
*******************************************************************************/

XYRectangle2d::XYRectangle2d(Vector const & p1, Vector const & p2) {
  center.x = 0.5 * p1.x + 0.5* p2.x;
  center.y = 0.5 * p1.y + 0.5* p2.y;
  if ( p2.x > p1.x )
    size_x= p2.x - p1.x;
  else
    size_x= p1.x - p2.x;

  if ( p2.y > p1.y )
    size_y= p2.y - p1.y;
  else
    size_y= p1.y - p2.y;
}

bool XYRectangle2d::inside(Vector const & point) const  {
  double max_x= center.x + 0.5*size_x;
  double min_x= center.x - 0.5*size_x;
  double max_y= center.y + 0.5*size_y;
  double min_y= center.y - 0.5*size_y;
  return ( point.x <= max_x && point.x >= min_x && point.y <= max_y && point.y >= min_y);
}

void XYRectangle2d::draw(std::ostream & o) const {
  double hx= size_x * 0.5;
  double hy= size_y * 0.5;
  o << "POLYGON col=ff0000 " 
    << "( " << center.x - hx << "," << center.y - hy << ")"
    << "( " << center.x + hx << "," << center.y - hy << ")"
    << "( " << center.x + hx << "," << center.y + hy << ")"
    << "( " << center.x - hx << "," << center.y + hy << ")"
    << ";";
}

/******************************************************************************
  Triangle2d
*******************************************************************************/


bool Triangle2d::inside(Vector const & point) const  {
  // look for  p= a* p1 + b * p2 + c * p3 with a+b+c=1 and a,b,c >= 0
  // if such a solution doesn't exits, then the point cannot be in the triangle;

  double A= p2.x - p1.x;
  double B= p3.x - p1.x;
  double C= p2.y - p1.y;
  double D= p3.y - p1.y;

  double det= A * D - C * B;
  if ( fabs(det) < 0.000001 ) {//consider matrix non regular (numerical stability)
    //cout << " false, det= " << det;
    return false;
  }

  double x= point.x - p1.x;
  double y= point.y - p1.y;


  double a= D * x - B * y;
  double b= -C * x + A * y;

  a/= det;
  b/= det;

  if (a < 0 || b < 0) {
    //cout << "\n false, a= " << a << " b= " << b;
    return false;
  }
  if ( a + b > 1.0) {
    //cout << "\n false, a= " << a << " b= " << b << " a+b= " << a+ b;
    return false;
  }

#if 0
  cout << "\n A= " << A << " B= " << B;
  cout << "\n C= " << C << " D= " << D;
  cout << "\n x= " << x << " y= " << y;
  cout << "\n true  a= " << a << " b= " << b << " c= " << 1- (a+ b) << " det= " << det;
#endif
  return true;
}

void Triangle2d::draw(std::ostream & o) const { 
  o << "POLYGON col=ff0000 " 
    << "( " << p1.x << "," << p1.y << ")"
    << "( " << p2.x << "," << p2.y << ")"
    << "( " << p3.x << "," << p3.y << ")"
    << ";";
}

/******************************************************************************
  Quadrangle2d
*******************************************************************************/

Quadrangle2d::Quadrangle2d(Vector const & q1, Vector const & q2, double width1, double width2) {
  Vector norm2= q2-q1;
  Vector norm;
  norm.x= -norm2.y;
  norm.y= norm2.x;
  norm2= norm;
  norm.normalize(0.5*width1);
  norm2.normalize(0.5*width2);

  p1= q1+ norm;
  p2= q1- norm;
  p3= q2- norm2;
  p4= q2+ norm2;
}

Quadrangle2d::Quadrangle2d(Vector const & q1, Vector const & q2, double width) {
  Vector tmp= q2-q1;
  Vector norm;
  norm.x= -tmp.y;
  norm.y= tmp.x;
  norm.normalize(0.5*width);
  p1= q1+ norm;
  p2= q1- norm;
  p3= q2- norm;
  p4= q2+ norm;
}

bool Quadrangle2d::inside(Vector const & point) const  {
  if ( Triangle2d(p1,p2,p3).inside( point ) ||
       Triangle2d(p1,p3,p4).inside( point ) )
    return true;
  return false;
}

void Quadrangle2d::draw(std::ostream & o) const { 
  o << "POLYGON col=ff0000 " 
    << "( " << p1.x << "," << p1.y << ")"
    << "( " << p2.x << "," << p2.y << ")"
    << "( " << p3.x << "," << p3.y << ")"
    << "( " << p4.x << "," << p4.y << ")"
    << ";";
}

/******************************************************************************
  Halfplane2d
*******************************************************************************/

Halfplane2d::Halfplane2d(Vector const & p, ANGLE const & a) {
  //we will implement this without the usage of the quite costly Vector::arg() 
  //method for the players
  pos= p;
  ANGLE ang1 = a + ANGLE(0.5*M_PI);
  normal.init_polar(1.0, ang1.get_value() ); 
}

bool Halfplane2d::inside(Vector const & point) const  {
  // A * x + B * y - C >= 0   is the representation of the halfplane
  double A= normal.x;
  double B= normal.y;
  double C= A*pos.x + B*pos.y;

  if (  A * point.x + B * point.y >= C ) 
    return true;
  return false;
}

/******************************************************************************
  Tube2d
*******************************************************************************/

bool Tube2d::inside(Vector const & point) const  {
  return Geometry2d::sqr_distance_to_line(point, Line2d(pos,dir)) <= sqr_width*0.25;
}

bool HalfTube2d::inside(Vector const & point) const  {
  return Geometry2d::sqr_distance_to_line(point, Line2d(pos,dir)) <= sqr_width*0.25 &&
    dir.dot_product(point) >= 0;
}
/******************************************************************************
  Cone2d
*******************************************************************************/

Cone2d::Cone2d(Vector const & p, ANGLE const & ang1, ANGLE const & ang2) {
  pos= p;
  dir1.init_polar(1.0,ang1);
  dir2.init_polar(1.0,ang2);
}

Cone2d::Cone2d(Vector const & p, Vector const & dir, ANGLE const & ang) {
  double a= 0.5*ang.get_value_0_p2PI(); //here a half of the angle is taken
  double c= cos( a );
  double s= sin( a );

  pos= p;
  /* Rotation Matrix (counterclockwise)    Rotation Matrix (clockwise) 
     [ cos   -sin]                         [  cos    sin]                         
     [ sin    cos]                         [ -sin    cos]                         
  */                                                                           

  dir1= Vector( c*dir.x + s*dir.y, c*dir.y - s*dir.x); //rotate counterclockwise
  dir2= Vector( c*dir.x - s*dir.y, s*dir.x + c*dir.y); //rotate clockwise  
}


bool Cone2d::inside(Vector const & point) const  {
  Vector tmp1,tmp2;
  tmp1.x= -dir1.y;
  tmp1.y= dir1.x;

  tmp2.x= dir2.y;
  tmp2.y= -dir2.x;

  if ( Halfplane2d(pos,tmp1).inside(point) &&
       Halfplane2d(pos,tmp2).inside(point) )
    return true;
  return false;
}

/******************************************************************************
  Geometry2d
*******************************************************************************/

//#include <math.h>
//#include <stdio.h> // DEBUG

//#define EPS 1.0e-4
//#define PI M_PI
#define sqr(x) ((x)*(x))
//#define max(x,y) (x>y?x:y)
//#define min(x,y) (x>y?y:x)

int Geometry2d::intersect_circle_with_circle(Vector & p1, Vector & p2, const Circle2d & c1, const Circle2d & c2) {
  // stelle den ersten Kreis in den Ursprung, d.h. verschiebe damit den Mittelpunkt
  // von Kreis 2 um den Vektor (0-Cx1,0-Cy1)=(-Cx1,-Cy1)
   double a,b,D,sqrNorm_ab,minus_p_half,q;
   a= c2.center.x - c1.center.x; b= c2.center.y - c1.center.y;//a= Cx2-Cx1; b= Cy2- Cy1;
   //   printf("\ncicle 1 (%g, %g | %g), circle 2 (%g, %g | %g), radius= %g\n\n",0.0,0.0,r1,a,b,r2);
   // Teste ob Loesungen vorhanden
   sqrNorm_ab= SQUARE(a)+SQUARE(b);   
   if (c1.radius<= 0.0 || c2.radius <= 0.0) return 0; // Radien sollten positiv sein
   if (a == 0.0 && b == 0.0 ) return 0; //die Kreise sollten sich unterscheiden 
   D= sqrt(sqrNorm_ab); //Entfernung der Mittelpunkte
   if ( D > c1.radius+c2.radius) return 0; // Kreise zu weit auseinander 
   if (c1.radius > D+ c2.radius) return 0; // ein Kreis liegt voll in dem anderen
   if (c2.radius > D+ c1.radius) return 0;
   if (D== c1.radius+c2.radius || c1.radius== D+c2.radius) { //diesen Fall gesondert betrachten, da sonst evtl. Gleitkommarechnung zu Fehlern fuehrt
      p1.x= c1.radius* (a/D);
      p1.y= c1.radius* (b/D);
      p2.x= p1.x; p2.y= p1.y;
   }
   else if (c2.radius== D+c1.radius) { //diesen Fall gesondert betrachten, da sonst evtl. Gleitkommarechnung zu Fehlern fuehrt
      p1.x= -c1.radius* (a/D);     
      p1.y= -c1.radius* (b/D);
      p2.x= p1.x; p2.y= p1.y;
   }
   else { //Kreise haben mehr als einen Schnittpunkt, also auch Fehlerquelle Gleitkommaarithmetik etwas eliminiert
      // Ok, jetzt ist sicher, dass mind. 1 Lsg. ex.
      D= SQUARE(a)+SQUARE(b)+SQUARE(c1.radius)-SQUARE(c2.radius);
      if (a != 0.0) {
	 minus_p_half= (b*D)/(2.0*sqrNorm_ab);
	 q= (0.25*SQUARE(D) - SQUARE(c1.radius)*SQUARE(a) ) / sqrNorm_ab; 
	 q= sqrt( SQUARE(minus_p_half)- q); //Ausdruck unter der Wurzel muss >= 0.0 sein, da eine Lsg. ex. 

	 p1.y= minus_p_half + q;
	 p1.x= (0.5*D- b*p1.y)/a;
   
	 p2.y= minus_p_half - q;
	 p2.x= (0.5*D- b*p2.y)/a;
      }
      else {
	 p1.y= D/(2.0*b);
	 p2.y= p1.y;
	 p1.x= sqrt( SQUARE(c1.radius)- SQUARE(p1.y) );
	 p2.x= -p1.x;
      }
   }
//Normalisierung auf den Mittelpunkt aufheben;
   p1.x+= c1.center.x;
   p1.y+= c1.center.y;
   p2.x+= c1.center.x;
   p2.y+= c1.center.y;
   return 1;
}   


bool Geometry2d::intersect_lines(Vector & p, double & stretch1, double & stretch2, Line2d const & L1, Line2d const & L2) {
  /* solve Ax= b 

     with A= [ -L1.dir | L2.dir ]
     
                1      / L2.dir.y  ,  -L2.dir.x \
     inv A = ------ * |                          |
              detA     \ L1.dir.y  ,  -L1.dir.x /


          / stretch1 \
     x = |            |
          \ stretch2 /

  */
  Vector b= L1.pos - L2.pos;
  Value detA= -L1.dir.x * L2.dir.y + L1.dir.y * L2.dir.x;
  if ( fabs(detA) < 0.000001 ) //consider matrix non regular (numerical stability)
    return false;

  stretch1= (L2.dir.y * b.x - L2.dir.x * b.y) / detA;
  stretch2= (L1.dir.y * b.x - L1.dir.x * b.y) / detA;
    
  p= L1.pos + stretch1 * L1.dir; 
  return true;
}

void Geometry2d::projection_to_line(Vector & res, Vector const & pos, Line2d const & line) {
  Vector b= pos - line.pos;
  double tmp= b.dot_product( line.dir ) / line.dir.sqr_norm();
  res= line.pos + tmp * line.dir;
}

#if 0
int Geometry::intesect_circle_with_x_line(double &x1,double &y1, double &x2, double &y2,
						 double Cx, double Cy, double r, double xline) {
   double dum,y= xline;
   if ( fabs(y-Cy) > r ) return 0;
   y1= y; y2= y;
   dum= sqrt( SQUARE(r)-SQUARE(y-Cy) );
   x1= Cx+ dum;
   x2= Cx- dum;
   return 1;
}

int Geometry::intesect_circle_with_y_line(double &x1,double &y1, double &x2, double &y2,
						 double Cx, double Cy, double r, double yline) {
   double dum,x= yline;
   if ( fabs(x-Cx) > r ) return 0;
   x1= x; x2= x;
   dum= sqrt( SQUARE(r)-SQUARE(x-Cx) );
   y1= Cy+ dum;
   y2= Cy- dum;
   return 1;
}

int Geometry::circles_with_angle(double &Cx1, double &Cy1, double &Cx2, double &Cy2,double &radius,
                       double x1, double y1, double x2, double y2, double  angle) {
   //P1= (x1,y1), P2= (x2,y2)
   double m_x,m_y, stretch_factor,dum;
   
   if (angle > PI) angle-= PI;
   dum= sin(angle);
   if (fabs(dum) < EPS) //Winkel hat 180 Grad 
      stretch_factor= 0.0;
   else
      stretch_factor= cos(angle)/dum;
   
   m_x= 0.5*(x1+x2);  //Der Mittelpunkt der Strecke zw. den 2 Punkten P1 und P2
   m_y= 0.5*(y1+y2);
   if (stretch_factor != 0.0) {   //Der Punkt liegt nicht auf der Geraden zw. den Punkten P1,P2
      Cx1= m_x + 0.5*(y1-y2)* stretch_factor;
      Cy1= m_y + 0.5*(x2-x1)* stretch_factor;
      radius= sqrt( SQUARE(Cx1-x1) + SQUARE(Cy1-y1) );
 
      Cx2= m_x - 0.5*(y1-y2)* stretch_factor;
      Cy2= m_y - 0.5*(x2-x1)* stretch_factor;
   } 
   else {  //Der Punkt liegt auf der Geraden zwischen den Punkten P1,P2
      Cx1= m_x;
      Cy1= m_y;
      Cx2= m_x;
      Cy2= m_y;
   }
   return 1;
}

#endif

/***************************** T E S T *****************************************/
#if 0

#include <iostream>

main() {
  Line2d  L1( Vector(0,0), Vector(10,10) );
  Line2d  L2( Vector(2,0), Vector(1,-1) );
  Vector pos; 
  double s1,s2;

  bool res= Geometry2d::intersect_lines(pos,s1,s2,L1,L2);
  
  cout << "\nres= " << res << " pos= " << pos << " : " 
       << L1.pos + s1 * L1.dir << " , " 
       << L2.pos + s2 * L2.dir; 

  Geometry2d::projection_to_line(pos, Vector(0,2),L1);
  cout << "\nres_pos = " << pos;

  Tube2d tube;
  tube.set_width(6);
  cout << "\ntube.inside= " << tube.inside(Vector(3,3));
}

#endif

#if 0
     
main() {
   double x,y,x1,y1,angle,radius,x2,y2;
   double Cx1,Cy1,r1,Cx2,Cy2,r2;
   int b;
   printf("\n\nEingabe C1: ");
   scanf("%f %f %f", &Cx1,&Cy1,&r1);
   printf("\n\nEingabe C2: ");
   scanf("%f %f %f", &Cx2,&Cy2,&r2);

   printf("\ncicle 1 (%g, %g | %g), circle 2 (%g, %g | %g), radius= %g\n\n",Cx1,Cy1,r1,Cx2,Cy2,r2);
   

   b= Geometry::intesect_circle1_with_circle2(x1,y1,x2,y2,Cx1,Cy1,r1,Cx2,Cy2,r2);
   if (b)
      printf("\nLoesung: (x1,y1)=(%g,%g) | (x2,y2)= (%g,%g) ",x1,y1,x2,y2);
   else {
      printf("\nKeine Loesung gefunden!!!");
      printf("\n???? (x1,y1)=(%g,%g) | (x2,y2)= (%g,%g) ",x1,y1,x2,y2);
   }
   printf("\n");
   return 0;
}

#endif






