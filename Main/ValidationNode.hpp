#include "valClass.hpp"
#include <iostream>



class ValidationNode {
private:
  ValClass* val;
  bool result;
public:
  ValidationNode(ValClass*);
  ValClass* getValidation(void);
  bool getResult();
  void setResult(bool);



};
