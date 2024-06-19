#include <ecf/ECF.h>

class Sqr: public Tree::Primitives::Primitive
{
public:
	Sqr(void);
	void execute(void* result, Tree::Tree& tree);
	~Sqr(void);
};


Sqr::Sqr(void)
{
	nArguments_ = 4;
	name_ = "sqr";
}




Sqr::~Sqr(void)
{	}


void Sqr::execute(void* result, Tree::Tree& tree)
{

}