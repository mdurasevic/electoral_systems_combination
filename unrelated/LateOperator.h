#include <ecf/ECF.h>

class LateOperator: public Tree::Primitives::Primitive
{
public:
	LateOperator(void);
	void execute(void* result, Tree::Tree& tree);
	~LateOperator(void);
};


LateOperator::LateOperator(void)
{
	nArguments_ = 2;
	name_ = "LATE";
}




LateOperator::~LateOperator(void)
{	}


void LateOperator::execute(void* result, Tree::Tree& tree)
{

}