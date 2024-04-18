# provide -I flags to build the list of #include locations for every header
CFLAGS = -g -Wall -std=c++20 -I src/interpreter -I src/io-handlers -I src/tokenization -I src/tree-parsing/utilities -I src/tree-parsing
CXX = g++
OBJDIR = obj
SRCDIR = src
BINDIR = bin
TARGET = app

# find all .cpp files in `src`
SRCFILES := $(shell find $(SRCDIR) -name '*.cpp')

# Convert .cpp files to .o files in `obj`
OBJFILES := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))

# Build all to `app` by default
all: $(TARGET)

# link obj files to create `app`
$(TARGET): $(OBJFILES)
	$(CXX) $(CFLAGS) $^ -o $@

# automatically generate .d dependency files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -c $< -o $@

# include dependencies
-include $(OBJFILES:.o=.d)