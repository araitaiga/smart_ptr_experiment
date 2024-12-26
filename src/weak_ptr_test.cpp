#include <iostream>
#include <memory>
#include <vector>

// 親クラスのthisポインタを, メンバオブジェクトのクラス（サブクラス）に渡す場合は, サブクラス側で親クラスのポインタをweak_ptrで受けないとダメ
// そうしないと, 親クラスのshared_ptrの参照カウントが増えてしまい, 両クラスのメモリが開放されない

class ChildShared;
class ChildWeak;
class Parent : public std::enable_shared_from_this<Parent>
{
public:
  Parent() { std::cout << "Parent Constructor" << std::endl; }
  void configure()
  {
    // shared_childshared_ptr = std::make_shared<ChildShared>(shared_from_this(), "shared"); // NG
    unique_childshared_ptr = std::make_unique<ChildShared>(shared_from_this(), "unique");  // NG
    // shared_childweak_ptr = std::make_shared<ChildWeak>(shared_from_this(), "shared"); // OK
    // unique_childweak_ptr = std::make_unique<ChildWeak>(shared_from_this(), "unique");  // OK
  }

  void chat() { std::cout << "Parent chat" << std::endl; }
  ~Parent() { std::cout << "[Parent Destructor!!!]" << std::endl; }

private:
  ////////////////////////////////////
  // 一番ダメ
  std::shared_ptr<ChildShared> shared_childshared_ptr;
  ////////////////////////////////////

  ////////////////////////////////////
  std::unique_ptr<ChildShared> unique_childshared_ptr;
  // unique_ptr でサブクラスを管理しても両者のデストラクタが呼ばれない!!! サブクラス側で親クラスのポインタをweak_ptrで受けないとダメ
  // main処理中にParentをデストラクトしようとする --> ChildShared内のParent参照カウントが残っているため, Parentのデストラクタが呼ばれない --> ChildSharedのデストラクタも呼ばれない(unique管理, shared管理によらない) --> Parentの参照カウントが減らない
  // 重要なのは, Parentの参照カウントを保持しているメンバが, Parentのデストラクタで解放されようとしていること. すると両者とも解放できなくなる
  ////////////////////////////////////

  // OK
  std::shared_ptr<ChildWeak> shared_childweak_ptr;
  std::unique_ptr<ChildWeak> unique_childweak_ptr;  // OK
};

// NG!!!
class ChildShared
{
public:
  ChildShared(std::shared_ptr<Parent> parent, const std::string & doc)
  : parent_ptr(parent), doc(doc)
  {
    std::cout << "ChildShared Constructor " << doc << std::endl;
    parent_ptr->chat();
  }

  ~ChildShared() { std::cout << "[ChildShared Destructor!!!] " << doc << std::endl; }

private:
  // 循環参照!!! Destructorが呼ばれないので, shared_ptrでなく weak_ptrを使うべき
  std::shared_ptr<Parent> parent_ptr;
  std::string doc;
};

class ChildWeak
{
public:
  ChildWeak(std::shared_ptr<Parent> parent, const std::string & doc) : parent_ptr(parent), doc(doc)
  {
    std::cout << "ChildWeak Constructor " << doc << std::endl;
    parent_ptr.lock()->chat();
  }

  ~ChildWeak() { std::cout << "[ChildWeak Destructor!!!] " << doc << std::endl; }

private:
  std::weak_ptr<Parent> parent_ptr;
  std::string doc;
};

int main()
{
  // 循環参照が発生する場合がある
  {
    std::cout << "=============[weak_ptr test]=============" << std::endl;
    auto parent = std::make_shared<Parent>();
    parent->configure();
  }
  std::cout << "=============[finish]=============" << std::endl;
  return 0;
}
