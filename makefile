
ALL: process_pool


process_pool:process_pool.c
	gcc $^ -o $@ -lpthread -lrt -g


