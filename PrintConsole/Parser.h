#ifndef _PARSER
#define _PARSER

#include "Keywords.h"

struct Node {

	Token token;
	unsigned num_children;
	Node* children[];
};

#endif // !_PARSER

