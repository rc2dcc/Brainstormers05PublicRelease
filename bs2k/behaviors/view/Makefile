.SUFFIXES: .c $(SUFFIXES)

CXX		= g++ 
#CXX		= gcc
CXXINCLUDE	= -I. -I../../../lib  -I../../basics -I../../moves
#CXXFLAGS	= -Wall 
#CXXFLAGS	= -g3

SRCS		= $(wildcard *.c)
OBJS		= $(SRCS:.c=.o) 

target: $(OBJS)

#erzeugt Abhaengigkeiten und schreibt die in Dateien mit Endung *.d
%.d: %.c
	@echo  '$(CXX) -MM $(CXXFLAGS) $(CXXINCLUDE) $< \
                 | sed '\''s/\($*\)\.o[ :]*/\1.o $@: /g'\'' > $@'
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $(CXXINCLUDE) $< \
                 | sed '\''s/\($*\)\.o[ :]*/\1.o $@: /g'\'' > $@'
	@(echo "   $@ generated")

#beachte alle Abhaengigkeitsdateien
include $(SRCS:.c=.d)

%.o: %.c	
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<
	rm -f ../libbehaviors.a

clean:
	rm -f $(OBJS) *.[ad] 



