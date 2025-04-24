#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include "Image.hpp"
#include "SeamCarver.hpp"

/**
 * @brief Compute energy map 
 */
std::vector<std::vector<int>> SeamCarver::computeEnergy() const {
   int h = image_.getHeight(), w = image_.getWidth();
    std::vector<std::vector<int>> E(h, std::vector<int>(w));
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int sum = 0;
            // for grayscale or average
            int v = image_.grayValue(i, j);
            if (i > 0)   sum += std::abs(v - image_.grayValue(i - 1, j));
            if (i < h-1) sum += std::abs(v - image_.grayValue(i + 1, j));
            if (j > 0)   sum += std::abs(v - image_.grayValue(i, j - 1));
            if (j < w-1) sum += std::abs(v - image_.grayValue(i, j + 1));
            E[i][j] = sum;
        }
    }
    return E;
}

/**
 * @brief Find min-energy vertical seam 
 */
std::vector<int> SeamCarver::findVerticalSeam(const std::vector<std::vector<int>>& energy) const {
    int h = energy.size();
    int w = energy[0].size();
    std::vector<std::vector<int>> M(h, std::vector<int>(w, std::numeric_limits<int>::max()));
    for (int j = 0; j < w; ++j) M[0][j] = energy[0][j];
    for (int i = 1; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int best = M[i - 1][j];
            if (j > 0)    best = std::min(best, M[i - 1][j - 1]);
            if (j < w - 1) best = std::min(best, M[i - 1][j + 1]);
            M[i][j] = energy[i][j] + best;
        }
    }
    std::vector<int> seam(h);
    int minj = 0;
    for (int j = 1; j < w; ++j) {
        if (M[h - 1][j] < M[h - 1][minj]) minj = j;
    }
    seam[h - 1] = minj;
    for (int i = h - 1; i > 0; --i) {
        int prev = seam[i];
        int start = std::max(0, prev - 1);
        int end = std::min(w - 1, prev + 1);
        int best = start;
        int val = M[i - 1][start];
        for (int k = start + 1; k <= end; ++k) {
            if (M[i - 1][k] < val) { val = M[i - 1][k]; best = k; }
        }
        seam[i - 1] = best;
    }
    return seam;
}

SeamCarver::SeamCarver(const Image& img) : image_(img) {}

/**
 * @brief Remove N vertical seams.
 */
void SeamCarver::removeVerticalSeams(int count) {
    for (int k = 0; k < count; ++k) {
        auto E = computeEnergy();
        auto seam = findVerticalSeam(E);
        image_.removeSeam(seam);
    }
}

/**
 * @brief Remove N horizontal seams via transpose.
 */
void SeamCarver::removeHorizontalSeams(int count) {
    for (int k = 0; k < count; ++k) {
        image_.transpose();
        removeVerticalSeams(1);
        image_.transpose();
    }
}

/** @brief Get processed Image. */
Image SeamCarver::getResult() const { return image_; }

