#include "ValidationNode.hpp"

ValidationNode::ValidationNode(ValClass* validation){
  val = validation;
  result = 0;

}

ValidationNode::~ValidationNode()
{
    delete val;
}

ValClass* ValidationNode::getValidation(void){
  return val;
}

bool ValidationNode::getResult(){
  return result;
}
void ValidationNode::setResult(bool res){
  result=res;
}
