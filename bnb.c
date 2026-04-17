#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h> // malloc, free, calloc のために必要
#include <string.h> // memcpy のために必要
#include "simplex.h"

#define NUM_VARS 4//変数の数
#define NUM_SLACKS 5//スラック変数の数
#define ROWS (NUM_SLACKS + 1)//行数(制約式５＋目的関数１)；
#define COLS (NUM_VARS + NUM_SLACKS + 1)//(変数の数＋スラック変数の数＋右辺)

// simplex.hと重複する場合は既存の定義を優先、なければ定義
#ifndef DBL_EPSILON
#define DBL_EPSILON 1e-9
#endif

#define ACCESS(matrix, i, j) (matrix[(i) * cols + (j)])

// --- 構造体とグローバル変数 ---
typedef double X_State[NUM_VARS];//変数ｘの状態
/*例：X_State={-1,-1,-1,-1}->xiが全て未決定->通常のLP緩和問題, 
     X_State={0,-1,-1,-1}->x1が0に固定,他は自由
     X_State={1,0,-1,-1}->x1が１に固定x2は０に固定、他自由
     X_State={1,0,-1,-1}->xiが全て固定->解の候補の一つ
*/



// 現在見つかっている最良の整数解の目的関数値 Z
double best_z = -DBL_MAX; //おそらくintでとると誤差でバグる。
X_State best_x;//最も良い整数解を入れる。

// --- プロトタイプ宣言 ---
LPSolution solve_lp_relaxation(X_State current_x);//
void branch_and_bound(X_State current_x, int current_var_index);//分枝限定法

LPSolution solve_lp_relaxation(X_State current_x) {//X_State current_xはdouble current_x[NUM_VARS]と同義
    LPSolution sol={0};
    int i,j;
    int rows=ROWS; // 実際のタブローの行数
    int cols=COLS; // 実際のタブローの列数
    const size_t total_size_bytes = rows * cols * sizeof(double); // 確保するバイトサイズ
    double *temp_matrix = (double *)malloc(total_size_bytes); // タブローデータを格納するメモリを確保し、その先頭のアドレスを返す。

    if (temp_matrix == NULL) {
        // メモリ確保に失敗した場合の処理
        fprintf(stderr, "Error: Memory allocation failed in solve_lp_relaxation.\n");
        sol.feasible_judge = 0;
        return sol;
    }

      double initial_data[ROWS][COLS]={
    //{x1,x2,x3,x4,s1,s2,s3,s4,s5,b}
        {2,3,1,3,1,0,0,0,0,4},
        {1,0,0,0,0,1,0,0,0,1},
        {0,1,0,0,0,0,1,0,0,1},
        {0,0,1,0,0,0,0,1,0,1},
        {0,0,0,1,0,0,0,0,1,1},
        {-3,-4,-1,-2,0,0,0,0,0,0}
    };  
    //printf("initial data");
    //print_matrix(rows,cols,(double *)initial_data);
// 静的配列(initial_data[ROWS][COLS])の内容を、動的メモリへ高速にコピー
    memcpy(temp_matrix, initial_data, total_size_bytes);//(アドレスAにアドレスBの内容をC文コピー)
    //printf("temp_matrix(変化前)");
    //print_matrix(rows,cols,(double *)temp_matrix);
    
    //固定した変数に応じてタブローを変化させる。
    for (i = 0; i < NUM_VARS; i++){//タブローの行の数ループ
        if(current_x[i]!=-1){//current_x[i]（変数xi）の状態（0か１）
            if(current_x[i]==1){
                for(j=0;j < rows;j++){
                    ACCESS(temp_matrix,j,cols-1)-=ACCESS(temp_matrix,j,i);//固定したxiの列の値を右辺から引く(もし０に固定したならこの工程はいらない)
                }
            }
             for(j=0;j < rows;j++){
                ACCESS(temp_matrix,j,i)=0;//固定したxiの列を全て０に
            }
        }
    }
    //printf("temp_matrix(変化後)");
    simplex(rows,cols,temp_matrix,NUM_VARS);//temp_marixは変化

    sol.feasible_judge=1;//制約条件の実行可能性の判定の初期化（可能とする）、この後不可能なら０に
    for(j=0;j<rows-1;j++){
        if(ACCESS(temp_matrix,j,cols-1)<-DBL_EPSILON){//限りなく０に近いのなら
            sol.feasible_judge=0;
            break;
        }
    }

    if(sol.feasible_judge){//==1ならば
        sol.z_value=ACCESS(temp_matrix,rows-1,cols-1); //これが線形計画緩和解の上限値(この時ｘが全て整数ならば終了)
    }

    free(temp_matrix);
    return sol;
}
int num_of_calculations=1;
   // -----------分枝限定法--------------
   void branch_and_bound(X_State current_x,int current_var_index){
   LPSolution lp_sol=solve_lp_relaxation(current_x);
//〜〜〜〜現在の値の確認〜〜〜~
printf("\n---------値の確認------------\n");
    printf("分岐数:%d\n",num_of_calculations);
    printf("current_x=");
    printf("(");
    for(int i=0;i<NUM_VARS;i++){
     printf("%.0f",current_x[i]);
     if(i!=NUM_VARS-1){
        printf(",");
     }
    }
    printf(")\n");
    if(best_z<0){
        printf("暫定解:best_zは初期値(-DBL_MAX)\n");
    }
    else{
    printf("暫定解:best_z=%f\n",best_z);
    }
    printf("xi(best_x):(x1,x2,x3,x4)=");
    printf("(");
    for(int i=0;i<NUM_VARS;i++){
     printf("%1.f",best_x[i]);
     if(i!=NUM_VARS-1){
        printf(",");
     }
    }
    printf(")\n");
    if (!lp_sol.feasible_judge) {
     printf("上限値:(実行不可能のため無効)\n");
    } 
    else {
     printf("上限値(シンプレックス法により求めた): lp_sol.z_value=%lf\n", lp_sol.z_value);
    }
    //printf("-------------------------\n");

 
    if(!lp_sol.feasible_judge)return;//実行不可能ならば枝を捨てる
    if(lp_sol.z_value<=best_z+DBL_EPSILON)return ;//現在持っている上限値(best_z)がいままでの最適解以下ならば枝を捨てる。
    if(current_var_index == NUM_VARS){//固定された変数が決定変数の数と等しい（今回は４変数全て固定のパターン）と終了
        best_z=lp_sol.z_value;
        memcpy(best_x, current_x, sizeof(double) * NUM_VARS);
        printf("best_z,best_xが更新.\n");
        printf("NEW 暫定解:best_z=%f\n",best_z);
        printf("NEW xi(best_x):(x1,x2,x3,x4)=");
        printf("(");
        for(int i=0;i<NUM_VARS;i++){
           printf("%1.f",best_x[i]);
           if(i!=NUM_VARS-1){
            printf(",");
           }
        }
    printf(")\n");
        return;
    }

 
    //枝の分岐
    int next_var=current_var_index;


    //左の枝:next_varを0に固定して探索
    X_State branch_0;
    memcpy(branch_0, current_x, sizeof(double) * NUM_VARS);//double型は浮動小数点で８バイトに相当するためsizeof(double)は８を返す。
    branch_0[next_var]=0;
    num_of_calculations++;
    
    branch_and_bound(branch_0,next_var+1);//0が選択された先でさらに分岐していくため試行順としては（0000,0001,0010,0011といった感じに分岐）


    //右の枝:next_var を1に固定して探索
    X_State branch_1;
    memcpy(branch_1, current_x, sizeof(double) * NUM_VARS);
    branch_1[next_var]=1;
    num_of_calculations++;
    //printf("分岐数:%d\n",num_of_calculations);
    branch_and_bound(branch_1, next_var+1);
}
    

int main(void) {
    printf("maximize:3*(x1)+4*(x2)+(x3)+2*(x4)\n");
    printf("s.t.:2*(x1)+3*(x2)+(x3)+3*(x4)<=4\n");
    printf("xi∈{1,0}, i=(1,2,3,4)\n");
    //線形緩和してシンプレックス
    printf("\n線形緩和後の式\n");
    printf("maximize:3*(x1)+4*(x2)+(x3)+2*(x4)\n");
    printf("s.t.:2*(x1)+3*(x2)+(x3)+3*(x4)<=4\n");
    printf("0<=x1<=1\n");
    printf("0<=x2<=1\n");
    printf("0<=x3<=1\n");
    printf("0<=x4<=1\n\n");

    printf("分枝限定法開始\n");
    X_State initial_x;
    for (int i = 0; i < NUM_VARS; i++) initial_x[i] = -1.0;//決定変数の状態を-1(未決定)に
    branch_and_bound(initial_x, 0);//初期の固定した変数xi(実際は固定されていないので全ての要素が−１)

    printf("\n-----------------------------\n");
    if (best_z > -DBL_MAX / 2) {
        printf("\n最大値 Z = %.2f\n", best_z);
        for (int i = 0; i < NUM_VARS; i++) {
            printf("x%d = %.0f\n", i + 1, best_x[i]);
        }
    } else {
        printf("解は見つからなかった\n");
    }

    return 0;
}