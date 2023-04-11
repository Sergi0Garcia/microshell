
NAME = microshell

run: 
	gcc -Wall -Werror -Wextra main.c -o $(NAME)

del: 
	rm -rf $(NAME)