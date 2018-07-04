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

/*
 *  Author:   Artur Merke 
 */

#include "geometry2d.h"
#include <math.h>

#include <stdio.h> // DEBUG

#define EPS 1.0e-4
//#define PI M_PI
#define sqr(x) ((x)*(x))
#define max(x,y) (x>y?x:y)
#define min(x,y) (x>y?y:x)

int Geometry2d::intersect_circle_with_circle(Point2d & p1, Point2d & p2, const Circle2d & c1, const Circle2d & c2) {
  // stelle den ersten Kreis in den Ursprung, d.h. verschiebe damit den Mittelpunkt
  // von Kreis 2 um den Vektor (0-Cx1,0-Cy1)=(-Cx1,-Cy1)
   Value a,b,D,sqrNorm_ab,minus_p_half,q;
   a= c2.center.x - c1.center.x; b= c2.center.y - c1.center.y;//a= Cx2-Cx1; b= Cy2- Cy1;
   //   printf("\ncicle 1 (%g, %g | %g), circle 2 (%g, %g | %g), radius= %g\n\n",0.0,0.0,r1,a,b,r2);
   // Teste ob Loesungen vorhanden
   sqrNorm_ab= sqr(a)+sqr(b);   
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
      D= sqr(a)+sqr(b)+sqr(c1.radius)-sqr(c2.radius);
      if (a != 0.0) {
	 minus_p_half= (b*D)/(2.0*sqrNorm_ab);
	 q= (0.25*sqr(D) - sqr(c1.radius)*sqr(a) ) / sqrNorm_ab; 
	 q= sqrt( sqr(minus_p_half)- q); //Ausdruck unter der Wurzel muss >= 0.0 sein, da eine Lsg. ex. 

	 p1.y= minus_p_half + q;
	 p1.x= (0.5*D- b*p1.y)/a;
   
	 p2.y= minus_p_half - q;
	 p2.x= (0.5*D- b*p2.y)/a;
      }
      else {
	 p1.y= D/(2.0*b);
	 p2.y= p1.y;
	 p1.x= sqrt( sqr(c1.radius)- sqr(p1.y) );
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

#if 0
int Geometry::intesect_circle_with_x_line(float &x1,float &y1, float &x2, float &y2,
						 float Cx, float Cy, float r, float xline) {
   float dum,y= xline;
   if ( fabs(y-Cy) > r ) return 0;
   y1= y; y2= y;
   dum= sqrt( sqr(r)-sqr(y-Cy) );
   x1= Cx+ dum;
   x2= Cx- dum;
   return 1;
}

int Geometry::intesect_circle_with_y_line(float &x1,float &y1, float &x2, float &y2,
						 float Cx, float Cy, float r, float yline) {
   float dum,x= yline;
   if ( fabs(x-Cx) > r ) return 0;
   x1= x; x2= x;
   dum= sqrt( sqr(r)-sqr(x-Cx) );
   y1= Cy+ dum;
   y2= Cy- dum;
   return 1;
}

int Geometry::circles_with_angle(float &Cx1, float &Cy1, float &Cx2, float &Cy2,float &radius,
                       float x1, float y1, float x2, float y2, float  angle) {
   //P1= (x1,y1), P2= (x2,y2)
   float m_x,m_y, stretch_factor,dum;
   
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
      radius= sqrt( sqr(Cx1-x1) + sqr(Cy1-y1) );
 
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

#if 0
     
main() {
   float x,y,x1,y1,angle,radius,x2,y2;
   float Cx1,Cy1,r1,Cx2,Cy2,r2;
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






