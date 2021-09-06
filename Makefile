NAME		=	so_long
CC			=	clang $(C_FLAGS) $(MEM)
C_FLAGS		=	-g -Wall -Wextra #-Werror $(MEM)
MEM			=	-fsanitize=address

LIB_FLAGS	=	-lX11 -lXext -lm -lmlx
#LIB_FLAGS	=	-Lmlx_linux -L/usr/lib -Iminilibx-linux -lXext -lX11 -lm -lz
MAKE_LIB	=	make -s -C
MLX_DIR		=	minilibx-linux/
MLX			=	minilibx-linux/libmlx_Linux.a

RM			=	rm -rf

SRC_DIR		=	src/
SRC_FILES	=	animate_sprites.c \
				clear_exploding_bomb.c \
				draw_exploding_bomb.c \
				ennemies_ai.c \
				exit_game.c \
				init.c \
				init_sprites.c \
				init_utils.c \
				keypress.c \
				load_textures.c \
				main.c \
				map_parsing.c \
				map_parsing_items.c \
				map_parsing_populate.c \
				render.c \
				set_bomb.c \
				system_calls.c \
				texture_interactions.c \
				utils.c \
				put_to_window.c

SRC			=	$(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_DIR		=	obj/
OBJ_FILES	=	$(SRC_FILES:.c=.o)
OBJ			=	$(addprefix $(OBJ_DIR), $(OBJ_FILES))

all: $(NAME)

mlx:
				@echo -n "\n"
				@echo -n "NOW COMPILING MINILIBX..."
				@$(MAKE_LIB) $(MLX_DIR) > /dev/null 2>&1
				@echo "\033[32m\t\t[OK]\033[0m\n"

$(NAME): mlx $(OBJ)
				$(CC) $(LIB_FLAGS) $(OBJ) $(MLX) libft/libft.a -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
				@mkdir -p obj/
				$(CC) -c $< -o $@
				#@$(CC) -c $< -o $@ > /dev/null 2>&1

clean:
				@make -s clean -C $(MLX_DIR)
				$(RM) $(OBJ_DIR)

fclean: clean
				$(RM) $(MLX) $(NAME)

re: clean all

.PHONY: all clean fclean re
