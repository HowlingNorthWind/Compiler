#include <iostream>
#include <memory>
#include <string>
extern FILE *yyout;
// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump(std::string& str0) const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump(std::string& str0) const override {
    // std::cout << "CompUnitAST { ";
    str0 = "";
    func_def->Dump(str0);
    // std::cout << " }";
  }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump(std::string& str0) const override {
    // std::cout << "FuncDefAST { ";
    // func_type->Dump();
    // std::cout << ", " << ident << ", ";
    // block->Dump();
    // std::cout << " }";
    // std::cout << "fun @";
    // std::cout << ident << "(): ";
    // func_type->Dump();
    // std::cout << " { "<<std::endl;
    // block->Dump();
    // std::cout << " }";
    str0 += "fun @";
    str0 += ident;
    str0 += "(): ";
    fprintf(yyout, "fun @");
    fprintf(yyout, ident.c_str());
    fprintf(yyout, "(): ");
    std::cout << "fun @";
    std::cout << ident << "(): ";
    func_type->Dump(str0);
    str0 += " { \n";
    fprintf(yyout, " { \n");
    std::cout << " { "<<std::endl;
    block->Dump(str0);
    str0 += "}";
    fprintf(yyout, "}");
    std::cout << "}";
  }
};

// FuncType 也是 BaseAST
class FuncTypeAST : public BaseAST {
 public:
  std::string func_type_str;

  void Dump(std::string& str0) const override {
    if(func_type_str == "int")
    {
        str0 += "i32";
        fprintf(yyout, "i32");
        std::cout << "i32";
    } 
  }
};

// Block 也是 BaseAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump(std::string& str0) const override {
    str0 += "%%";
    str0 += "entry";
    str0 += ":\n";
    fprintf(yyout, "%%");
    fprintf(yyout, "entry");
    fprintf(yyout, ":\n");
    std::cout << "%";
    std::cout << "entry";
    std::cout << ":"<<std::endl;
    stmt->Dump(str0);
    str0 += "\n";
    fprintf(yyout, "\n");
    std::cout << std::endl;
  }
};

// Stmt 也是 BaseAST
class StmtAST : public BaseAST {
 public:
  int number;

  void Dump(std::string& str0) const override {
    str0 += " ret ";
    str0 += std::to_string(number).c_str();
    fprintf(yyout, "  ret ");
    fprintf(yyout, std::to_string(number).c_str());
    std::cout <<"  "<< "ret ";
    std::cout << number;
  }
};
