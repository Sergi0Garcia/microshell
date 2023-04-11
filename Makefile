
NAME = microshell

run: 
	gcc -Wall -Werror -Wextra main2.c -o $(NAME)

del: 
	rm -rf $(NAME)