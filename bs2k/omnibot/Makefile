.SUFFIXES: .C $(SUFFIXES)

N++DIR		= ${NPP_DIR}

CXX		= g++
LEX		= flex
YACC		= bison -d

CXXINCLUDE	= -I ../basics -I ../moves -I ../policy -I $(N++DIR)/src

CXXLIBS		=  -L../basics -lbasics

CXXLIBS2	= -L../moves -lmoves\
		-L../policy -lpolicy

CXXFLAGS	= $(CXXADDFLAGS) -Wall


#SRCS		= $(wildcard *.c)
SRCS		= client.c udpsocket.c sensorbuffer.c si.tab.c wmstate.c angle.c geometry2d.c

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
	@(echo -e "---------------------------------\n[../basics/$(MAKE)]:";\
          cd ../basics; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../moves/$(MAKE)]:";\
          cd ../moves; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	@(echo -e "---------------------------------\n[../policy/$(MAKE)]:";\
          cd ../policy; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")
	$(CXX) -o BS2kAgent $(OBJS) $(N++DIR)/linux_lib/n++.o ../basics/*.o ../moves/*.o ../policy/*.o
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")

si.tab.c: sensor_parser.y lex.si.c
	$(YACC) -b si -p si sensor_parser.y
lex.si.c: sensor_lexer.lex
	$(LEX) sensor_lexer.lex

Parser:  sensorbuffer.o si.tab.o
	g++ -o $@ $^ $(CXXLIBS)

mdpAgent: $(SRCS:.c=.o)
	@(echo -e "---------------------------------\n[../basics/$(MAKE)]:";\
          cd ../basics; $(MAKE))
	@(echo -e "---------------------------------\n[$(MAKE)]:")

	$(CXX) -o $(TARGET) $(OBJS)  $(CXXLIBS) 
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")


Posmatch: posmatch.o
	$(CXX) -o $@ $^  $(CXXLIBS) 
	@(echo -e ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n$(TARGET) successfully generated")


receiver: receiver.o
	$(CXX) -o $@ $^ $(CXXLIBS)

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


