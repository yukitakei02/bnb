#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// --- 定数定義 ---
#define NUM_VARS 4      // 変数の数
#define NUM_SLACKS 5    // スラック変数の数
#define ROWS (NUM_SLACKS + 1) // タブローの行数（制約式5 + 目的関数1）
#define COLS (NUM_VARS + NUM_SLACKS + 1) // タブローの列数（変数 + スラック + 右辺）

// 丸め誤差対策用の微小値
#ifndef DBL_EPSILON
#define DBL_EPSILON 1e-9
#endif

// --- 構造体定義 ---
// 変数xの状態を保持する型（-1.0: 未決定, 0.0: 0に固定, 1.0: 1に固定）
typedef double X_State[NUM_VARS]; 

// LP緩和解の結果を保持する構造体
typedef struct {
    double z_value;       // 目的関数値
    int integer_judge;    // 整数判定フラグ
    int feasible_judge;   // 実行可能フラグ（1: 可能, 0: 不可能）
    double x_sol[NUM_VARS]; // 解のベクトル
} LPSolution;

// --- マクロ定義 ---
// 一次元配列を二次元タブローとして扱うためのマクロ
#define ACCESS(matrix, i, j) (matrix[(i) * cols + (j)])

// --- 関数プロトタイプ宣言 ---

/* simplex.c に実装されている主要関数 */
void print_matrix(int rows, int cols, double *matrix);
int pipot_col_get(int rows, int cols, double *matrix);
int pipot_row_get(int rows, int cols, double *matrix, int pipot_col);
void pipot_change(int rows, int cols, double *matrix, int pipot_row, int pipot_col);
void check_base_variable(int rows, int cols, double *matrix, int variables_num);
void simplex(int rows, int cols, double *matrix, int variables_num);

/* bnb.c に実装されている主要関数 */
LPSolution solve_lp_relaxation(X_State current_x);
void branch_and_bound(X_State current_x, int current_var_index);

#endif // SIMPLEX_H