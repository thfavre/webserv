# Color codes
COLOR_RESET = \033[0m
COLOR_GREEN = \033[1;32m
COLOR_YELLOW = \033[1;33m
COLOR_CYAN = \033[1;36m
COLOR_RED = \033[1;31m
COLOR_ORANGE = \033[1;38;5;208m

# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror #-std=c++98

# Source files
SOURCES = srcs/webserv.cpp $(wildcard srcs/request/*.cpp) $(wildcard srcs/server/*.cpp) $(wildcard srcs/Configuration/*.cpp) $(wildcard srcs/response/*.cpp)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = webserv

all: $(TARGET)
	@echo "$(COLOR_GREEN)Compilation completed successfully!$(COLOR_RESET)"

$(TARGET): $(OBJECTS)
#	 @echo "$(COLOR_YELLOW)Compiling srcs/main.cpp..$(COLOR_RESET)"
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "$(COLOR_CYAN)Linking $(TARGET)...$(COLOR_RESET)"

.PHONY: all clean fclean re

clean:
	@rm -f $(OBJECTS)
	@echo "$(COLOR_ORANGE)Object files removed!$(COLOR_RESET)"

fclean: clean
	@rm -f $(TARGET)
	@echo "$(COLOR_RED)Executable removed!$(COLOR_RESET)"

re: fclean all
