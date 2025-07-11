
#ifndef _IMCodeGen_H_
#define _IMCodeGen_H_

#include "STL.h"

class CSyntaxTree;
class IOPCode;
typedef list<IOPCode*>	IMCODES;
typedef map<int, IMCODES*>	FUNCTIONS;

void	GenerateCode( CSyntaxTree &, IMCODES &, FUNCTIONS & );

#endif