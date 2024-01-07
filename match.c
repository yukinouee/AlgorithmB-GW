#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 1000
#define M 10000

int** data;
int P;

void readData(FILE *dataFile) {
    char ch; // ファイルから読み込む一時的な文字

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            ch = fgetc(dataFile);  // 1文字読み込む
            if (ch == '0' || ch == '1') {
                data[i][j] = ch - '0';  // 文字を整数に変換して格納
            } else {
                // 予期しない文字があった場合のエラー処理
                fprintf(stderr, "Unexpected character in data file: %c\n", ch);
                exit(1);
            }
        }
        ch = fgetc(dataFile);  // 行末の改行文字を読み飛ばす
        if (ch != '\n') {
            // 改行文字でない場合のエラー処理
            fprintf(stderr, "Expected newline at end of row, found: %c\n", ch);
            exit(1);
        }
    }

    fclose(dataFile);
}

typedef struct {
    int start;
    int range;
} Range;

void readRange(FILE *rangeFile,Range ranges[]) {
    for (int i = 0; i < P; i++) {
        fscanf(rangeFile, "%d, %d", &ranges[i].start, &ranges[i].range);
    }
    fclose(rangeFile);
}

int computeMatches(int person, Range range) {
    int endOfRange = range.start + range.range;
    for (int i = range.start; i < endOfRange; i++) {
        if (data[0][i] != data[person][i]) return 0;
    }
    return 1; // 全ての回答が一致した場合は1を返す、それ以外は0
}

void processRangesAndOutput(FILE *ansFile, Range ranges[]) {
    int count;
    for (int i = 0; i < P; i++) {
        count = 0;
        for (int j = 1; j < N; j++) { // 1から開始してAさんをスキップ
            count += computeMatches(j, ranges[i]);
        }
        // printf("range%d: %d people\n",i,count);
        fprintf(ansFile, "%d, %d, %d\n", ranges[i].start, ranges[i].range, count);
    }
    fclose(ansFile);
}


int main(int argc, char *argv[]) {
    // コマンドライン引数が正しい数であるか確認
    if (argc != 4) {
        printf("Usage: ./[Execute File] ./[datafile] ./[rangefile] ./[outputfile]\n", argv[0]);
        return -1;
    }

    // データファイルを開く
    FILE *dataFile = fopen(argv[1], "r");
    if (dataFile == NULL) {
        perror("Error opening data file.");
        return -1;
    }
    printf("Opened data file: %s\n", argv[1]);

    // 範囲ファイルを開く
    FILE *rangeFile = fopen(argv[2], "r");
    if (rangeFile == NULL) {
        perror("Error opening range file.");
        fclose(dataFile);  // 最初のファイルを閉じる
        return -1;
    }
    printf("Opened input range file: %s\n", argv[2]);

    // 出力ファイルを開く
    FILE *ansFile = fopen(argv[3], "w");
    if (ansFile == NULL) {
        perror("Error opening answer file");
        fclose(dataFile);  // 入力ファイルを閉じる
        fclose(rangeFile);
        return -1;
    }
    printf("Opened answer file: %s\n", argv[3]);

    char model[10];
    fscanf(rangeFile, "%s", model); // モデル名を読み取る
    if (strcmp(model, "MODEL1") == 0) {
        P = 30000;
    } else if (strcmp(model, "MODEL2") == 0) {
        P = 50000;
    } else {
        printf("select valid range file.\n");
        exit(1);
    }

    data = (int**)malloc(N * sizeof(int*));  
    for (int i = 0; i < N; i++) {
        data[i] = (int*)malloc(M * sizeof(int));  
    }

    Range *ranges;

    ranges = (Range*)malloc(P * sizeof(Range));

    readData(dataFile);
    readRange(rangeFile,ranges);
    processRangesAndOutput(ansFile,ranges);

    for (int i = 0; i < N; i++) {
        free(data[i]);  
    }
    free(data);  
    free(ranges);

    return 0;
}
