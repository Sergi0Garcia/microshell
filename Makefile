
NAME = microshell

run: 
	gcc -Wall -Werror -Wextra tmp.c -o $(NAME)

del: 
	rm -rf $(NAME)