#include <stdio.h> 
#include <float.h> 
#include <math.h> 
#include "simplex.h"
//修正後の配列アクセス用マクロ コードの可読性を高めるため ( ) 
#define ACCESS(matrix, i, j) (matrix[(i) * cols + (j)])
//デバック内容は一旦削除 //


// 1. print_matrix タブロー表の出力

void print_matrix(int rows, int cols, double *matrix) {
     printf("\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) { //一次元配列アクセスに修正
            printf("| %7.2f ",ACCESS(matrix ,i,j));
         }
        printf("|\n");
        }
    printf("\n");
}

// 2. pipot_col_get ピポット列: (pipot_col)の決定
int pipot_col_get(int rows, int cols, double *matrix) {
    double min = DBL_MAX;
    int pipot_col = 0;
    int last_row = rows - 1;
    for (int j = 0; j < cols; j++) {
        //? ? 一次元配列アクセスに修正
        if(ACCESS(matrix ,last_row , j) < min){
         min = ACCESS(matrix ,last_row , j); 
         pipot_col = j;
        }      
    }
    if(min < 0){
        return pipot_col;
    }
    else{
        return -1; // 終了
    }
}

// 3. pipot_row_getピポット行:(pipot_row)の決定:右辺をピポット列の値で割っ てその比を比較、最も小さいものの行をピポット行とする。値は行番号を返す

int pipot_row_get(int rows, int cols, double *matrix, int pipot_col){
    int last_col = cols - 1; // 最後の列(右辺値)
    int pipot_row = -1;
    double min_ratio = DBL_MAX;
    for (int i = 0; i < rows - 1; i++) {
        double divisor = ACCESS(matrix,i, pipot_col); // 一次元配列アクセスに修正
        if (divisor > DBL_EPSILON) { // 分母が正の場合のみ
            double ratio = ACCESS(matrix,i, last_col) / divisor; //一次元配列アクセスに修正
            if(ratio < min_ratio){
                min_ratio = ratio;
                pipot_row = i; 
            }
            
        } 
        
    }
    return pipot_row;
}

// 4. pipot_change0ピポット要素を0にして掃き出し法:
void pipot_change0(int rows, int cols, double *matrix, int pipot_row , int pipot_col){
    double pipot_num = ACCESS(matrix ,pipot_row , pipot_col);//一次元配列アクセスに
    // 要素を1にするためにピポット行をピポット値で割る 
    for(int j=0; j<cols; j++){
        ACCESS(matrix ,pipot_row , j) /= pipot_num;
    }
    //列のピポット要素以外の値を0に(掃き出し)
    for(int i = 0; i < rows; i++){
        if(i != pipot_row){
            double pipot_col_value = ACCESS(matrix ,i,pipot_col);
            for(int j=0; j<cols; j++){
                ACCESS(matrix ,i, j) -= (ACCESS(matrix ,pipot_row , j) *pipot_col_value);
            }
        }
    }
}


// 5. check_base_variable 関数の修正






