#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define MAX(a, b, c) (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c)))

typedef struct {
    float *row;
    const float *prev_row;
    float match_reward;
    float mismatch_penalty;
    float indel_penalty;
    const char *s;
    const char *t;
    int index;
    int length;
} RowArgs;

typedef struct {
    float *col;
    const float *prev_col;
    float match_reward;
    float mismatch_penalty;
    float indel_penalty;
    const char *s;
    const char *t;
    int index;
    int length;
} ColArgs;

void *process_row(void *args) {
    RowArgs *row_args = (RowArgs *)args;
    float *row = row_args->row;
    const float *prev_row = row_args->prev_row;
    float match_reward = row_args->match_reward;
    float mismatch_penalty = row_args->mismatch_penalty;
    float indel_penalty = row_args->indel_penalty;
    const char *s = row_args->s;
    const char *t = row_args->t;
    int index = row_args->index;

    if (index == 0) {
        for (int i = 1; i <= row_args->length; i++) {
            row[i] = row[i - 1] + indel_penalty;
        }
    } else {
        for (int i = 1; i < row_args->length; i++) {
            if (s[index + i - 1] == t[index - 1]) {
                row[i] = MAX(prev_row[i] + match_reward,
                             prev_row[i + 1] + indel_penalty,
                             row[i - 1] + indel_penalty);
            } else {
                row[i] = MAX(prev_row[i] + mismatch_penalty,
                             prev_row[i + 1] + indel_penalty,
                             row[i - 1] + indel_penalty);
            }
        }
    }
    return NULL;
}

void *process_col(void *args) {
    ColArgs *col_args = (ColArgs *)args;
    float *col = col_args->col;
    const float *prev_col = col_args->prev_col;
    float match_reward = col_args->match_reward;
    float mismatch_penalty = col_args->mismatch_penalty;
    float indel_penalty = col_args->indel_penalty;
    const char *s = col_args->s;
    const char *t = col_args->t;
    int index = col_args->index;

    if (index == 0) {
        for (int i = 1; i <= col_args->length; i++) {
            col[i] = col[i - 1] + indel_penalty;
        }
    } else {
        for (int i = 1; i < col_args->length; i++) {
            if (s[index - 1] == t[index + i - 1]) {
                col[i] = MAX(prev_col[i] + match_reward,
                             prev_col[i + 1] + indel_penalty,
                             col[i - 1] + indel_penalty);
            } else {
                col[i] = MAX(prev_col[i] + mismatch_penalty,
                             prev_col[i + 1] + indel_penalty,
                             col[i - 1] + indel_penalty);
            }
        }
    }
    return NULL;
}

float global_alignment(float match_reward, float mismatch_penalty, float indel_penalty, const char *s, const char *t) {
    int m = strlen(s);
    int n = strlen(t);
    int i=0;

    float *prev_row = (float *)calloc(m + 1, sizeof(float));
    float *prev_col = (float *)calloc(n + 1, sizeof(float));

    pthread_t row_thread, col_thread;
    for (i = 0; i < fmin(m+1, n+1); i++) {
        float *row = (float *)calloc(m + 1 - i, sizeof(float));
        float *col = (float *)calloc(n + 1 - i, sizeof(float));
        if(i!=0){
            if(s[i-1] == t[i-1]){
                 row[0] = MAX(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty);
                 col[0] = MAX(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty);
                }
            else{
                 row[0] = MAX(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty);
                 col[0] = MAX(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty);
            }
        }

        RowArgs row_args = {row, prev_row, match_reward, mismatch_penalty, indel_penalty, s, t, i, m + 1 - i};
        ColArgs col_args = {col, prev_col, match_reward, mismatch_penalty, indel_penalty, s, t, i, n + 1 - i};

        pthread_create(&row_thread, NULL, process_row, &row_args);
        pthread_create(&col_thread, NULL, process_col, &col_args);

        pthread_join(row_thread, NULL);
        pthread_join(col_thread, NULL);

        memcpy(prev_row, row, (m + 1 - i) * sizeof(float));
        memcpy(prev_col, col, (n + 1 - i) * sizeof(float));
        free(row);
        free(col);
    }
    int x,y;
    x = m + 1 - i;
    y = n + 1 - i;
    float result = (x > y) ? prev_row[x] : prev_col[y];

    free(prev_row);
    free(prev_col);

    return result;
}
