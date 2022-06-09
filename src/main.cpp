#include "koopa.h"
#include "ast.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
#include <map>
#include <variant>

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(unique_ptr<BaseAST> &ast);
int tmpcnt = 0;
int cnt0 = 0;
int symcnt = 0;
int ifcnt = 0;
int whilecnt = 0;
int bblockcnt = 0;
int landcnt = 0;
int lorcnt = 0;
int fl_break_continue = 0;
string cur_end = "";
string cur_entry = "";
map<string, variant<int, string>> sym_table;
map<string, variant<int, string>> var_table;
map<string, variant<int, string>> *curFunvar_table;
map<string, int> value_table;
map<string, string> funcTable;
map<string, variant<int, string>> *cur_table;
map<map<string, variant<int, string>>*,map<string, variant<int, string>>*> total_table;

map<string, int> *cur_array_dims_table;
map<map<string, int>*, map<string, int>*> total_array_dims_table;




map<koopa_raw_value_t, int> stackForInsts;
map<koopa_raw_function_t, int> mapFuncToSp;
map<koopa_raw_function_t, int> mapFuncToRa;
koopa_raw_function_t curFunc;
string strForRISCV;
// 函数声明略
// ...
void Visit(const koopa_raw_program_t &program);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);
void Visit(const koopa_raw_return_t &ret);
void Visit(const koopa_raw_integer_t &integer);
void Visit(const koopa_raw_binary_t &binary);
void Visit(const koopa_raw_store_t &rawStore);
void Visit(const koopa_raw_load_t &load);
void Visit(const koopa_raw_jump_t &jump);
void Visit(const koopa_raw_branch_t &branch);
void Visit(const koopa_raw_call_t &call);
void Visit(const koopa_raw_func_arg_ref_t &funcArgRef);
void Visit(const koopa_raw_global_alloc_t &myGlobalAlloc);
void Visit(const koopa_raw_aggregate_t &myAggregate) ;
void Visit(const koopa_raw_get_elem_ptr_t &myGetElemPtr);
void Visit(const koopa_raw_get_ptr_t &myGetPtr);

int retValue(const koopa_raw_value_t &rawValue);
int retValue(const koopa_raw_integer_t &rawInterger);

void myEpilogue(const koopa_raw_function_t &func);
void myPrologue(const koopa_raw_function_t &func);

void myPrintGlobalVar(const koopa_raw_value_t &value);
void myInitArray(const koopa_raw_value_t &value);

void writeTo(const koopa_raw_value_t &value, string srcReg){
  cout<<"raw_Value"<<endl;
  cout<<value<<endl;
  if(stackForInsts.find(value) == stackForInsts.end()){
    // assert(false);
    if(value->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
      string myGlobalName = value->name;
      myGlobalName.erase(0,1);
      strForRISCV += " la t1, " + myGlobalName + "\n";
      strForRISCV += " sw t0, 0(t1)\n";
    }
  }else{
      strForRISCV += " sw "+srcReg+", "+to_string(stackForInsts[value])+"(sp)\n";
      strForRISCV += "\n";
      // cout<<strForRISCV<<endl;
  }
  
}

void readFrom(const koopa_raw_value_t &value, string destReg){
  // auto instType = value->ty->tag;
  if(stackForInsts.find(value) != stackForInsts.end()){
    strForRISCV += " lw "+destReg+", "+ to_string(stackForInsts[value]) + "(sp)" + "\n";
    // cout<<strForRISCV<<endl;
  }else if(value->kind.tag == KOOPA_RVT_INTEGER){
    strForRISCV += " li "+destReg+", "+ to_string(value->kind.data.integer.value) + "\n";
    // cout<<strForRISCV<<endl;
  }else if(value->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
    string myGlobalName = value->name;
    myGlobalName.erase(0,1);
    strForRISCV += " la " + destReg + ", " + myGlobalName + "\n";
    strForRISCV += " lw " + destReg + ", 0("+destReg+")\n";
  }else{
    cout<<"readFrom"<<value->kind.tag<<endl;
    assert(false);
  }
}

// 访问 raw program
void Visit(const koopa_raw_program_t &program) {
  // 执行一些其他的必要操作
  strForRISCV = "";
  // cout<<"  .text "<<endl;
  // strForRISCV += "  .text \n";
  // fprintf(yyout, "  .text ");
  // fprintf(yyout, "\n");
  for (size_t i = 0; i < program.values.len; ++i) {
  // 正常情况下, 列表中的元素就是变量, 我们只不过是在确认这个事实
  // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
  assert(program.values.kind == KOOPA_RSIK_VALUE);
  // 获取当前变量
  // koopa_raw_function_t value = (koopa_raw_function_t) program.values.buffer[i];

  // 进一步处理全局变量
  // cout<<value->name<<endl;
  // strForRISCV += value->name;
  // strForRISCV += "\n";
  // fprintf(yyout,"%s", value->name);
  // fprintf(yyout, "\n");
  }
  // cout<<"  .global ";
  // strForRISCV += "  .global ";
  // fprintf(yyout, "  .global ");

  for (size_t i = 0; i < program.funcs.len; ++i) {
  // 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
  // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
    assert(program.funcs.kind == KOOPA_RSIK_FUNCTION);
    // 获取当前函数
    // koopa_raw_function_t func = (koopa_raw_function_t) program.funcs.buffer[i];

    // 进一步处理当前函数
    //cout<<func->name<<endl;
    // for(int i = 1; i < strlen(func->name);i++)
    // {
    //   char c = func->name[i]; 
    //   // cout<<c;
    //   // fprintf(yyout, "%c",c);
    //   strForRISCV += c;
    // }
    // // fprintf(yyout, "%s",func->name);
    // // cout<<endl;
    // strForRISCV += "\n";
    // fprintf(yyout, "\n");
  }
  // 访问所有全局变量
  Visit(program.values);
  // 访问所有函数
  Visit(program.funcs);

  fprintf(yyout, "%s", strForRISCV.c_str());
}

// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice) {
  for (size_t i = 0; i < slice.len; ++i) {
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind) {
      case KOOPA_RSIK_FUNCTION:
        // 访问函数
        if(reinterpret_cast<koopa_raw_function_t>(ptr)->bbs.len != 0){
          Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
        }
        break;
      case KOOPA_RSIK_BASIC_BLOCK:
        // 访问基本块
        Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
        break;
      case KOOPA_RSIK_VALUE:
        // 访问指令
        Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
        break;
      default:
        // 我们暂时不会遇到其他内容, 于是不对其做任何处理
        assert(false);
    }
  }
}

// 访问函数
void Visit(const koopa_raw_function_t &func) {
  // 执行一些其他的必要操作
  // ...
  strForRISCV += "  .text \n";
  strForRISCV += "  .global ";
  string funcName = func->name;
  funcName.erase(0,1);
  strForRISCV += funcName+"\n";
  strForRISCV += funcName;
  strForRISCV += ":\n";
  // 访问所有基本块
  Visit(func->params);

  
  int lenForBbs = func->bbs.len;
  cout<<"lenForBbs "<<lenForBbs<<endl;
  int spForEachInst = 0;
  int spForRa = 0;
  int spForFuncParams = 0;
  int maxLenForArgs = 0;
  for(int i = 0; i < lenForBbs; i++){
      auto ptr = func->bbs.buffer[i];
      koopa_raw_basic_block_t funcBb = reinterpret_cast<koopa_raw_basic_block_t>(ptr);
      int lenForInsts = funcBb->insts.len;
      for(int j = 0; j < lenForInsts; j++){
        auto ptr = funcBb->insts.buffer[j];
        koopa_raw_value_t bbInst = reinterpret_cast<koopa_raw_value_t>(ptr);
        if(bbInst->ty->tag != KOOPA_RTT_UNIT){
          // cout<<"AAAAAAAAAAAAAAA"<<endl;
          // cout<<spForEachInst<<endl;
          // cout<<bbInst<<endl;
          // stackForInsts[bbInst] = spForEachInst;

          if(bbInst->ty->tag != KOOPA_RTT_ARRAY){
            spForEachInst += 4;
          }else if(bbInst->ty->tag == KOOPA_RTT_ARRAY){
            auto targetArray = bbInst->ty->data.array;
            int arraySize = targetArray.len;
            while(targetArray.base->tag == KOOPA_RTT_ARRAY){
              targetArray = targetArray.base->data.array;
              arraySize *= targetArray.len;
            }
            if(targetArray.base->tag != KOOPA_RTT_INT32){
              assert(false);
            }
            arraySize *= 4;
            spForEachInst += arraySize;

          }

          
        }
        if(bbInst->kind.tag == KOOPA_RVT_CALL){
          if(spForRa == 0){
            spForRa = 4;
          }
          koopa_raw_slice_t funcArgs = bbInst->kind.data.call.args;
          int lenForFuncArgs = funcArgs.len;
          maxLenForArgs = max(lenForFuncArgs, maxLenForArgs);
        }
      }
  }
  spForFuncParams = max(0, maxLenForArgs - 8)*4;
  int spForAll = spForEachInst + spForFuncParams + spForRa;
  spForAll = (spForAll + 15)/16*16;
  mapFuncToSp[func] = spForAll;
  mapFuncToRa[func] = spForRa;
  int specialSpForEachInst = 0;
  for(int i = 0; i < lenForBbs; i++){
      auto ptr = func->bbs.buffer[i];
      koopa_raw_basic_block_t funcBb = reinterpret_cast<koopa_raw_basic_block_t>(ptr);
      int lenForInsts = funcBb->insts.len;
      for(int j = 0; j < lenForInsts; j++){
        auto ptr = funcBb->insts.buffer[j];
        koopa_raw_value_t bbInst = reinterpret_cast<koopa_raw_value_t>(ptr);
        if(bbInst->ty->tag != KOOPA_RTT_UNIT){
          stackForInsts[bbInst] = spForFuncParams+specialSpForEachInst;
          if(bbInst->ty->tag != KOOPA_RTT_ARRAY){
            specialSpForEachInst += 4;
          }else if(bbInst->ty->tag == KOOPA_RTT_ARRAY){
            auto targetArray = bbInst->ty->data.array;
            int arraySize = targetArray.len;
            while(targetArray.base->tag == KOOPA_RTT_ARRAY){
              targetArray = targetArray.base->data.array;
              arraySize *= targetArray.len;
            }
            if(targetArray.base->tag != KOOPA_RTT_INT32){
              assert(false);
            }
            arraySize *= 4;
            specialSpForEachInst += arraySize;
          }
        }
      }
  }
  // cout<<strForRISCV<<endl;
  koopa_raw_function_t prevFunc = curFunc;
  curFunc = func;
  myPrologue(curFunc);
  Visit(func->bbs);
  curFunc = prevFunc;
  // cout<<strForRISCV<<endl;
}

void myPrologue(const koopa_raw_function_t &func){
  int sp = mapFuncToSp[func];
  int ra = mapFuncToRa[func];
  if(sp != 0){
      strForRISCV += " addi sp, sp, " + to_string(-sp) + "\n";
      if(ra != 0){
        strForRISCV += " sw ra, "+to_string(sp-4) +"(sp)\n";
      }
  }
}

void myEpilogue(const koopa_raw_function_t &func){
  int sp = mapFuncToSp[func];
  int ra = mapFuncToRa[func];
  if(sp != 0){
      if(ra != 0){
        strForRISCV += " lw ra, "+to_string(sp-4) +"(sp)\n";
      }
      strForRISCV += " addi sp, sp, " + to_string(mapFuncToSp[func]) + "\n";   
  }
}

// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb) {
  // 执行一些其他的必要操作
  // ...
  // 访问所有指令
  string bbLabel = bb->name;
  bbLabel.erase(0,1);
  if(bbLabel != "entry"){
    strForRISCV += bbLabel+":\n";
  }
  Visit(bb->insts);
}

// 访问指令
void Visit(const koopa_raw_value_t &value) {
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  cout<<kind.tag<<endl;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit(kind.data.ret);
      break;
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit(kind.data.integer);
      break;
    case KOOPA_RVT_STORE:
      // 访问 integer 指令
      cout<<"KOOPA_RVT_STORE"<<endl;
      Visit(kind.data.store);
      // writeTo(value);
      break;
    case KOOPA_RVT_LOAD:
      Visit(kind.data.load);
      writeTo(value, "t0");
      break;
    case KOOPA_RVT_BINARY:
      // cout<<"KOOPA_RVT_BINARY"<<endl;
      Visit(kind.data.binary);
      writeTo(value, "t0");
      break;
    case KOOPA_RVT_ALLOC:

      cout<<"KOOPA_RVT_ALLOC"<<endl;
      
      break;
    case KOOPA_RVT_BRANCH:
      Visit(kind.data.branch);
      break;
    case KOOPA_RVT_JUMP:
      Visit(kind.data.jump);
      break;
    case KOOPA_RVT_CALL:
      Visit(kind.data.call);
      writeTo(value, "a0");
      break;
    case KOOPA_RVT_FUNC_ARG_REF:
      Visit(kind.data.func_arg_ref);
      break;
    case KOOPA_RVT_GLOBAL_ALLOC:
      myPrintGlobalVar(value);
      Visit(kind.data.global_alloc);
      break;
    case KOOPA_RVT_ZERO_INIT:
     
   
      myInitArray(value);
      // strForRISCV += " .zero 4\n";
      break;
    case KOOPA_RVT_AGGREGATE:
      Visit(kind.data.aggregate);
      break;
    case KOOPA_RVT_GET_ELEM_PTR:
      // Visit(kind.data.get_elem_ptr);
      // writeTo(value, "t0");
      // assert(false);
      break;
    // case KOOPA_RVT_GET_PTR:
    //   Visit(kind.data.get_ptr);
    //   writeTo(value, "t0");
    //   break;
    default:
      // 其他类型暂时遇不到
      cout<<kind.tag<<endl;
      assert(false);
  }
}

void myPrintGlobalVar(const koopa_raw_value_t &value){
  strForRISCV += " .data\n";
  string myGlobalName = value->name;
  myGlobalName.erase(0, 1);
  strForRISCV += " .global "+myGlobalName+"\n";
  strForRISCV += myGlobalName+":\n";
}


void myInitArray(const koopa_raw_value_t &value){

  strForRISCV += " .zero ";

  if(value->ty->tag == KOOPA_RTT_INT32){
    strForRISCV += "4";
  }else if(value->ty->tag == KOOPA_RTT_ARRAY){
    auto curArray = value->ty->data.array;
    int sizeOfArray = curArray.len;
    while(curArray.base->tag == KOOPA_RTT_ARRAY){
      sizeOfArray *= curArray.base->data.array.len;
      curArray = curArray.base->data.array;
    }
    sizeOfArray *= 4;
    strForRISCV += to_string(sizeOfArray);
  }
  

  


  strForRISCV += "\n";

  // cout<<strForRISCV<<endl;
  // assert(false);
  
}
// 访问对应类型指令的函数定义略
// 视需求自行实现
// ...

void Visit(const koopa_raw_return_t &ret) {
  // cout<<"Return "<<ret.value->ty->tag<<endl;
  if(ret.value == NULL){

  }else if(stackForInsts.find(ret.value)!= stackForInsts.end() ){
    strForRISCV += " lw a0, ";
    strForRISCV += to_string(stackForInsts[ret.value]);
    strForRISCV += "(sp)\n";
  }else{
    strForRISCV += " li a0, ";
    strForRISCV += to_string(ret.value->kind.data.integer.value);
    strForRISCV += "\n";
  }
  
  myEpilogue(curFunc);
  strForRISCV += " ret\n";
  // strForRISCV += " ret\n";
}

void Visit(const koopa_raw_call_t &call) {
  auto callFuncArgs = call.args;
  int lenForCallFuncArgs = callFuncArgs.len;
  for(int i = 0; i < lenForCallFuncArgs; i++){
    string destReg = "";
    if(i<8){
      destReg = "a"+to_string(i);
      readFrom(reinterpret_cast<koopa_raw_value_t>(call.args.buffer[i]), destReg);
    }else{
      string destMem = to_string((i-8)*4)+"(sp)";
      readFrom(reinterpret_cast<koopa_raw_value_t>(call.args.buffer[i]), "t0");
      strForRISCV += "sw t0, "+destMem+"\n";
    }
    
  }
  string callFuncName = call.callee->name;
  callFuncName.erase(0,1);
  strForRISCV += " call " +callFuncName+"\n";

}

void Visit(const koopa_raw_func_arg_ref_t &funcArgRef) {
  cout<<"Index"<<funcArgRef.index<<endl;
}

void Visit(const koopa_raw_global_alloc_t &myGlobalAlloc) {
  cout<<myGlobalAlloc.init->kind.tag<<endl;
  if(myGlobalAlloc.init->kind.tag == KOOPA_RVT_INTEGER){
    strForRISCV += " .word "+to_string(myGlobalAlloc.init->kind.data.integer.value)+"\n";
  }else if(myGlobalAlloc.init->kind.tag == KOOPA_RVT_ZERO_INIT){
    Visit(myGlobalAlloc.init);
  }else if(myGlobalAlloc.init->kind.tag == KOOPA_RVT_AGGREGATE){
    // assert(false);
    Visit(myGlobalAlloc.init);
  }
  else{
    std::cout<<myGlobalAlloc.init->kind.tag<<std::endl;
    assert(false);
  
  }

}

void Visit(const koopa_raw_aggregate_t &myAggregate) {
  Visit(myAggregate.elems);
  // cout<<strForRISCV<<endl;
  // assert(false);
}

void Visit(const koopa_raw_get_elem_ptr_t &myGetElemPtr) {
  
  // cout<<strForRISCV<<endl;
  cout<<myGetElemPtr.src->ty->tag<<endl;
  if(myGetElemPtr.src->ty->tag != KOOPA_RTT_POINTER){
    assert(false);
  }
  auto srcPtr = myGetElemPtr.src->ty->data.pointer;
  
  int sizeOfPtr = 1;
  
  if(srcPtr.base->tag == KOOPA_RTT_INT32){
    sizeOfPtr *= 4;
  }else{
    auto srcArray = srcPtr.base->data.array;
    // sizeOfPtr *= srcArray.len;
    // cout<<"srcArray.len"<<srcArray.len<<endl;
    cout<<srcArray.base->tag<<endl;
    while(srcArray.base->tag != KOOPA_RTT_INT32){
      if(srcArray.base->tag != KOOPA_RTT_ARRAY){
        assert(false);
      }
      sizeOfPtr *= srcArray.base->data.array.len;
      // cout<<"srcArray.base->data.array.len"<<srcArray.base->data.array.len<<endl;
      srcArray = srcArray.base->data.array;
    }
    sizeOfPtr *= 4;
  }
  
  // if(myGetElemPtr.index->kind.tag != KOOPA_RVT_INTEGER){
  //   assert(false);
  // }

  // int index = myGetElemPtr.index->kind.data.integer.value;


  // readFrom(myGetElemPtr.src, "t0");

  // strForRISCV += " li t1, "+to_string(index)+'\n';
  // strForRISCV += " li t2, "+to_string(sizeOfPtr)+'\n';
  // strForRISCV += " mul t1, t1, t2\n";
  // strForRISCV += " add t0, t0, t1\n";

  if(myGetElemPtr.index->kind.tag != KOOPA_RVT_INTEGER){
    // cout<<strForRISCV<<endl;

    readFrom(myGetElemPtr.src, "t0");

    assert(stackForInsts.find(myGetElemPtr.index)!= stackForInsts.end()); 

    strForRISCV += " lw t1, ";
    strForRISCV += to_string(stackForInsts[myGetElemPtr.index]);
    strForRISCV += "(sp)\n";

    // strForRISCV += " li t1, "+to_string(index)+'\n';
    strForRISCV += " li t2, "+to_string(sizeOfPtr)+'\n';
    strForRISCV += " mul t1, t1, t2\n";
    strForRISCV += " add t0, t0, t1\n";

    // assert(false);
  }else if(myGetElemPtr.index->kind.tag == KOOPA_RVT_INTEGER){
    int index = myGetElemPtr.index->kind.data.integer.value;


    readFrom(myGetElemPtr.src, "t0");

    strForRISCV += " li t1, "+to_string(index)+'\n';
    strForRISCV += " li t2, "+to_string(sizeOfPtr)+'\n';
    strForRISCV += " mul t1, t1, t2\n";
    strForRISCV += " add t0, t0, t1\n";
  }

  // cout<<strForRISCV<<endl;
  // cout<<"sizeOfPtr"<<sizeOfPtr<<endl;
  // assert(false);


}


void Visit(const koopa_raw_get_ptr_t &myGetPtr) {
  
  // cout<<strForRISCV<<endl;
  cout<<myGetPtr.src->ty->tag<<endl;
  if(myGetPtr.src->ty->tag != KOOPA_RTT_POINTER){
    assert(false);
  }
  auto srcPtr = myGetPtr.src->ty->data.pointer;
  
  int sizeOfPtr = 1;
  
  if(srcPtr.base->tag == KOOPA_RTT_INT32){
    sizeOfPtr *= 4;
  }else{
    auto srcArray = srcPtr.base->data.array;
    // sizeOfPtr *= srcArray.len;
    // cout<<"srcArray.len"<<srcArray.len<<endl;
    cout<<srcArray.base->tag<<endl;
    while(srcArray.base->tag != KOOPA_RTT_INT32){
      if(srcArray.base->tag != KOOPA_RTT_ARRAY){
        assert(false);
      }
      sizeOfPtr *= srcArray.base->data.array.len;
      // cout<<"srcArray.base->data.array.len"<<srcArray.base->data.array.len<<endl;
      srcArray = srcArray.base->data.array;
    }
    sizeOfPtr *= 4;
  }
  
  if(myGetPtr.index->kind.tag != KOOPA_RVT_INTEGER){
    // cout<<strForRISCV<<endl;

    readFrom(myGetPtr.src, "t0");

    assert(stackForInsts.find(myGetPtr.index)!= stackForInsts.end()); 

    strForRISCV += " lw t1, ";
    strForRISCV += to_string(stackForInsts[myGetPtr.index]);
    strForRISCV += "(sp)\n";

    // strForRISCV += " li t1, "+to_string(index)+'\n';
    strForRISCV += " li t2, "+to_string(sizeOfPtr)+'\n';
    strForRISCV += " mul t1, t1, t2\n";
    strForRISCV += " add t0, t0, t1\n";

    // assert(false);
  }else if(myGetPtr.index->kind.tag == KOOPA_RVT_INTEGER){
    int index = myGetPtr.index->kind.data.integer.value;


    readFrom(myGetPtr.src, "t0");

    strForRISCV += " li t1, "+to_string(index)+'\n';
    strForRISCV += " li t2, "+to_string(sizeOfPtr)+'\n';
    strForRISCV += " mul t1, t1, t2\n";
    strForRISCV += " add t0, t0, t1\n";
  }

  

  // cout<<strForRISCV<<endl;
  // cout<<"sizeOfPtr"<<sizeOfPtr<<endl;
  // assert(false);


}




void Visit(const koopa_raw_branch_t &branch) {
  readFrom(branch.cond, "t0");
  string trueLabel = branch.true_bb->name;
  trueLabel.erase(0,1);
  strForRISCV += " bnez t0, "+trueLabel+"\n";
  string falseLabel = branch.false_bb->name; 
  falseLabel.erase(0,1);
  strForRISCV += " j "+falseLabel+"\n";
 
}

void Visit(const koopa_raw_jump_t &jump) {
 string targetLabel = jump.target->name;
 targetLabel.erase(0,1);
 strForRISCV += " j "+targetLabel+"\n";
}




void Visit(const koopa_raw_binary_t &binary) {
  // const auto leftValue = retValue(binary.lhs);
  // const auto rightValue = retValue(binary.rhs);
  // cout<<"leftValue "<<leftValue<<endl;
  // cout<<"rightValue "<<rightValue<<endl;

  readFrom(binary.lhs, "t0");
  readFrom(binary.rhs, "t1");

  switch (binary.op) {
    case KOOPA_RBO_EQ:
      strForRISCV += " xor t0, t0, t1\n";
      strForRISCV += " seqz t0, t0\n";
      break;
    case KOOPA_RBO_NOT_EQ:
      strForRISCV += " xor t0, t0, t1\n";
      strForRISCV += " snez t0, t0\n";
      break;
    case KOOPA_RBO_GT:
      strForRISCV += " sgt t0, t0, t1\n";
      break;
    case KOOPA_RBO_LT:
      strForRISCV += " slt t0, t0, t1\n";
      break;
    case KOOPA_RBO_GE:
      strForRISCV += " slt t0, t0, t1\n";
      strForRISCV += " seqz t0, t0\n";
      break;
    case KOOPA_RBO_LE:
      strForRISCV += " sgt t0, t0, t1\n";
      strForRISCV += " seqz t0, t0\n";
      break;
    case KOOPA_RBO_SUB:
      strForRISCV += " sub t0, t0, t1\n";
      break;
    case KOOPA_RBO_ADD:
      strForRISCV += " add t0, t0, t1\n";
      break;
    case KOOPA_RBO_MUL:
      strForRISCV += " mul t0, t0, t1\n";
      break;
    case KOOPA_RBO_DIV:
      strForRISCV += " div t0, t0, t1\n";
      break;
    case KOOPA_RBO_MOD:
      strForRISCV += " rem t0, t0, t1\n";
      break;
    case KOOPA_RBO_AND:
      strForRISCV += " and t0, t0, t1\n";
      break;
    case KOOPA_RBO_OR:
      strForRISCV += " or t0, t0, t1\n";
      break;
    case KOOPA_RBO_XOR:
      strForRISCV += " xor t0, t0, t1\n";
      break;
    case KOOPA_RBO_SHL:
      strForRISCV += " sll t0, t0, t1\n";
      break;
    case KOOPA_RBO_SHR:
      strForRISCV += " srl t0, t0, t1\n";
      break;
    case KOOPA_RBO_SAR:
      strForRISCV += " sra t0, t0, t1\n";
      break;
    default:
      assert(false);
  }
  // cout<<"op"<<binary.op<<endl;
  // cout<<binary.lhs->ty->tag<<endl;
  // cout<<"binary.rhs"<<endl;
  // cout<<binary.rhs->kind.data.integer.value<<endl;
  // std::string tmp1,tmp2,tmp3;
  // tmp1 = "t" + std::to_string(cnt0);
  // cnt0++;
  // cout<<tmp1<<endl;
  
}

void Visit(const koopa_raw_store_t &rawStore) {
  cout<<"rawStore"<<endl;
  // cout<<rawStore.dest->kind.tag<<endl;
  cout<<rawStore.value->kind.tag<<endl;
  if(rawStore.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF){
      int myIndex = rawStore.value->kind.data.func_arg_ref.index;
      if(myIndex < 8){
        writeTo(rawStore.dest, "a"+to_string(rawStore.value->kind.data.func_arg_ref.index));
      }else{
        int spNumForCalleeArgs = mapFuncToSp[curFunc] + (myIndex-8)*4;
        // readFrom(to_string(spNumForCalleeArgs)+"(sp)","t0");
        strForRISCV += " lw t0, "+to_string(spNumForCalleeArgs)+"(sp)\n";
        writeTo(rawStore.dest, "t0");
      }
     
  }else if(rawStore.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
      readFrom(rawStore.value, "t0");
      writeTo(rawStore.dest, "t0");
      // assert(false);
  }else{
      readFrom(rawStore.value, "t0");
      writeTo(rawStore.dest, "t0");
  }

}

void Visit(const koopa_raw_load_t &load) {
  readFrom(load.src, "t0");
}

void Visit(const koopa_raw_integer_t &integer) {
  cout<<integer.value<<endl;
  // strForRISCV += " .word "+to_string(integer.value)+'\n';
}

int retValue(const koopa_raw_value_t &rawValue){
  const auto &kind = rawValue->kind;
  switch (kind.tag) {
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      return retValue(kind.data.integer);
      break;
    default:
      // 其他类型暂时遇不到
      assert(false);
  }
}


int retValue(const koopa_raw_integer_t &rawInterger){
  return rawInterger.value;
}




int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  cout<<"mode"<<mode<<endl;
  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  //打开输出文件
  yyout = fopen(output, "w");

  // parse input file
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  string str0 = "";
  // dump AST
  // if(mode == )
  
  cout<<"BEGIN KOOPA IR"<<endl;
  ast->Dump(str0);
  cout << endl;

  //得到str，即KoopaIR
  cout<< str0 <<endl;
  const char* str = str0.c_str();

  cout<<"mode"<<endl;
  cout<<mode<<endl;
  cout<<str<<endl;
  if(strcmp(mode,"-koopa") == 0)
  {
    cout<<"yyout"<<endl;
    for(int i=0; i <strlen(str); i++)
    {
      char c = str[i];
      cout<<c;
      if(c=='%')
      {
        fprintf(yyout,"%%");
      }else{
        fprintf(yyout,"%c", c);
      }
    }
    // fprintf(yyout,"%s", str);
  }

  // 解析字符串 str, 得到 Koopa IR 程序
  koopa_program_t program;
  koopa_error_code_t ret0 = koopa_parse_from_string(str, &program);
  assert(ret0 == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  // 处理 raw program
  // ...
  if(strcmp(mode,"-riscv") == 0)
  {
    cout<<"riscv-out"<<endl;
    Visit(raw);
  }

  // cout<<"____________"<<endl;
  // for (size_t i = 0; i < raw.funcs.len; ++i) {
  // // 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
  // // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
  // assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
  // // 获取当前函数
  // koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];
  // for (size_t j = 0; j < func->bbs.len; ++j) {
  // assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
  // koopa_raw_basic_block_t bb = (const koopa_raw_basic_block_data_t *)func->bbs.buffer[j];
  // for (size_t k = 0; k < bb->insts.len; ++k){
  //   assert(bb->insts.kind == KOOPA_RSIK_VALUE);
  //   koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
  //   assert(value->kind.tag == KOOPA_RVT_RETURN);
  //   cout<<"return"<<endl;
  //   // 于是我们可以按照处理 return 指令的方式处理这个 value
  //   // return 指令中, value 代表返回值
  //   koopa_raw_value_t ret_value = value->kind.data.ret.value;
  //   // 示例程序中, ret_value 一定是一个 integer
  //   assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
  //   // 于是我们可以按照处理 integer 的方式处理 ret_value
  //   // integer 中, value 代表整数的数值
  //   int32_t int_val = ret_value->kind.data.integer.value;
  //   cout<<int_val<<endl;
  //   // 示例程序中, 这个数值一定是 0
  //   assert(int_val == 0);
  // }
  // }
  // }

  // 处理完成, 释放 raw program builder 占用的内存
  // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
  // 所以不要在 raw program builder 处理完毕之前释放 builder
  koopa_delete_raw_program_builder(builder);


  // 输出解析得到的 AST, 其实就是个字符串
  // cout << *ast << endl;
  return 0;
}