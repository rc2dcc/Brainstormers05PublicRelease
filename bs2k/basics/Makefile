.SUFFIXES: .c $(SUFFIXES)

CXX		= g++ 
#CXX		= gcc
CXXINCLUDE	= -I. -I../../lib  -I../moves -I../policy 
#CXXFLAGS	= -Wall 
#CXXFLAGS	= -g3

SRCS		= $(wildcard *.c)
OBJS		= $(SRCS:.c=.o) 


libbasics.a:	$(OBJS)
	@echo ${OSMAKE} ${VERSION}
	ar rc  $@ $(OBJS) 
	ranlib $@
	@(echo "All object files generated and packed in a library")


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
	rm -f $(OBJS) *.[ad] 



