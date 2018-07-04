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

#ifndef _WS_PSET_H_
#define _WS_PSET_H_

#include "ws.h"
#include "intercept.h"
#include "geometry2d.h"
#include <string.h>

typedef WS::Player const* PPlayer;
class InterceptBall;

/**
   WSpset - WS player set
   This is the fundamental data structure for operations with player sets. 
   You can perform different set theoretic operations with them. But most
   important are the geometric methods, which should simplify extraction
   of features in the future.
 */
struct WSpset {
  
  static InterceptBall cvInterceptBallBehavior;
  struct InterceptBallResult
  {
    InterceptBallResult() 
    { validAtTime=-1;
      for (int i=0; i<NUM_PLAYERS; i++) stepsToInterceptMyTeam[i]=1000;
      for (int i=0; i<NUM_PLAYERS; i++) stepsToInterceptHisTeam[i]=1000;
    }
    void reset()
    { validAtTime=-1;
      for (int i=0; i<NUM_PLAYERS; i++) stepsToInterceptMyTeam[i]=1000;
      for (int i=0; i<NUM_PLAYERS; i++) stepsToInterceptHisTeam[i]=1000;
    }
    long   validAtTime;
    int    stepsToInterceptMyTeam [NUM_PLAYERS+1];
    Vector icptPositionsMyTeam    [NUM_PLAYERS+1];
    int    stepsToInterceptHisTeam[NUM_PLAYERS+1];
    Vector icptPositionsHisTeam   [NUM_PLAYERS+1];
  };
  static InterceptBallResult cvInterceptBallResult;
  
  friend class WSinfo;
  //typedef WS::Player const* Player;
public: //internal data structure
  static const int MAX_NUM= 2*NUM_PLAYERS+2;
  int num;                             ///< number of valid pointers i.e. p[i], i>= num is NOT defined
  PPlayer p[MAX_NUM];  ///< keeps const pointers to WS::Player structures
  
public:
  WSpset() { num= 0; }                 ///< standard constructor

public: //operators
  /**
     selects the i-th player out of p, there is no check if idx >= this->num
   */
  PPlayer operator[](int idx) const { 
    return p[idx]; 
  }

  void operator=(const WSpset & pset) {
    num= pset.num;
    if (num>0) memcpy(p,pset.p, num * sizeof( WS::Player * ));
  }

  /** appends the elements of pset to the own player set 
      (skips elements from pset if the resulting size gets larger then MAX_NUM)
   */
  void operator+=(const WSpset & pset) {
    append(pset);
  }
  
public: //some methods for set/tuple operations
  /**
     append elements from pset (duplicate members won't be removed)
     you should prefer this method to the join method, if you know
     that "this" and pset are disjoint.
     (skips elements from pset if the resulting size gets larger then MAX_NUM)
   */
  void append(const WSpset & pset);
  /**
     returns false if there is no place to append the new player
   */
  bool append(PPlayer);

  /**
     returns false if there is no place to prepend the new player
   */
  bool prepend(PPlayer);

  /**
     append elements from pset, but don't keep duplicate members.
     This operation is quite costly, requiring this->num * pset.num operations,
     so if you know that "this" and pset are disjoint, use the (very fast)
     append method.
     (skips elements from pset if the resulting size gets larger then MAX_NUM)
   */
  void join(const WSpset & pset);
  /**
     returns false if the player is not in the set and there is no place to append the new player
   */
  bool join(PPlayer);

  /**
     keep just those elements which coincide with elements from pset
     requires this->num * pset.num operations!

     The order of the elements is not changed, and the gaps a filled by a left shift 
     of the tail (but impemented more efficiently ;-)
   */
  void meet(const WSpset & pset);

  void remove(const WSpset & pset);
  void remove(PPlayer player);
public: 
  //other useful member functions, the return value can always be == 0  
  PPlayer closest_player_to_point(Vector pos) const;
  PPlayer get_player_by_number(int number) const;
  PPlayer get_player_by_team_and_number(int team, int number) const;
  PPlayer get_player_with_newest_pass_info() const;

protected:

public: //some "geometric" methods
  /** setting how_many to exactly the value you really use, saves many
      sorting steps (i.e. if you need 2 players, don't ask unnecessary 
      for more, it costs time)
   */
  void keep_and_sort_players_by_x_from_right(int how_many);
  void keep_and_sort_players_by_x_from_left(int how_many);
  void keep_and_sort_players_by_y_from_right(int how_many);
  void keep_and_sort_players_by_y_from_left(int how_many);

  void keep_and_sort_closest_players_to_point(int how_many, Vector pos);

  void keep_players_with_max_age(int age);
  /** this method makes most of the intercept stuff in PolicyTools superfluous!!!
      
      REMARK: the size of the array intercept_res must be at least the minimum 
              of the parameter 'how_many' and the actual number of players in the 
              player set!
  */
  void keep_and_sort_best_interceptors(int how_many, Vector ball_pos, Vector ball_vel, InterceptResult * intercept_res);
  void keep_and_sort_best_interceptors_with_intercept_behavior
       ( int how_many, Vector ball_pos, Vector ball_vel, InterceptResult * intercept_res);
  void keep_and_sort_best_interceptors_with_intercept_behavior_to_WSinfoBallPos
       ( int how_many, InterceptResult * intercept_res);

  void keep_players_in(Set2d const & set);

  void keep_players_in_circle(Vector pos, Value radius);

  /** if you cannot constrain the sides to be parallel to the x or y axis,
      then take a look at keep_players_in_quadrangle
   */
  void keep_players_in_rectangle(Vector center, Value size_x, Value size_y);

  /** a rectangle can be also specified by two of his corners which lie 
      on opposite sides of one of his diagonals

    p1                                        p2           p1
      +---------+                               +---------+  
      |         |     <--- OK                   |         |  <--- NOT OK
      |         |                               |         |  
      +---------+                               +---------+  
                 p2
   */
  void keep_players_in_rectangle(Vector p1, Vector p2);
  
  /**
     if p1,p2,p3 are not colinear, then they define a triangle
   */
  void keep_players_in_triangle(Vector p1, Vector p2, Vector p3);

  /**
     quadrangle is like a rectangle, but vertices are not required to be parallel to
     the x or the y axes!!!
     p1 and p3 must be connected by a diagonal of the quadrangle, or equivalently: 
     the points p1,p2,p3,p4 must follow the circumference of the rectangle

    p2           p1                             p3           p1
      +---------+                                +---------+  
      |         |     <--- OK                    |         |  <--- NOT OK
      |         |                                |         |  
      +---------+                                +---------+  
    p3           p4                             p2          p4
       
   */
  void keep_players_in_quadrangle(Vector p1, Vector p2, Vector p3, Vector p4);

  /**
     quadrangle is like a rectangle, but vertices are not required to be parallel to
     the x or the y axes!!!

      a                           b
       +-------------------------+                  
       |                         |
       |                         |
    p1 +                         + p2
       |                         |
       |                         |
       +-------------------------+ 
      c                           d
 
      the distance between  (a and c) is width
      the distance between  (b and d) is width

      the vectors a-c and b-d are parallel and orthogonal to the vector p2-p1

      but p2-p1 doesn't need to be parallel to the x or the y axes
   */
  void keep_players_in_quadrangle(Vector p1, Vector p2, Value width);

  /**
     quadrangle is like a rectangle, but vertices are not required to be parallel to
     the x or the y axes!!!
                                  b
                         --------+
      a         ---------        |
       +--------                 |
       |                         |
       |                         |
    p1 +                         + p2
       |                         |
       |                         |
       +--------                 |
      c         ---------        |
                         --------+
                                  d
 
      the distance between  (a and c) is width
      the distance between  (b and d) is width2 (and doesn't need to be the same as width)

      the vectors a-c and b-d are parallel and orthogonal to the vector p2-p1

      but p2-p1 doesn't need to be parallel to the x or the y axes
   */
  void keep_players_in_quadrangle(Vector p1, Vector p2, Value width, Value width2);

  /** a halfplane is specified be a point [pos] on it's boundary and by an angle. 
      all vectors belonging to the halfplane are then:

      pos + k * vec; with vec.arg() between [ang] and [ang + PI]
   */
  void keep_players_in_halfplane(Vector pos, ANGLE ang1);

  /** a halfplane can also be defined by a point [pos] on it's boundary and by
      the normal vector [normal_vec] pointing towards the interior of the 
      half plane. |normal_vec| = 1 is NOT required!!!
   */
  void keep_players_in_halfplane(Vector pos, Vector normal_vec);


  /** a cone is specified by his focal point [pos] and the two angles
      all vectors belonging to the cone are then

      pos + k * vec; with vec.arg() between [ang1] and [ang2] (going counterclockwise from ang1 to ang2)
   */
  void keep_players_in_cone(Vector pos, ANGLE ang1, ANGLE ang2);
  
  /** a cone can also be defined by his focal point [pos] and the two directions,
      so that all vectors belonging to the cone have the form:

      pos + k * vec; with vec.arg() between [dir1.arg()] and [dir2.arg()] (going counterclockwise from ang1 to ang2)
      
             / dir2
            /
           /
          /
       p /
         \
          \
           \
            \
             \ dir1

      dir1.norm() and dir2.norm() are NOT relevant.
   */
  void keep_players_in_cone(Vector pos, Vector dir1, Vector dir2);

  /** a cone can also be defined by his focal point [pos],its direction,
      and the width of its opening angle

             /
            /
           /
          /
       p /_________  dir
         \
          \
           \
            \
             \


     dir1.norm() is NOT relevant.
   */
  void keep_players_in_cone(Vector pos, Vector dir, ANGLE ang);

protected: //kept protected until nobody needs it
  /** keeps players with pass_info.valid == true  */
  void keep_players_with_pass_info(); //see WSinfo::get_teammate_with_newest_pass_info()
protected: //some useful tools but not part of the interface
  /**
     increasing sorting, the smallest elements in front and just how_many of them will be kept
   */
  void keep_and_sort(int how_many, Value * measured_data);


};



#endif
