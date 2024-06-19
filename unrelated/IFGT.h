#include <ecf/ECF.h>

class Ifgt: public Tree::Primitives::Primitive
{
public:
	Ifgt(void);
	void execute(void* result, Tree::Tree& tree);
	~Ifgt(void);
};


Ifgt::Ifgt(void)
{
	nArguments_ = 4;
	name_ = "ifgt";
}




Ifgt::~Ifgt(void)
{	}


void Ifgt::execute(void* result, Tree::Tree& tree)
{

}

