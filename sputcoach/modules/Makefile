.SUFFIXES: .c $(SUFFIXES)

CXX		= g++
CXXINCLUDE	= -I. -I.. -I../../lib 

#CXXINCLUDE	= -I/sw/share/X11R6.4/include

CXXFLAGS	= -Wall 
#CXXFLAGS	= -Wall -Wno-implicit-function-declaration -Wno-return-type
#CXXFLAGS	= -Wformat -Wno-implicit-function-declaration\
		-Wimplicit-int -Wunused -Wuninitialized

SRCS		= $(wildcard *.c)
#SRCS		= artbot.C 

OBJS		=  $(SRCS:.c=.o)
#OBJS		= artcoach.o qtab.o

libmodules.a: $(OBJS)
	ar rc  $@ $(OBJS)
	ranlib $@
	@(echo "All object files generated and packed in a library")

#TARGET		= SputCoach
#$(TARGET): $(OBJS) 
#	$(CXX) -o $(TARGET) $(OBJS) $(CXXLIBS) 

#Qtab: qtab.o  
#	$(CXX) -o $@ $^ $(CXXLIBS)

#erzeugt Abhaengigkeiten und schreibt die in Dateien mit Endung *.d
%.d: %.c
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $(CXXINCLUDE) $< \
                 | sed '\''s/\($*\)\.o[ :]*/\1.o $@: /g'\'' > $@'
	@(echo "   $@ generated")

#beachte alle Abhaengigkeitsdateien
include $(SRCS:.c=.d)

.c.o:	
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<

clean:
	rm -f $(OBJS) $(SRCS:.c=.d) $(TARGET) *~ *.o *.a *.d 


