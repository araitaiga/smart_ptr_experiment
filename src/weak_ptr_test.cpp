#include <iostream>
#include <memory>
#include <vector>

#include "stop_watch.h"

class Parent;
class Child;

class Parent
{
public:
  std::shared_ptr<Child> child_ptr;
  ~Parent() { std::cout << "Parent Destructor" << std::endl; }
};

class Child
{
public:
  // 循環参照!!! shared_ptr でなく weak_ptr を使うべき
  std::shared_ptr<Parent> parent_ptr;
  ~Child() { std::cout << "Child Destructor" << std::endl; }
};

class Parent2;
class Child2;
class Parent2 : public std::enable_shared_from_this<Parent2>
{
public:
  Parent2() { std::cout << "Parent2 Constructor" << std::endl; }
  void configure() { child_ptr = std::make_shared<Child2>(shared_from_this()); }

  void chat() { std::cout << "Parent2 chat" << std::endl; }
  ~Parent2() { std::cout << "Parent2 Destructor" << std::endl; }

private:
  std::shared_ptr<Child2> child_ptr;
};

class Child2
{
public:
  // 循環参照!!! shared_ptr でなく weak_ptr を使うべき
  // Child2(std::shared_ptr<Parent2> parent) : parent_ptr(parent)
  // {
  //   std::cout << "Child2 Constructor" << std::endl;
  // }
  Child2(std::weak_ptr<Parent2> parent) : parent_ptr(parent)
  {
    std::cout << "Child2 Constructor" << std::endl;
    parent_ptr.lock()->chat();
  }
  ~Child2() { std::cout << "Child2 Destructor" << std::endl; }

private:
  // 循環参照!!! shared_ptr でなく weak_ptr を使うべき
  // std::shared_ptr<Parent2> parent_ptr;

  std::weak_ptr<Parent2> parent_ptr;
};

int main()
{
  // 互いのshared_ptrが参照を持ち合っているため、メモリが解放されない
  {
    std::cout << "=============[weak_ptr test]=============" << std::endl;
    auto parent = std::make_shared<Parent>();
    auto child = std::make_shared<Child>();
    parent->child_ptr = child;
    child->parent_ptr = parent;
  }
  std::cout << "=============[memories are not released]=============" << std::endl;

  // Chile2はParent2のweak_ptrを持つため、循環参照が解消される
  {
    std::cout << "=============[weak_ptr test2]=============" << std::endl;
    auto parent = std::make_shared<Parent2>();
    parent->configure();
  }
  std::cout << "=============[call destractor]=============" << std::endl;
  return 0;
}
