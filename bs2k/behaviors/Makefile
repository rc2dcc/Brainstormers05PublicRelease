.SUFFIXES: .c $(SUFFIXES)

CXX		= g++ 
#CXX		= gcc
CXXINCLUDE	= -I. -I../../lib  -I../basics -I../moves
#CXXFLAGS	= -Wall 
#CXXFLAGS	= -g3

SRCS		= $(wildcard *.c)
#ALL_OBJS	= $(wildcard *.o view/*.o wildcard skills/*.o)
ALL_SRCS        = $(wildcard *.c view/*.c wildcard skills/*.c)
ALL_OBJS        = $(ALL_SRCS:.c=.o)
OBJS		= $(SRCS:.c=.o) 
#VPATH		= view:skills



libbehaviors.a: $(OBJS)
	@(echo -e "---------------------------------\n[view/$(MAKE)]:";\
        cd view; $(MAKE))
	@(echo -e "---------------------------------\n[skills/$(MAKE)]:";\
        cd skills; $(MAKE))

	@echo ${OSMAKE} ${VERSION}
	ar rc  $@ $(ALL_OBJS) 
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



