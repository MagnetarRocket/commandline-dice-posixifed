//bash -c 'gcc -O2 -Wall d.cpp `pkg-config --cflags --libs glib-2.0` -o ../d' 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <glib.h>
#include <errno.h>

typedef long long int randval_t;

void rand_init(void) {
	// 1-second precision is annoying
	// Even, run 'd' two times in one second and get the same results!
	// #include <time.h> ..... srand(time(NULL));
	struct timeval now;
	gettimeofday(&now, NULL);
	srand(now.tv_usec ^ now.tv_sec);
}

randval_t rand_val(void) {
	return ((randval_t)rand()) + ((randval_t)rand()>>16)
		+ ((randval_t)rand()<<16) + ((randval_t)rand()<<31);
}

void pr(void* k, void* v, void* ud) {
	printf(":%i %i\n", *(int*)k, *(int*)v);
}

int main(int argc, char** argv)
{
	rand_init();

	//In case of overflow, atoll returns max (long)long. huh.
	randval_t sides = atoll(argv[1]);
	if(sides < 0) {
		fprintf(stderr, "sides = %lli is too negative.", sides);
		exit(1);
	}
	int times = 1;
	bool factorial = false;
	if(argc >= 3) {
		char* nextc;
		errno = 0;
		times = strtol(argv[2], &nextc, 0);
		if(*nextc == '!') {
			factorial = true;
			if(nextc == argv[2]) {
				times = sides;
			}
			if(times > sides) {
				fprintf(stderr, "can't pigeonhole %i nums in %lli possibilities\n", times, sides);
				exit(1);
			}
		}
	}
	randval_t* results = (randval_t*)malloc(times * sizeof(randval_t));
	GHashTable* already = g_hash_table_new(&g_int_hash, &g_int_equal);
	for(int i = 0; i < times; ++i) {
		do {
			results[i] = rand_val() % sides + 1;
		} while(g_hash_table_lookup(already, &results[i]));
		g_hash_table_insert(already, &results[i], &results[i]);
		printf("%lld\n", results[i]);
	}
//	g_hash_table_foreach(already, pr, NULL);
	g_hash_table_destroy(already);
	free(results);
}
