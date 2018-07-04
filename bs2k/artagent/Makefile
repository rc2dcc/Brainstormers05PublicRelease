.SUFFIXES: .C $(SUFFIXES)

CXX		= g++

CXXINCLUDE	= -I../../lib -I../basics -I../policy -I../behaviors
#-I../moves
CXXLIBS		= -L../../lib -lbs -L../basics -lbasics -L../policy -lpolicy
#-L../moves -lmoves

# Set global CXXFLAGS settings here!
# default (old) setting:
#CXXFLAGS	= -Wall -g
# optimized setting:
CXXFLAGS	= -Wall -O2   -march=i686 -mcpu=i686 -pipe
# setting for athlon XP CPUs (working with gcc3 only!):
#CXXFLAGS	= -Wall -O3 -g0 -march=athlon-xp -mcpu=athlon-xp -pipe
export CXXFLAGS

#SRCS		= $(wildcard *.c)
SRCS		= client.c sensorbuffer.c sensorparser_gen.c sensorparser.c wmstate.c wmoptions.c wmtools.c serverparam.c bit_fifo.c messages_times.c wmstats.c

OBJS		=  $(SRCS:.c=.o) 
#OBJS		= client.o 

TARGET		= BS2kAgent
$(TARGET): heck

lib: $(SRCS:.c=.o)
	@(echo -e "---------------------------------\n[../basics/$(MAKE)]:";\
          cd ../basics; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../moves/$(MAKE)]:";\
          cd ../moves; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../policy/$(MAKE)]:";\
          cd ../policy; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")

	$(CXX) -o $(TARGET) $(OBJS) $(N++DIR)/linux_lib/n++.o $(CXXLIBS) $(CXXLIBS2)
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")

lib2: $(SRCS:.c=.o)
	@(echo -e "---------------------------------\n[../basics/$(MAKE)]:";\
          cd ../basics; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../moves/$(MAKE)]:";\
          cd ../moves; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../policy/$(MAKE)]:";\
          cd ../policy; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")

	$(CXX) -o $(TARGET) $(OBJS) $(N++DIR)/linux_lib/n++.o ../basics/libbasics.a ../policy/libpolicy.a ../moves/libmoves.a
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")

heck: $(OBJS)
	@(echo -e "---------------------------------\n[../../lib/$(MAKE)]:";\
          cd ../../lib; $(MAKE))
	@(echo -e "---------------------------------\n[../basics/$(MAKE)]:";\
          cd ../basics; $(MAKE))
	@(echo -e "---------------------------------\n[../behaviors/skills/$(MAKE)]:";\
          cd ../behaviors/skills; $(MAKE))
	@(echo -e "---------------------------------\n[../behaviors/view/$(MAKE)]:";\
          cd ../behaviors/view; $(MAKE))
	@(echo -e "---------------------------------\n[../behaviors/$(MAKE)]:";\
          cd ../behaviors; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
#	@(echo -e "---------------------------------\n[../moves/$(MAKE)]:";\
#          cd ../moves; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../policy/$(MAKE)]:";\
          cd ../policy; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	$(CXX) -o BS2kAgent $(OBJS) ../basics/*.o -L../behaviors -lbehaviors -L../policy -lpolicy -L../../lib -lbs 
#-L../moves/ -lmoves 
#	$(CXX) -pg -o BS2kAgent $(OBJS)  -L../moves/ -lmoves -L../policy -lpolicy -L../../lib -lbs 
#	$(CXX) -pg -o BS2kAgent $(OBJS) ../basics/*.o ../moves/*.o ../policy/*.o -L../../lib -lbs 
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")


Sensorparser:  sensorparser.o sensorbuffer.o wmtools.o bit_fifo.o ../basics/server_options.o wmoptions.o
	g++ -o $@ $^ -L../../lib -lbs

Bit_fifo:  bit_fifo.o 
	g++ -o $@ $^ -L../../lib -lbs


#erzeugt Abhaengigkeiten und schreibt die in Dateien mit Endung *.d
%.d: %.c
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) $(CXXINCLUDE) $< \
                 | sed '\''s/\($*\)\.o[ :]*/\1.o $@: /g'\'' > $@'
	@(echo "   $@ generated")

#beachte alle Abhaengigkeitsdateien
include $(SRCS:.c=.d)

lex.si.o:
	echo "do nothing"

%.o: %.c	
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDE) $<

clean:
	rm -f $(OBJS) $(SRCS:.c=.d) $(TARGET) *~ 


