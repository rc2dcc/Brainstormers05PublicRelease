.SUFFIXES: .C $(SUFFIXES)

CXX		= g++
CXXINCLUDE	= 

CXXLIBS		= -lm 

CXXFLAGS	= -Wall -g0 -O2 -march=i686 -mcpu=i686 -pipe

SRCS		= angle.c valueparser.c udpsocket.c str2val.c Vector.c n++.c \
                  lattice_map.c joystick.c txt_log.c txt_filter_bs.c pfilter.c random.c normal_dist2d.c \
		  geometry2d.c
		  
OBJS		= $(SRCS:%.c=%.o)

libbs.a: $(OBJS)
	ar rc libbs.a $(OBJS)
	ranlib libbs.a

geometry: geometry2d.o Vector.o angle.o
	$(CXX) -o $@ $^ $(CXXLIBS)

#erzeugt Abhaengigkeiten und schreibt die in Dateien mit Endung *.d
%.d: %.c
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $(CXXINCLUDE) $< \
                 | sed '\''s/\($*\)\.o[ :]*/\1.o $@: /g'\'' > $@'
	@(echo "   $@ generated")

#beachte alle Abhaengigkeitsdateien
include $(SRCS:.c=.d)

%.o: %.c	
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<

clean:
	rm -f $(OBJS) libbs.a



