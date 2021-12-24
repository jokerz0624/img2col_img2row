
#include <iostream>
#include "timelog.h"

#define IMG2COL_TIME_PROFILE
#define IMG2ROW_TIME_PROFILE
#define TEST_CORRECTNESS
void img2col(float *src, float *dst, int ic, int iw, int ih, int k) {
    int ow = iw - k + 1;
    int oh = ih - k + 1;
    for (int c = 0; c < ic; ++c) {
        float *src_c = src + c * ih *iw;
        for (int m = 0; m < k; ++m) {
            float *src_m = src_c + m * iw;
            for (int n = 0; n < k; ++n) {
                float *src_n = src_m + n;
                for (int h = 0; h < oh; ++h) {
                    float *src_h = src_n + h * iw;
                    for (int w = 0; w < ow; ++w) {
                        dst[0] = src_h[w];
                        ++dst;
                    }
                }
            }
        }
    }
}

void img2row(float *src, float *dst, int ic, int iw, int ih, int k) {
    int ow = iw - k + 1;
    int oh = ih - k + 1;
    for (int h = 0; h < oh; ++h) {
        float *src_h = src + h * iw;
        for (int w = 0; w < ow; ++w) {
            float *src_w = src_h + w;
            for (int c = 0;c < ic; ++c) {
                float *src_c = src_w + c * ih * iw;
                for (int m = 0; m < k; ++m) {
                    float *src_m = src_c + m * iw;
                    for (int n = 0; n < k; ++n) {
                        dst[0] = src_m[n];
                        dst++;
                    }
                }
            }
        }
    }
}

void transpose(float *src, float *dst, int dst_h, int dst_w) {
    for (int i = 0; i < dst_h; ++i) {
        for (int j = 0; j < dst_w; ++j) {
            dst[i * dst_w + j] = src[j * dst_h + i];
        }
    }
}

int main(int argc, char **argv) {
    int ic = 8;
    int iw = 256;
    int ih = 256;
    int oc = 16;
    int k = 3;
    int ow = iw - k + 1;
    int oh = ih - k + 1;
    int src_len = ic * iw * ih;
    int dst_len = ic * k * k * ow * oh;
    float *src = new float[ic * iw * ih];
    float *dst_col = new float[dst_len];
    float *dst_row = new float[dst_len];
    float *dst_trans = new float[dst_len];
    for (int i = 0; i < src_len; ++i) {
        src[i] = i;
    }

    Timer timer;

// We can't test img2col and img2row together because of cache line
#if defined(IMG2COL_TIME_PROFILE)
    timer.reset();
    img2col(src, dst_col, ic, iw, ih, k);
    printf("img2col time cost: %f ms\n", timer.cost());
#elif defined(IMG2ROW_TIME_PROFILE)
    timer.reset();
    img2row(src, dst_row, ic, iw, ih, k);
    printf("img2row time cost: %f ms\n", timer.cost());
#elif defined(TEST_CORRECTNESS)
    img2col(src, dst_col, ic, iw, ih, k);
    img2row(src, dst_row, ic, iw, ih, k);
    transpose(dst_row, dst_trans, ic * k * k, ow * oh);
    bool flag = true;
    for (int i = 0; i < dst_len; ++i) {
        if (dst_col[i] != dst_trans[i]) {
            flag = false;
            printf("unmatch data in number %d\n", i);
        }
    }
    if (flag) printf("correctness test has passed.\n");
#endif

    delete[] src;
    delete[] dst_col;
    delete[] dst_row;
    delete[] dst_trans;
    return 0;
}
