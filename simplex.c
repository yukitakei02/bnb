#include <stdio.h>
#include <float.h>
#include <math.h>
#include "simplex.h"
	
	
//  修正後の配列アクセス用マクロ (コードの可読性を高めるため)
#define ACCESS(matrix, i, j) (matrix[(i) * cols + (j)])
	
//デバック内容は一旦削除
// ----------------------------------------------------------------------
// 1. print_matrix タブロー表の出力
// ----------------------------------------------------------------------
void print_matrix(int rows, int cols, double *matrix) {
	printf("\n");
	for (int i = 0; i < rows; i++) {       
		for (int j = 0; j < cols; j++) {
			//  一次元配列アクセスに修正
			printf("| %7.2f ", ACCESS(matrix,i,j));
		}
    	printf("|\n");
	}
	printf("\n");
}
// ----------------------------------------------------------------------
// 2. pipot_col_get :ピポット列 (pipot_col)の決定
// ----------------------------------------------------------------------
int pipot_col_get(int rows, int cols, double *matrix) {
	double min = DBL_MAX;
	int pipot_col = 0;
	int last_row = rows - 1; 	
	for (int j = 0; j < cols; j++) {
		// ?? 一次元配列アクセスに修正
		if(ACCESS(matrix,last_row, j) < min){
			min = ACCESS(matrix,last_row, j);
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
	
// ----------------------------------------------------------------------
// 3. pipot_row_get:ピポット行(pipot_row)の決定：右辺をピポット列の値で割ってその比を比較，最も小さいものの行をピポット行とする。return値は行番号
// ----------------------------------------------------------------------
int pipot_row_get(int rows, int cols, double *matrix, int pipot_col){
int last_col = cols - 1; // 最後の列（右辺値）
	int pipot_row = -1;
	double min_ratio = DBL_MAX;	
	for (int i = 0; i < rows - 1; i++) {
	    double divisor = ACCESS(matrix,i, pipot_col); // ?? 一次元配列アクセスに修正	
			if (divisor > DBL_EPSILON) { // 分母が正の場合のみ
				double ratio = ACCESS(matrix,i, last_col) / divisor; // ?? 一次元配列アクセスに修正
				
			if(ratio < min_ratio){
				min_ratio = ratio;
				pipot_row = i;
			}
		}
	}
	return pipot_row;
}
	
// ----------------------------------------------------------------------
// 4. pipot_change0:ピポット要素を０にして掃き出し法
// ----------------------------------------------------------------------
void pipot_change0(int rows, int cols, double *matrix, int pipot_row, int pipot_col){
	double pipot_num = ACCESS(matrix,pipot_row, pipot_col); // ?? 一次元配列アクセスに修正
		
	//pipot要素を１にするためにピポット行をピポット値で割る
	for(int j=0; j<cols; j++){
		ACCESS(matrix,pipot_row, j) /= pipot_num; 
	}
	// //pipot列のピポット要素以外の値を０に（掃き出し）
	for(int i = 0; i < rows; i++){
		if(i != pipot_row){
			double pipot_col_value = ACCESS(matrix,i, pipot_col); // ?? 一次元配列アクセスに修正
			for(int j=0; j<cols; j++){
				// ?? 掃き出し計算も全て一次元配列アクセスに修正
				ACCESS(matrix,i, j) -= (ACCESS(matrix,pipot_row, j) * pipot_col_value);
			}
		}
	}
}
	
// ----------------------------------------------------------------------
// 5. check_base_variable 関数の修正
// ----------------------------------------------------------------------
void check_base_variable(int rows, int cols, double *matrix,int variables_num){
	int base_variable[cols]; // ?? COLSではなく cols に依存
	int base_row_num[cols];
	// 確保した領域をゼロで初期化 (ごみデータ排除)←前に使っていた動的領域のデータが残っている可能性がある。
	for (int k = 0; k < cols; k++) {
		base_variable[k] = 0;
		base_row_num[k]=-1;
	}
	printf("基底:1，非基底:0\n");
	printf("(");   
	for(int i = 0; i < cols; i++){
		int count_one=0;
		int count_zero=0;
		int pipot_row_have_one;
		if(i!=0){
			printf(",");
		}
		for(int j=0; j<rows; j++){
			if(fabs(ACCESS(matrix,j, i) - 1.0) < DBL_EPSILON){//計算よって生まれる誤差
				count_one++;
				pipot_row_have_one = j;
			} 
			else if(fabs(ACCESS(matrix,j, i)) < DBL_EPSILON){
				count_zero++;
			}
		}
		if(count_one == 1 && (count_one + count_zero) == rows){
			// 基底変数としてマークし，その変数の値（右辺の値）を表示
			base_variable[i] = 1;
			base_row_num[i]=pipot_row_have_one;
		}
		printf(" %d ",  base_variable[i]);
			
		// 非基底変数は通常0なので，値を0とみなす
	}
	printf(")\n");
	printf("("); 
	for(int i = 0; i < variables_num; i++){
		if(i!=0){
			printf(" ,");
		}
		printf(" x%d",i+1);
	}
	printf(" ) = (");
	for(int i = 0; i < variables_num; i++){
		if(i!=0){
			printf(" ,");
		}
		if(base_variable[i] == 0){
			printf(" 0");
		}
		else if(base_variable[i] == 1){
			int row_index = base_row_num[i];
			if (row_index != -1) {
				printf("%0.2lf", ACCESS(matrix,row_index, cols-1)); // cols-1 は右辺値
			} 
			else {
				printf(" Error");
			}
		}
	}
	printf(" )\n");
		
		
		
	printf("--------------------------\n");
		
}
// ----------------------------------------------------------------------
// 6. simplex 関数の修正
// ----------------------------------------------------------------------
void simplex(int rows, int cols, double *matrix,int variables_num){
	printf("\n初期タブロー表");
	print_matrix(rows,cols,matrix);
	for(int q=0; q<rows*5 ; q++){ // 反復回数の上限を修正,反復の最大値は (ROWS*COLS)!/(ROWS!*COLS!)だが経験則的に制約式の５倍ほどで解けることが多いらしい
		int pipot_col = pipot_col_get(rows, cols, matrix);//pipot_col_getでreturn値が-1,つまり目的関数行に負の値がない場合
		if(pipot_col == -1){
		printf("\n終了時タブロー表");
		print_matrix(rows,cols,matrix);
			check_base_variable(rows, cols, matrix,variables_num);
			return;
		};
			
		int pipot_row = pipot_row_get(rows, cols, matrix, pipot_col);
			
		if (pipot_row == -1) {
			printf("\n[ERROR] ピボット行が見つかりません。非有界解または実行不可能です。\n");
			return;
		}
			
		pipot_change0(rows, cols, matrix, pipot_row, pipot_col);
		printf("%d回目反復終了時タブロー",q+1);
		print_matrix(rows,cols,matrix);
	}
	printf("\n--- 反復回数上限に到達 ---\n");
	print_matrix(rows,cols,matrix);
	check_base_variable(rows,cols,matrix,variables_num);
	// ... (終了処理) ...
	return;
}	








