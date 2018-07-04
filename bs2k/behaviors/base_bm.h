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

#ifndef _BASE_BM_H_
#define _BASE_BM_H_

#include "cmd.h"
#include <typeinfo>

class BaseBehavior {
 public:
  static bool init(char const * conf_file, int argc, char const* const* argv) { return true; }
  virtual bool get_cmd(Cmd & cmd)= 0;

  virtual void reset_intention() {};
  /** don't overwrite this id() method, 
      it will deliver the class names also in child classes 
  */
  virtual char const* id();
};

/* These are preliminary classes. */
class NeckBehavior {
  public:
  static bool init(char const * conf_file, int argc, char const* const* argv) { return true; }
  virtual bool get_cmd(Cmd & cmd)= 0;

  /** don't overwrite this id() method, 
      it will deliver the class names also in child classes 
  */
  virtual char const* id();
};

class ViewBehavior {
  public:
  static bool init(char const * conf_file, int argc, char const* const* argv) { return true; }
  virtual bool get_cmd(Cmd & cmd)= 0;

  /** don't overwrite this id() method, 
      it will deliver the class names also in child classes 
  */
  virtual char const* id();
};

class AttentionToBehavior {
  public:
  static bool init(char const * conf_file, int argc, char const* const* argv) { return true; }
  virtual bool get_cmd(Cmd & cmd)= 0;

  /** don't overwrite this id() method, 
      it will deliver the class names also in child classes 
  */
  virtual char const* id();
};

#endif
