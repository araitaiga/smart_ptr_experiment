# ムーブ

## の前にコピー

main関数ではHeavyCopyのコピーコンストラクタは何回呼ばれる?

```cpp
class HeavyCopy
{
  // HeavyCopyはムーブコンストラクタを持たない
  HeavyCopy(std::string str);
...
};

int main()
{
  std::vector<HeavyCopy> hoge{HeavyCopy("1"), HeavyCopy("2"), HeavyCopy("3")}; 
  std::vector<HeavyCopy> fuga(hoge);
  return 0;
}
```

<details>
<summary>正解</summary>
6回. hogeの初期化で3回(HeavyCopyはムーブコンストラクタを持たず, コピーされる), fugaのコピーコンストラクタ内で更に3回コピー.
</details>

</br>

以下のコードでは?

```cpp
int main()
{
  std::vector<HeavyCopy> hoge;
  hoge.reserve(3);
  hoge.emplace_back(1);
  hoge.emplace_back(2);
  hoge.emplace_back(3);
  std::vector<HeavyCopy> fuga(std::move(hoge));
  return 0;
}
```

<details>
<summary>正解</summary>
0回. move! fugaはvectorのムーブコンストラクタで構築される. vectorが管理する要素へのポインタをコピーするだけなのでHeavyCopyのコピーコンストラクタは呼ばれない  

![vector_copy](./images/vector_copy.png)  
</details>

</br>


例: [vector.cpp](./src/vector.cpp)  

## コピーコンストラクタ

引数に同じクラスのconst左辺値参照を取り, メンバを"コピー"する  
引数のインスタンスは不変. (const &)  

```cpp
class HeavyCopy{
  char * ptr;
  HeavyCopy(const HeavyCopy & rhs)
  {
    ptr = new char[1000] ;
    std::copy(rhs.ptr, rhs.ptr + 1000, ptr);
  }
};
```

## ムーブコンストラクタ

引数に同じクラスの右辺値参照を取り, 引数のメンバの所有権を奪う  
引数のインスタンスは可変.  

```cpp
class HeavyCopyMove{
  char * ptr;
public:
  HeavyCopyMove(HeavyCopyMove && rhs)
  {
    // ポインタだけコピーする
    ptr = rhs.ptr;
    rhs.ptr = nullptr;
    return *this;
  }
};

```

例: [heavy_copy.cpp](./include/heavy_copy.h)

<https://cpprefjp.github.io/lang/cpp11/rvalue_ref_and_move_semantics.html>  
<https://theolizer.com/cpp-school1/cpp-school1-36/>

## 右辺値と左辺値

<https://cpprefjp.github.io/lang/cpp11/rvalue_ref_and_move_semantics.html>

```cpp

int main()
{
  int i = 0;

  i;      // 名前付きオブジェクトは左辺値
  0;      // リテラル値は右辺値

  Foo x ;

  x;      // 名前付きオブジェクトは左辺値
  Foo();  // コンストラクタの戻り値は右辺値

  f();    // 関数の戻り値は右辺値
}
```

## 右辺値参照

```T&&```. 右辺値のみを束縛する参照.  一方```T&```は左辺値参照.  
**受け取った実引数が右辺値だったか左辺値だったかを, 関数やコンストラクタが区別するためのフラグとして用いられる**

```cpp
void operateHeavyCopyMove(HeavyCopyMove && obj)
{
  //  引数に右辺値が渡された場合, こちらが呼び出される
  std::cout << "operate with rvalue reference\n";
}
void operateHeavyCopyMove(const HeavyCopyMove & obj)
{
  //  引数に左辺値が渡された場合, こちらが呼び出される
  std::cout << "operate with lvalue reference\n";
}

int main()
{
  HeavyCopyMove base_obj("1");
  std::cout << "=====Operate with LValue Reference===== \n";
  operateHeavyCopyMove(base_obj);

  std::cout << "=====Operate with RValue Reference===== \n";
  operateHeavyCopyMove(HeavyCopyMove("2"));
  operateHeavyCopyMove(std::move(base_obj));

  return 0;
}

```

右辺値は一時オブジェクトのため自由に破棄して良く, 左辺値はそうではない.  
--> 関数やコンストラクタ側がムーブにより所有権を奪うためには右辺値が必要.  

例: [rvalue_function.cpp](./src/rvalue_function.cpp)

## ムーブ

std::move(X)は実際には**ムーブしない**. 左辺値Xを右辺値にキャストするだけ.  
実際のムーブ(所有権の移動)はムーブコンストラクタ, ムーブ代入演算子により行われる.  

```cpp
int main()
{
  std::string x = "Hello, world!";
  // 何も起こらない
  std::move(x);

  // 実際にxからyへ文字列がムーブされる
  std::string y = std::move(x);
}
```

## コピー or ムーブ ?

以下のコードで行われるのはコピー or ムーブ?

```cpp

class HeavyCopy
{
  // Copyに時間のかかるクラス
private:
  std::string str{0};

public:
  HeavyCopy() = default;
  explicit HeavyCopy(const std::string str) : str(str)
  {
    std::cout << "HeavyCopy created " << str << std::endl;
  }
  HeavyCopy(const HeavyCopy & rhs) : str(rhs.str)
  {
    std::cout << "HeavyCopy start copy " << rhs.str << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "HeavyCopy copied" << std::endl;
  }
};
int main(){
  std::array<HeavyCopy, 3> hoge_array;
  HeavyCopy val1(1);
  HeavyCopy val2(2);
  HeavyCopy val3(3);
  hoge_array.at(0) = std::move(val1);
  hoge_array.at(1) = std::move(val2);
  hoge_array.at(2) = std::move(val3);

  return 0;
}
```

<details>
<summary>正解</summary>
copy. ムーブコンストラクタが定義されていないので, 右辺値を代入していてもムーブ代入演算子は呼ばれない
</details>

</br>

以下の例で, ```Hoge hoge2 = std::move(hoge1);```でメンバであるmapはムーブ？コピー？  

```cpp
class Hoge
{
private:
  std::map<std::string, HeavyCopyMove> map;

public:
  ~Hoge() { std::cout << "Hoge Destructor" << std::endl; };
  void addMap(std::string key, HeavyCopyMove && value) { map[key] = std::move(value); }
};

int main()
{
  Hoge hoge1;
  hoge1.addMap("key1", HeavyCopyMove("1"));
  Hoge hoge2 = std::move(hoge1);
  return 0;
}


```

<details>
<summary>正解</summary>
コピー. Hogeクラスのムーブ代入演算子が自動生成されないのでコピー代入される  
</details>

</br>

例: [auto_generated_member.cpp](./src/auto_generated_member.cpp)

特殊メンバ関数は以下  

- コンストラクタ  
- デストラクタ  
- コピーコンストラクタ/コピー代入演算子  
- ムーブコンストラクタ/ムーブ代入演算子  

ユーザー宣言のデストラクタを持つクラスではムーブ演算を自動生成生成しない  
ムーブ演算が自動生成されるのは以下３つの条件がすべて真の場合  

- クラスがコピー演算を宣言していない  
- ムーブ演算を宣言していない  
- デストラクタを宣言していない  

## ムーブのつかいどころ

- std::vectorのような, リソースを受け取って管理するクラスを自作する際に, 右辺値を渡してムーブで所有権を移動できると, 計算量を減らせる  
<https://cpprefjp.github.io/reference/vector/vector/push_back.html>  
std::vector::push_backでも右辺値参照引数でオーバーロードされている  

```cpp
void push_back(const T& x);           // (1) C++03
void push_back(T&& x);                // (2) C++11
```

```cpp
HeavyCopyMove heavy_copy_move("4");
std::vector<HeavyCopyMove> hoge_copy4;
// push_backでもmoveコンストラクタが呼ばれる
hoge_copy4.push_back(std::move(heavy_copy_move));
```

- ムーブ演算を持つクラスはstd::vectorで管理してメモリの再確保が行われた際にコピーではなくムーブされるのでお得

```cpp

int main()
{
  // HeavyCopyのコピーコンストラクタが3回呼ばれる
  std::vector<HeavyCopy> hoge_copy1{HeavyCopy("1"), HeavyCopy("2"), HeavyCopy("3")};

  // HeavyCopyはムーブコンストラクタを持たない
  // メモリの再確保が発生するため, コピーコンストラクタが呼ばれる
  std::vector<HeavyCopy> hoge_copy2;
  hoge_copy2.emplace_back("1");
  hoge_copy2.emplace_back("2");
  hoge_copy2.emplace_back("3");

  // HeavyCopyMoveはnoexceptなムーブコンストラクタを持つ
  // メモリの再確保の際にムーブコンストラクタが呼ばれる
  // https://qiita.com/kei10in/items/00f65e68a3d08093aceb
  std::vector<HeavyCopyMove> hoge_copy3;
  hoge_copy3.emplace_back("1");
  hoge_copy3.emplace_back("2");
  hoge_copy3.emplace_back("3");

  return 0;
}
```

![vector_allocate](./images/vector_allocate.png)

その他  
- <https://theolizer.com/cpp-school1/cpp-school1-37/>  
  - 要約
  - > RAIIパターンのクラス(リソースの確保と値の初期化を同時に行うもの. unique_ptrなど) はコピー不可能. ムーブを使えばstd::vectorで管理できるようになる  
  - > 所有権が一つのオブジェクトにのみ割り当てられるようなクラス（ファイルをオープンしたときのハンドラ, unique_ptrなど）をムーブ対応しておく（ムーブコンストラクタを書いておく）と、必要なときに所有権を移動できて便利

- <https://theolizer.com/cpp-school1/cpp-school1-36/>  
  - memo: RVOと矛盾...?  

---

## std::forwardと完全転送

std::move()は引数を無条件に右辺値へキャストするものだったが, std::forwardは条件付きで右辺値へキャストする  
std::forwardの実引数が左辺値で初期化されたものの場合は左辺値のままとし, 右辺値で初期化されたものの場合は右辺値にキャストする  

```cpp
class ForwardSample
{
private:
  std::vector<HeavyCopyMove> vec;
  std::vector<HeavyCopyMove> vec2;

public:
  ForwardSample()
  {
    std::cout << "ForwardSample Constructor" << std::endl;
    vec.reserve(100);
    vec2.reserve(100);
  };

  // 4種類のオーバーロードが必要?
  void add(HeavyCopyMove && value, HeavyCopyMove && value2)
  {
    std::cout << "add(HeavyCopyMove && value, HeavyCopyMove && value2)\n";
    vec.push_back(std::move(value));
    vec2.push_back(std::move(value2));
  }
  void add(const HeavyCopyMove & value, const HeavyCopyMove & value2)
  {
    std::cout << "add(const HeavyCopyMove & value, const HeavyCopyMove & value2)\n";
    vec.push_back(value);
    vec2.push_back(value2);
  }
  // 以下２つは無くてもコンパイルエラーにならないが, 上のconst LValueの定義が呼ばれるため適切にムーブされずコピーになる
  void add(HeavyCopyMove && value, const HeavyCopyMove & value2)
  {
    std::cout << "add(HeavyCopyMove && value, const HeavyCopyMove & value2)\n";
    vec.push_back(std::move(value));
    vec2.push_back(value2);
  }
  void add(const HeavyCopyMove & value, HeavyCopyMove && value2)
  {
    std::cout << "add(const HeavyCopyMove & value, HeavyCopyMove && value2)\n";
    vec.push_back(value);
    vec2.push_back(std::move(value2));
  }
};

```

```cpp
class ForwardSample
{
public:
  // テンプレートとforwardを使うと一つの関数定義でOK
  // 引数は(右,右), (左,右), (右,左), (左,左)の4パターンあるが, 適切にムーブ/コピーされる
  template <typename T, typename U>
  void add(T && value, U && value2)
  {
    vec.push_back(std::forward<T>(value));
    vec2.push_back(std::forward<U>(value2));
  }
};

```

例: [forward.cpp](./src/forward.cpp)

## RVO

例: [rvo.cpp](./src/rvo.cpp)
