#include <iostream>
#include <memory>
#include <string>
// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    // std::cout << "CompUnitAST { ";
    func_def->Dump();
    // std::cout << " }";
  }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    // std::cout << "FuncDefAST { ";
    // func_type->Dump();
    // std::cout << ", " << ident << ", ";
    // block->Dump();
    // std::cout << " }";
    std::cout << "fun @";
    std::cout << ident << "(): ";
    func_type->Dump();
    std::cout << " { "<<std::endl;
    block->Dump();
    std::cout << " }";
  }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;

  void Dump() const override {
    if(func_type_str == "int")
    {
        std::cout << "i32";
    } 
  }
};

// Block 也是 BaseAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override {
    std::cout << "%";
    std::cout << "entry";
    std::cout << ":"<<std::endl;
    stmt->Dump();
    std::cout << std::endl;
  }
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
 public:
  int number;

  void Dump() const override {
    std::cout <<"  "<< "ret ";
    std::cout << number;
  }
};
