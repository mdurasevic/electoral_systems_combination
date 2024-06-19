#include <ecf/ECF.h>

class Avg: public Tree::Primitives::Primitive
{
public:
	Avg(void);
	void execute(void* result, Tree::Tree& tree);
	~Avg(void);
};


Avg::Avg(void)
{
	nArguments_ = 2;
	name_ = "avg";
}




Avg::~Avg(void)
{	}


void Avg::execute(void* result, Tree::Tree& tree)
{

}