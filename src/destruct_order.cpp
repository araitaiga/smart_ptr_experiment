#include <iostream>
#include <memory>
#include <vector>

class ChildShared;
class ChildWeak;
class Parent
{
public:
  Parent() { std::cout << "Parent Constructor" << std::endl; }
  void configure()
  {
    // カスタムデリータを指定してshared_ptrを作成
    string_ptr = std::shared_ptr<std::string>(new std::string("Hoge!!!"), [](std::string * p) {
      std::cout << "[Call string_ptr Destructor!!!] " << *p << std::endl;
      delete p;
    });

    // OK. string_ptrのデストラクタが呼ばれる
    unique_childshared_ptr = std::make_unique<ChildShared>(string_ptr);
    // unique_childweak_ptr = std::make_unique<ChildWeak>(string_ptr);  // OK
  }

  ~Parent() { std::cout << "[Parent Destructor!!!]" << std::endl; }

private:
  std::shared_ptr<std::string> string_ptr;
  std::unique_ptr<ChildShared> unique_childshared_ptr;
  // std::unique_ptr<ChildWeak> unique_childweak_ptr;

  // 宣言順序を変えても正しくデストラクトされる
  // std::shared_ptr<std::string> string_ptr;
  // 先にParent::string_ptrが破棄される場合, string_ptrのカウントが1に --> ChildSharedの破棄の実行中, string_ptrのリソース解放を完了できる
  // 先にChildSharedが破棄される場合, やはりstring_ptrのカウントが1に --> Parent::string_ptrの破棄を完了できる
};

class ChildShared
{
public:
  ChildShared(std::shared_ptr<std::string> parent_str) : parent_str_(parent_str)
  {
    std::cout << "ChildShared Constructor " << *parent_str_ << std::endl;
  }

  ~ChildShared() { std::cout << "[ChildShared Destructor!!!] " << *parent_str_ << std::endl; }

private:
  std::shared_ptr<std::string> parent_str_;
};

class ChildWeak
{
public:
  ChildWeak(std::shared_ptr<std::string> parent_str) : parent_str_(parent_str)
  {
    std::cout << "ChildWeak Constructor " << *parent_str_.lock() << std::endl;
  }

  ~ChildWeak() { std::cout << "[ChildWeak Destructor!!!] " << *parent_str_.lock() << std::endl; }

private:
  std::weak_ptr<std::string> parent_str_;
};

int main()
{
  {
    std::cout << "=============[weak_ptr test]=============" << std::endl;
    auto parent = std::make_shared<Parent>();
    parent->configure();
  }
  std::cout << "=============[finish]=============" << std::endl;
  return 0;
}
