#include <stdio.h>

main(int argc, char * argv[])

{
	int idx, n, ntot;
	int append;

	int root_env_read(char *, int);
	int root_env_getval_i(char *, int);
	int root_env_getval_x(char *, int);
	int root_env_getval_f(char *, double);
	int root_env_getval_s(char *, char *);
	int root_env_getval_b(char *, int);
	
	append = 0;
	ntot = 0;
		
	idx = 1;
	while (argc) {
		switch (*argv[1]) {
			case 'r':
				n = root_env_read(argv[2], append);
				if (n < 0) {
					fprintf(stderr, "File \"%s\" not found\n", argv[2]);
				} else {
					ntot += n;
					append = 1;
				}
				printf("There are %d entries stored in the environment now\n", ntot);
				break;
		case 'i':
			printf("%s: %d\n", argv[2], root_env_getval_i(argv[2], 0));
			break;
		case 'x':
			printf("%s: %#lx\n", argv[2], root_env_getval_x(argv[2], 0));
			break;
		case 'f':
			printf("%s: %f\n", argv[2], root_env_getval_f(argv[2], 0.));
			break;
		case 's':
			printf("%s: %s\n", argv[2], root_env_getval_s(argv[2], "none"));
			break;
		case 'b':
			printf("%s: %d\n", argv[2], root_env_getval_b(argv[2], 0));
			break;
		}
	}
}	
