#include <iostream>
#include <memory>
#include <string>
// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;
};

// Block 也是 BaseAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> number;
};

// Number 也是 BaseAST
class NumberAST : public BaseAST {
 public:
   std::string int_const;
};