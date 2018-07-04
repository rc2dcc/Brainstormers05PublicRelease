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


#include "normal_dist2d.h"
#include "random.h"
#include <cmath>
#include "macro_msg.h"

Normal_distribution_2d::Normal_distribution_2d (double v1, double v2, double cov) {// throw (invalid_argument) {
  // berechne Cholesky-Faktor:
  if (v1<=0)
    ERROR_OUT << "Varianz-Kovarianzmatrix nicht positiv definit";
  //throw invalid_argument ("Varianz-Kovarianzmatrix nicht positiv definit");

  c1=sqrt(v1);
  c2=cov/c1;
  double q=v2-c2*c2;
  if (q<=0)
    ERROR_OUT << "Varianz-Kovarianzmatrix nicht positiv definit";
  //throw invalid_argument ("Varianz-Kovarianzmatrix nicht positiv definit");
  c3=sqrt(q);
  
  normal = 1.0/(2*M_PI*c1*c3);
  
  // berechne inversen Cholesky-Faktor
  ic1=1.0/c1;
  ic3=1.0/c3;
  ic2=-c2/(c1*c3);
}

namespace {
  inline double square (double x) {
    return x*x;
  }
}

double Normal_distribution_2d::pdf (double x1, double x2, double mean1, double mean2) const { // throw () {
  double d=x1-mean1;
  double q=square (ic1*d)+square (ic2*d+ic3*(x2-mean2));
  return normal*exp(-0.5*q);
}

void Normal_distribution_2d::operator() (double& x1, double& x2, double mean1, double mean2) const { // throw () {
  // erzeuge eine Standard-Normalverteilung
  double u1 = Statistics::Random::basic_random();
  double u2 = Statistics::Random::basic_random();
  double r = sqrt(-2.0*log(u1));
  double w = 2*M_PI*u2;
  double y1 = r*cos(w);
  double y2 = r*sin(w);

  // transformiere die Normalverteilung
  x1=mean1+c1*y1;
  x2=mean2+c2*y1+c3*y2;
}

