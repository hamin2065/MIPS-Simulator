// 구하민 - ALU
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int ALU(int X, int Y, int C, int* Z);
int logicOperation(int X, int Y, int s1s0);
int shiftOperation(int X, int Y, int s1s0);
int addSubtract(int X, int Y, int s0);
int checkSetLess(int X, int Y);
int checkZero(int Oa);
void test();

int main(){
    test();
}

//test program example
//test for each C value
void test(void){
    int x, y, c, s, z;

    x = 5;
    y = 5;

    printf("x : %8x, y : %8x\n", x, y);
    for(int i = 0; i < 16; i++){
        s = ALU(x, y, i, &z);
        printf("s : %8x, z : %8x\n", s, z);
    }

}
int ALU(int X, int Y, int C, int *Z){
    int s3s2, s1s0;
    int ret;

    s3s2 = (C >> 2) & 3;
    s1s0 = C&3;

    if(s3s2 == 0){
        //shift 연산
        ret = shiftOperation(X, Y, s1s0);
    }else if(s3s2 == 1){
        // set less
        ret = checkSetLess(X, Y);
    }else if(s3s2 == 2){
        //add substract
        *Z = checkZero(s1s0 & 1);
        ret = addSubtract(X, Y, *Z);
    }else if(s3s2 == 3){
        // AND OR XOR NOR
        ret = logicOperation(X, Y, s1s0);
    }else{
        printf("error in ALU");
        exit(1);
    }
    return ret;
}

int logicOperation(int X, int Y, int s1s0){
    if(s1s0 < 0 || s1s0 > 3){
        printf("error in logic\n");
        exit(1);
    }
    if(s1s0 == 0)
        return X&Y;
    else if(s1s0 == 1)
        return X|Y;
    else if(s1s0 == 2)
        return X^Y;
    else 
        return !(X|Y);
}

int shiftOperation(int X, int Y, int s1s0){
    int ret;
    if(s1s0 < 0 || s1s0 > 3 ){
        printf("error in shift");
        exit(1);
    }
    if(s1s0 == 0){
        ret = X;
    }else if(s1s0 == 1){ // shift left logical
        ret = X << Y;
    }else if(s1s0 == 2){ // shift right logical
        ret = (X >> Y) & ~(((0x1 << sizeof(X)) >> Y) << 1);
    }else{ //shift right arithmatic
        ret = X >> Y;
    }
    return ret;
}

int addSubtract(int X, int Y, int s0){
    int ret;
    if(s0 < 0 || s0 > 1){
        printf("error in addSubtract\n");
        exit(1);
    }
    if(s0 == 0){ // add
        ret = X + Y;
    }else{ // substract
        ret = X - Y;
    }
    return ret;
}

int checkSetLess(int X, int Y){
    int ret;

    if(addSubtract(X, Y, 1) < 0)
        ret = 1;
    else
        ret = 0;
    return ret;
}

int checkZero(int Oa){
    int ret;

    if(Oa == 0)
        ret = 1;
    else
        ret = 0;
    return ret;
}
