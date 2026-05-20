NAME = ft_ping

# Colors
GREEN = \033[1;32m
PURPLE = \033[1;35m
NC = \033[0m

# Header(s)
HEADERS_DIR = ./includes/

HEADERS_FILES = ft_ping.h

HEADERS = $(addprefix $(HEADERS_DIR), $(HEADERS_FILES))

# --- SRCS files and objs ---
SRCS_DIR = ./srcs/

SRCS_FILES = ft_ping.c\
	checksum.c\
	icmp.c\
	host.c\
	loop.c\
	exit.c\
	stats.c

SRCS = $(addprefix $(SRCS_DIR), $(SRCS_FILES))
OBJS = $(SRCS:.c=.o)

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm
RM_FLAGS = -rf

$(SRCS_DIR)%.o: $(SRCS_DIR)%.c Makefile $(HEADERS)
	@$(CC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -lm -o $(NAME)
	@echo "${GREEN}-> ${NAME} executable compiled successfully${NC}"

clean:
	@$(RM) $(RM_FLAGS) $(OBJS)
	@echo "${PURPLE}-> All ${NAME} objs files removed successfully${NC}"

fclean:
	@$(RM) $(RM_FLAGS) $(NAME) $(OBJS)
	@echo "${PURPLE}-> ${NAME} executable and objs removed successfully${NC}"

re: fclean all

.PHONY: all re fclean clean
