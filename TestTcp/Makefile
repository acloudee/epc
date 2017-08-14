####### Compiler, tools and options

CC	= g++
CXX	= g++
CFLAGS	= -pipe -Wall -W -g -D_REENTRANT -D_LINUX_ -D_USE_NS_1
CXXFLAGS= -pipe -Wall -W -g -D_REENTRANT -D_LINUX_ -D_USE_NS_1

INCPATH	=  
LINKS	= g++
LFLAGS	= -g
PRJ_LIBS= $(IPPHONE_LIB)/Utility.a $(IPPHONE_LIB)/Network.a
OS_LIBS = -lpthread
LIBS	= $(PRJ_LIBS) $(OS_LIBS)
AR		= ar cqs
TAR		= tar -cf
GZIP	= gzip -9f
COPY	= cp -f
COPY_FILE = $(COPY) -p
COPY_DIR  = $(COPY) -pR
DEL		= rm -f
MOVE	= mv

####### Output directory

OBJECTS_DIR = ./tmpObj.tmp/
IPPHONE_LIB = /home/yao/wks/MMPhone/Lib/Linux9.0/General
IPPHONE_PROJ = /home/yao/wks/MMPhone

####### Files

MYAPP = TestTcpClient

HEADERS	= 
SOURCES = TestTcpClient.cpp
OBJECTS = TestTcpClient.o

MYAPP2 = TestTcpServer
TRSOURCES = TestTcpServer.cpp
TROBJECTS = TestTcpServer.o

MYAPP3 = ns
NANO_SOURCES = TestNano.cpp
NANO_OBJECTS = TestNano.o

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o: 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: $(MYAPP)

$(MYAPP) : $(OBJECTS)
	$(CXX) -o $(MYAPP) $(OBJECTS) -Wl,--start-group $(LIBS) -Wl,--end-group
	
ts : $(TROBJECTS)
	$(CXX) -o $(MYAPP2) $(TROBJECTS) -Wl,--start-group $(LIBS) -Wl,--end-group
	
nano : $(NANO_OBJECTS)
	$(CXX) -o $(MYAPP3) $(NANO_OBJECTS) -Wl,--start-group $(LIBS) -Wl,--end-group

depend:
	makedepend $(CXXFLAGS) -Y $(SOURCES)

clean:
	$(DEL) $(OBJECTS) $(MYAPP) 
