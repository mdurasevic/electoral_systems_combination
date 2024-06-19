#include <ecf/ECF.h>

class Iflt: public Tree::Primitives::Primitive
{
public:
	Iflt(void);
	void execute(void* result, Tree::Tree& tree);
	~Iflt(void);
};


Iflt::Iflt(void)
{
	nArguments_ = 4;
	name_ = "iflt";
}




Iflt::~Iflt(void)
{	}


void Iflt::execute(void* result, Tree::Tree& tree)
{

}