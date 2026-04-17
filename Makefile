CC = gcc
CFLAGS = -Wall -g
LIBS = -lm

# ターゲット名
TARGET = main

# ソースファイル
SRCS = bnb.c simplex.c

# 実行ファイルの生成（ビルド）
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# --- ここから追加 ---
# 実行用ターゲット
run: $(TARGET)
	./$(TARGET)
# --- ここまで追加 ---

# クリーンアップ
clean:
	rm -f $(TARGET)