#include <stdio.h>

main(int argc, char * argv[])

{
	int n;
	int root_env_getval_i(char *, int);
	int root_env_getval_x(char *, int);
	int root_env_getval_f(char *, double);
	int root_env_getval_s(char *, char *);
	int root_env_getval_b(char *, int);
	
		
	n = root_env_read(".mbsrc");
	if (n < 0)
	{
		fprintf(stderr, "File \".mbsrc\" not found\n");
		exit(1);
	}
	
	switch (*argv[1]) {
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
