SRC		=	$(addprefix src/,	main.cpp\
								Server.cpp\
								Connection.cpp\
								VHost_start.cpp\
								VHost_processing.cpp\
								ConnectonPool.cpp\
								Parser.cpp\
								Request.cpp\
								Responce.cpp\
								Cgi.cpp\
								Header.cpp\
								utils.cpp)

OBJDIR	=	obj
SRCDIR	=	src
OBJ		=	$(addprefix  obj/, $(notdir  $(SRC:.cpp=.o)))
NAME	=	webserv
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -MD -MP -std=c++98 -I include
DEPENDS :=	$(addprefix  obj/, $(notdir  $(SRC:.cpp=.d)))



.PHONY			:	all re clean fclean

all				:	$(NAME)

$(NAME)			:	$(OBJ) Makefile
					$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o	:	$(SRCDIR)/%.cpp
					$(CC) $(CFLAGS) -c -o $@ $<

clean			:	
					rm -f $(OBJ) $(DEPENDS)

fclean			:	clean
					rm -f $(NAME)

x				:	all
					~/clean.sh
					./$(NAME) config.conf

leaks			:	all
					leaks --atExit -- ./$(NAME)

$(OBJ)			: | $(OBJDIR)

$(OBJDIR)		: 
					mkdir $(OBJDIR)

re				:	fclean all

-include $(DEPENDS)