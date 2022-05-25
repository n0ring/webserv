SRC		=	$(addprefix src/, main.cpp Server.cpp Poll.cpp Connection.cpp)
OBJDIR	=	obj
SRCDIR	=	src
OBJ		=	$(addprefix  obj/, $(notdir  $(SRC:.cpp=.o)))

NAME	=	webserv
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -MD -MP -std=c++98
DEPENDS :=	$(addprefix  obj/, $(notdir  $(SRC:.cpp=.d)))



.PHONY	:	all re clean fclean

all		:	$(NAME)

$(NAME)	:	$(OBJ) Makefile
			$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o		:	$(SRCDIR)/%.cpp
			$(CC) $(CFLAGS) -c -o $@ $<

clean	:	
			rm -f $(OBJ) $(DEPENDS)

fclean	:	clean
			rm -f $(NAME)

x		:	all
			./$(NAME)

leaks	:	all
			leaks --atExit -- ./$(NAME)

re		:	fclean all

-include $(DEPENDS)