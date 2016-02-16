#include "ValidationNode.hpp"

ValidationNode::ValidationNode(ValClass* validation){
  val=validation;

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
