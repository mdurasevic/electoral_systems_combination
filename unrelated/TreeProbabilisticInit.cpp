#include "TreeProbabilisticInit.h"
#include "StringUtils.h"

ProbabilisticPrimitiveGenerator Tree::ProbabilisticTree::generator;

Tree::ProbabilisticTree::ProbabilisticTree()
{
	name_ = "ProbabilisticTree";
	startDepth_ = 0;
}

bool Tree::ProbabilisticTree::initialize(StateP state) 
{
	if (!generator.initialised) {
		voidP sptr = getParameterValue(state, "maxdepth");
		generator.initialise(*((int*)sptr.get()));

		sptr = getParameterValue(state, "initialisationStrategy");
		generator.strategy = *((std::string*)sptr.get());
	}

	Tree::initialize(state);
	
	return true;
}

void Tree::ProbabilisticTree::registerParameters(StateP state)
{
	Tree::registerParameters(state);
	registerParameter(state, "initialisationStrategy",  (voidP) (new std::string), ECF::STRING, "Initialisation strategy");
}

uint Tree::ProbabilisticTree::growBuild(PrimitiveSetP primitiveSet, int myDepth)
{
	generator.state = this->state_;
	generator.primitiveSet = this->primitiveSet_;

	Node* node = new Node();
	node->depth_ = myDepth;

	if (node->depth_ < this->initMinDepth_) {
		//node->setPrimitive(primitiveSet->getRandomFunction());
		node->setPrimitive(generator.generateFunction(myDepth, string("")));

		this->addNode(node);
	}
	else if (node->depth_ < this->initMaxDepth_) {
		//node->setPrimitive(primitiveSet->getRandomPrimitive());
		node->setPrimitive(generator.generatePrimitive(myDepth, string("")));

		this->addNode(node);
	}
	else {
		//node->setPrimitive(primitiveSet->getRandomTerminal());
		node->setPrimitive(generator.generateTerminal(myDepth, string("")));

		this->addNode(node);
	}

	string treeContext = node->primitive_->getName();
	for (int i = 0; i < node->primitive_->getNumberOfArguments(); i++) {
		auto res = growBuild1(primitiveSet, myDepth + 1, treeContext);
		node->size_ += res.second;
		treeContext = treeContext + " " + res.first;
	}

	return node->size_;
}

uint Tree::ProbabilisticTree::fullBuild(PrimitiveSetP primitiveSet, int myDepth)
{
	generator.state = this->state_;
	generator.primitiveSet = this->primitiveSet_;

	Node* node = new Node();
	node->depth_ = myDepth;

	if (node->depth_ < this->initMaxDepth_) {
		//node->setPrimitive(primitiveSet->getRandomFunction());
		node->setPrimitive(generator.generateFunction(myDepth, string("")));
		this->addNode(node);
	}
	else {
		node->setPrimitive(generator.generateTerminal(myDepth, string("")));
		//node->setPrimitive(primitiveSet->getRandomTerminal());
		this->addNode(node);
	}

	string treeContext = node->primitive_->getName();

	for (int i = 0; i < node->primitive_->getNumberOfArguments(); i++) {
		auto res = fullBuild1(primitiveSet, myDepth + 1, treeContext);
		node->size_ += res.second;
		treeContext = treeContext + " " + res.first;
	}

	return node->size_;
}

pair<string, int>  Tree::ProbabilisticTree::growBuild1(PrimitiveSetP primitiveSet, int myDepth, string context)
{
	Node* node = new Node();
	node->depth_ = myDepth;


	if (node->depth_ < this->initMinDepth_) {
		//node->setPrimitive(primitiveSet->getRandomFunction());
		node->setPrimitive(generator.generateFunction(myDepth, context));

		this->addNode(node);
	}
	else if (node->depth_ < this->initMaxDepth_) {
		//node->setPrimitive(primitiveSet->getRandomPrimitive());
		node->setPrimitive(generator.generatePrimitive(myDepth, context));

		this->addNode(node);
	}
	else {
		//node->setPrimitive(primitiveSet->getRandomTerminal());
		node->setPrimitive(generator.generateTerminal(myDepth, context));

		this->addNode(node);
	}

	string treeContext = node->primitive_->getName();
	for (int i = 0; i < node->primitive_->getNumberOfArguments(); i++) {
		auto res = growBuild1(primitiveSet, myDepth + 1, treeContext);
		node->size_ += res.second;
		treeContext = treeContext + " " + res.first;
	}

	return make_pair(node->primitive_->getName(), node->size_);
}

pair<string, int>  Tree::ProbabilisticTree::fullBuild1(PrimitiveSetP primitiveSet, int myDepth, string context)
{
	Node* node = new Node();
	node->depth_ = myDepth;

	if (node->depth_ < this->initMaxDepth_) {
		//node->setPrimitive(primitiveSet->getRandomFunction());
		node->setPrimitive(generator.generateFunction(myDepth, context));
		this->addNode(node);
	}
	else {
		node->setPrimitive(generator.generateTerminal(myDepth, context));
		//node->setPrimitive(primitiveSet->getRandomTerminal());
		this->addNode(node);
	}

	string treeContext = node->primitive_->getName();

	for (int i = 0; i < node->primitive_->getNumberOfArguments(); i++) {
		auto res = fullBuild1(primitiveSet, myDepth + 1, treeContext);
		node->size_ += res.second;
		treeContext = treeContext + " " + res.first;
	}

	return make_pair(node->primitive_->getName(), node->size_);
}

Tree::Tree* Tree::ProbabilisticTree::copy()
{
	ProbabilisticTree *newObject = new ProbabilisticTree(*this);

	// create new nodes, copy primitives if necessary
	for (int i = 0; i < (int)this->size(); i++) {
		(*newObject)[i] = (NodeP)(new Node((*this)[i]));
	}
	return newObject;
}
