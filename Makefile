# Makefile for APB VCD Analyzer

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

# Directories
INCDIR = inc
SRCDIR = src
BINDIR = bin
OBJDIR = obj

# Target binary
TARGET = $(BINDIR)/vcd_analyzer

# Source files
SRCS = main.cpp \
       $(SRCDIR)/VCDParser.cpp \
       $(SRCDIR)/TransactionAnalyzer.cpp \
       $(SRCDIR)/HazardManager.cpp \
       $(SRCDIR)/SignalHistory.cpp \
       $(SRCDIR)/CompleterMapper.cpp \
       $(SRCDIR)/outputwriter.cpp

# Object files (stored in obj/)
OBJS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRCS)))

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp to .o
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Ensure obj directory exists
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Clean build files
clean:
	rm -rf $(BINDIR) $(OBJDIR)

.PHONY: all clean


## # Makefile for APB VCD Analyzer
## 
## # Compiler and flags
## CXX = g++
## CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
## 
## # Directories
## INCDIR = inc
## SRCDIR = src
## BINDIR = bin
## 
## # Target binary
## TARGET = $(BINDIR)/vcd_analyzer
## 
## # Source and object files
## SRCS = main.cpp $(SRCDIR)/VCDParser.cpp $(SRCDIR)/TransactionAnalyzer.cpp \
##        $(SRCDIR)/HazardManager.cpp $(SRCDIR)/SignalHistory.cpp \
##        $(SRCDIR)/CompleterMapper.cpp $(SRCDIR)/outputwriter.cpp
## # old OBJS = $(SRCS:.cpp=.o)
## # new objs to put everything in one obj folder
## OBJDIR = obj
## OBJS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRCS)))
## 
## # Default target
## all: $(TARGET)
## 
## $(TARGET): $(SRCS)
## 	@mkdir -p $(BINDIR)
## 	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o $@ $^
## 
## # Clean build files
## clean:
## 	rm -rf $(BINDIR) *.o $(SRCDIR)/*.o
## 
## .PHONY: all clean