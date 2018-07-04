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

// 2-dimensionale Normalverteilung mit pdf- und Zufallsfunktion
// Spezialanfertigung fuer die Brainstormers
// created 25-APR-2003 by Martin Lauer
// ------------------------------------------------------------

#ifndef normal_dist2d_h
#define normal_dist2d_h

//#include <stdexcept>

class Normal_distribution_2d {
 public:
  Normal_distribution_2d (double, double, double);// throw (invalid_argument);
  // Argumente: (arg1, arg2)=Varianzen, arg3=Kovarianz

  void operator() (double&, double&, double, double) const;// throw ();
  // Zufallszahlenerzeuger, liefert die Zufallswerte in (arg1, arg2)

  double pdf (double, double, double, double) const;// throw ();
  // Dichtefunktion

 protected:
  double c1, c2, c3;  // Cholesky-Faktor
  double ic1, ic2, ic3;  // Inverser Cholesky-Faktor
  double normal;  // Normalisierungskonstante
};

#endif
