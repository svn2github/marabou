#ifndef __UseHeap_h__
#define __UseHeap_h__

#include "TList.h"

#define USE_HEAP()		TList _HEAP
#define TO_HEAP(x)		_HEAP.AddFirst((TObject *) x)
#define CLEAR_HEAP()	_HEAP.Delete()

#endif
