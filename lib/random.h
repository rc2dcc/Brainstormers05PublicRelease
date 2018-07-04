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

// File random.h:
// contains the declaration of random number generators
// created 06-OCT-00 by Martin Lauer
// ---------------------------------------------

#ifndef random_h
#define random_h


namespace Statistics {

  class Random {
  public:
    static void seed (unsigned long int) throw ();     // set seed
    static unsigned long int seed () throw ();         // get seed
    static double basic_random () throw ();            // random number in [0,1]
  private:
    static unsigned long int sd;
  };

}

#endif
