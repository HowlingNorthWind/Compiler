docker pull maxxing/compiler-dev  
docker run -it --rm -v 'MyPath':/root/compiler maxxing/compiler-dev bash  

make  
//To Koopa-IR  
build/compiler -koopa hello.c -o hello.koopa  
//To RISC-V  
build/compiler -riscv hello.c -o target   
