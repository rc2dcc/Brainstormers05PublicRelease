.SUFFIXES: .c $(SUFFIXES)

CXX		= g++
CXXINCLUDE	= -I. -I../lib -I./modules 

#CXXINCLUDE	= -I/sw/share/X11R6.4/include
CXXLIBS		= -L../lib -lbs -L./modules -lmodules

CXXFLAGS	=  -Wall -O2 -march=i686 -mcpu=i686 -pipe
#CXXFLAGS	= -Wall -Wno-implicit-function-declaration -Wno-return-type
#CXXFLAGS	= -Wformat -Wno-implicit-function-declaration\
		-Wimplicit-int -Wunused -Wuninitialized

SRCS		= $(wildcard *.c)

OBJS		=  $(SRCS:.c=.o) 

TARGET		= SputCoach
$(TARGET): heck

heck: $(OBJS)
	@(echo -e "\n-------------< Compiling Modules >-------------\n[./modules/$(MAKE)]:";\
	cd ./modules; $(MAKE))
	@(echo -e "\n--------------< Compiling Coach >--------------\n[$(MAKE)]:")

	$(CXX) -o $(TARGET) $(OBJS) -L../lib -lbs -L./modules -lmodules 

	@(echo -e "\n=====< SputCoach successfully generated >======\n") 


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
	find . -name '*.[oad]' | xargs rm -f


