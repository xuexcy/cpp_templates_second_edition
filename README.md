# cpp_templates_second_edition
## Build And Run
- `env_variable.sh`: 设置环境变量，如 cmake 工作目录、可执行文件存放的 bin 文件夹目录等
- `build.sh`: 编译所有 src/*.cc
- `run.sh`: 运行所有编译后的可执行文件，并将结果输出到 stdout 目录
- `build_one.sh`: 编译 src 目录下的一个 .cc 文件，具体编译哪一个文件取决于 `env_variable.sh` 中的 `default_execute_file` 环境变量。 这个脚本主要是自己修改、测试某个 .cc 代码的时候使用，该文件里没有调用 cmake，如果编译的相关依赖发生改变，请重新执行 `build.sh` 生成新的 makefile 文件
- `run_one.sh`: 执行一个编译结果，同 `build_one.sh` 一样，具体哪个取决于 `env_variable.sh` 中的 `default_execute_file`
- `src/*.cc`: 各个 main 文件

## 目录
### 第一部分 基础知识
1. 函数模板
  ```cpp
  template <typename T>
  T max(T a, T b) { return a > b ? a : b; }
  ```
2. 类模板
  ```cpp
  template <typename T>
  class Stack {
  private:
    std::vector<T> elems;
  public:
    void push(const T& elem);
    const T& top() const;
  };  // class Stack
  ```
3. 非类型模板参数
  ```cpp
  // template <typename T, size_t MaxSize>
  template <typename T, auto MaxSize>
  class Stack {
  public:
    using size_type = decltype(MaxSize);
  public:
    std::array<T, MaxSize> elems;
  public:
    void push(const T& elem);
    const T& top() const;
  };  // class Stack
  ```
4. 变参模板
  ```cpp
  template <typename T, typename... Args>
  std::shared_ptr<T> make_shared(Args&&... args) {
    return std::shared_ptr<T>(new T(args...));
  }
  ```
5. 基本技巧
6. 移动语义与 `enable_if<>`
7. 传值还是传引用
8. 编译期编程
9. 在实践中使用模板
10. 模板基本术语
11. 泛型库

## 第二部分 深入模板
12. 深入模板基础
13. 模板中的名称
## 第三部分 模板与设计
28
