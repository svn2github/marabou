#include <stdio.h>
#include "root_env.h"

#include "Version.h"

main(int argc, char * argv[])

{
	int idx, n, ntot;

	idx = 1;
	while (idx < argc) {
		switch (*argv[idx]) {
			case 'r':
				n = root_env_read(argv[idx + 1]);
				if (n < 0) fprintf(stderr, "File \"%s\" not found\n", argv[idx + 1]);
				printf("There are %d entries now stored in the environment\n", n);
				break;
			case 'i':
				printf("%s: %d\n", argv[idx + 1], root_env_getval_i(argv[idx + 1], 0));
				break;
			case 'x':
				printf("%s: %#lx\n", argv[idx + 1], root_env_getval_x(argv[idx + 1], 0));
				break;
			case 'f':
				printf("%s: %f\n", argv[idx + 1], root_env_getval_f(argv[idx + 1], 0.));
				break;
			case 's':
				printf("%s: %s\n", argv[idx + 1], root_env_getval_s(argv[idx + 1], "none"));
				break;
			case 'b':
				printf("%s: %d\n", argv[idx + 1], root_env_getval_b(argv[idx + 1], 0));
				break;
			case 'p':
				root_env_print();
				break;
		}
		idx += 2;
	}
}	
