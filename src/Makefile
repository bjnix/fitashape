# Makefile for Irrlicht Examples
# It's usually sufficient to change just the target name and source file list
# and be sure that CXX is set to a valid compiler
MASTEXEC = fitashape_M
SLAVEXEC = fitashape_S
RELYEXEC = fitashape_R

RLYSOURCE = DGRRelay.cpp
Sources = Main.cpp Player.cpp Game.cpp CircleNode.cpp DGR_framework.cpp

FIT_INC = -I../include 
X11_INC = -I/usr/X11R6/include
#LOCAL_INC = -I./include

# general compiler settings
CPPFLAGS = $(X11_INC) $(FIT_INC)
CXXFLAGS = -std=c++0x -O3 -ffast-math -g -Wall -fpermissive

#default target is Linux


ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif

X11_LIBS= -L/usr/X11R6/lib$(LIBSELECT) -lGL -lXxf86vm -lXext -lX11 -lXcursor 
IRR_LIBS = -L../lib/irrlicht -lIrrlicht
VICON_LIBS = -L../lib/vicon-libs -Wl,-rpath,../lib/vicon-libs -lViconDataStreamSDK_CPP
# If sound enabled, add the proper library for linking
#LDFLAGS += -lIrrKlang
#LDFLAGS += -laudiere
#LDFLAGS += -lSDL_mixer -lSDL


# target specific settings
LDFLAGS = $(X11_LIBS) $(IRR_LIBS) $(VICON_LIBS)
# name of the binary - only valid for targets which set SYSTEM
#DESTPATH = ../bin/$(SYSTEM)/$(Target)$(SUF)
DESTPATH = ../bin
ODIRM = ./objMaster
ODIRS = ./objSlave
_OBJ = $(Sources:.cpp=.o)
OBJM = $(addprefix $(ODIRM)/,$(_OBJ)) 
OBJS = $(addprefix $(ODIRS)/,$(_OBJ)) 

all: $(MASTEXEC) $(SLAVEXEC) $(RELYEXEC)

$(ODIRM)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DDGR_MASTER=1 -c -o $@ $<

$(ODIRS)/%.o: %.cpp
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(MASTEXEC): $(OBJM)
	@echo "++"$(_OBJ)"++"
	@echo "++"$(OBJM)"++"
	@echo "=== COMPILING MASTER ==="
	$(CXX) -o $(DESTPATH)/$@ $^ $(CPPFLAGS) $(LDFLAGS)

$(SLAVEXEC): $(OBJS)
	@echo "++"$(OBJS)"++"  
	@echo "=== COMPILING SLAVE ==="
	$(CXX) -o $(DESTPATH)/$@ $^ $(CPPFLAGS) $(LDFLAGS)
	

$(RELYEXEC): $(RLYSOURCE)
	@echo "=== COMPILING RELAY ==="
	$(CXX) $(CXXFLAGS) $^ -o $(DESTPATH)/$@ $(CPPFLAGS) $(LDFLAGS)

clean: clean_objs clean_exes
	$(warning Cleaning...)

clean_objs:
	@$(RM) $(OBJM)/$(_OBJ) $(OBJS)/$(_OBJ) $(RLYSOURCE:.cpp=.o)

clean_exes:
	@$(RM) $(addprefix $(DESTPATH)/,$(MASTEXEC) $(SLAVEXEC) $(RELYEXEC))

.PHONY: all clean clean_objs clean_exes
