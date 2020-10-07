# Compilation Parameters.
THDIR=..
OBJDIR=../../build
BINDIR=../../bin
FONTS=$(wildcard *.cpp)
OBJECTS=$(FONTS:.cpp=.o)
OBJS_PATH=$(patsubst %,$(OBJDIR)/%,$(OBJECTS))
FLAGS=-std=c++11 -O3 -rdynamic -g3
BOOST_PATH=~

# Compilation rules.
.PHONY: all clean

all: mkdir_out TH_example_1TH_1alg TH_example_1TH TH_example_7TH

TH_example_1TH_1alg: $(OBJDIR)/TH_example_1TH_1alg.o $(OBJDIR)/RosenbrockFitnessPolicy.o
	mpic++ -o $(BINDIR)/$@ $^ -lm -ldl -lSegFault $(FLAGS)

TH_example_1TH: $(OBJDIR)/TH_example_1TH.o $(OBJDIR)/RosenbrockFitnessPolicy.o
	mpic++ -o $(BINDIR)/$@ $^ -lm -ldl -lSegFault $(FLAGS)

TH_example_7TH: $(OBJDIR)/TH_example_7TH.o $(OBJDIR)/RosenbrockFitnessPolicy.o
	mpic++ -o $(BINDIR)/$@ $^ -lm -ldl -lSegFault $(FLAGS)

$(OBJDIR)/%.o: %.cpp
	mpic++ -c $< -o $@ -I $(BOOST_PATH) -I $(THDIR) -Wall $(FLAGS)

$(OBJDIR)/RosenbrockFitnessPolicy.o: $(THDIR)/RosenbrockFitnessPolicy.cpp
	mpic++ -c $< -o $@ -I $(BOOST_PATH) -I $(THDIR) -Wall $(FLAGS)

mkdir_out:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

clean:
	rm $(OBJS_PATH) $(OBJDIR)/RosenbrockFitnessPolicy.o
