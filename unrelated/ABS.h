#include <ecf/ECF.h>

class Abs: public Tree::Primitives::Primitive
{
public:
	Abs(void);
	void execute(void* result, Tree::Tree& tree);
	~Abs(void);
};


Abs::Abs(void)
{
	nArguments_ = 2;
	name_ = "abs";
}




Abs::~Abs(void)
{	}


void Abs::execute(void* result, Tree::Tree& tree)
{

}