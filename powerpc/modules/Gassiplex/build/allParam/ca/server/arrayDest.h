#ifndef ARRAYDEST_H
#define ARRAYDEST_H

#include <gdd.h>

class ArrayDestructor : public gddDestructor {
	virtual void run(void *);
};

class ArrayIntDestructor : public ArrayDestructor {
	virtual void run(void *);
};

class ArrayStringDestructor : public ArrayDestructor {
	virtual void run(void *);
};

#endif /* !ARRAYDEST_H */

