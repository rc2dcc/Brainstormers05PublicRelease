CXXADDFLAGS = -g -pg
STARTDIR = "$$HOME/robocup/bs2k/RealTimeAgent"

xxx:
	cd artagent && make
all: tags cleanup profile

profile:
	(cd $(STARTDIR); \
	make CXXADDFLAGS='-g -pg';\
	cd ..)

clean:
	find . -name '*.[oad]' | xargs rm -f

cleanup: 
	find . -name '*.[oad]' | xargs rm -f

tags:
	etags --c++ {basics/,policy/,moves/,RealTimeAgent/,artagent/}*.[chy]