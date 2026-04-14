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
        for (int j = 0; j < cols; j++) { / / 一次元配列アクセスに修正
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



