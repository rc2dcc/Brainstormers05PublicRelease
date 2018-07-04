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

#ifndef _MACRO_MSG_H_
#define _MACRO_MSG_H_

#include <iostream>

#ifndef ERROR_STREAM
#define ERROR_STREAM std::cerr
#endif

#ifndef ERROR_OUT
#define ERROR_OUT ERROR_STREAM << "\n\n*** ERROR file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#ifndef WARNING_STREAM
#define WARNING_STREAM std::cerr
#endif

#ifndef WARNING_OUT
#define WARNING_OUT WARNING_STREAM << "\n\n*** WARNING file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#ifndef INFO_STREAM
#define INFO_STREAM std::cout
#endif

#ifndef INFO_OUT
#define INFO_OUT INFO_STREAM << "\n\n*** INFO file=\"" << __FILE__ << "\" line=" << __LINE__
#endif

#endif
