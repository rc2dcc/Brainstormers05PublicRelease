.SUFFIXES: .c $(SUFFIXES)

CXX		= g++ 
CXXINCLUDE	= -I. -I../../lib -I../basics -I../moves 
#CXXFLAGS	= -Wall 
#CXXFLAGS	= -g3

SRCS		= $(wildcard *.c)
OBJS		= $(SRCS:.c=.o) 


libpolicy.a: $(OBJS)
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

.c.o:	
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<

clean:
	rm -f $(OBJS) *.[ad]



