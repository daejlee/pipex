
NAME = pipex
CFLAGS = -Wall -Wextra -Werror
CC = cc
SRC = pipex_utils.c pipex.c
OBJ = pipex_utils.o pipex.o
BONUS_SRC = ./bonus/pipex_utils_bonus.c ./bonus/pipex_bonus.c
BONUS_OBJ = ./bonus/pipex_utils_bonus.o ./bonus/pipex_bonus.o
LIBFT = ./libft/libft.a
LIBFT_DIR = ./libft

ifdef WITH_BONUS
	OBJ_FILES = $(BONUS_OBJ)
	SRC_FILES = $(BONUS_SRC)
else
	OBJ_FILES = $(OBJ)
	SRC_FILES = $(SRC)
endif

all : $(NAME)

$(NAME) : $(OBJ_FILES) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ_FILES) -lft -L$(LIBFT_DIR)

$(OBJ_FILES) : $(SRC_FILES)
	$(CC) $(CFLAGS) -c $(SRC_FILES)

$(LIBFT) :
	cd $(LIBFT_DIR); $(MAKE)

clean :
	rm -f $(OBJ_FILES) $(BONUS_OBJ)

fclean :
	rm -f $(OBJ_FILES) $(BONUS_OBJ) $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

bonus : $(NAME) $(BONUS_SRC)
	make WITH_BONUS=1 $(NAME)