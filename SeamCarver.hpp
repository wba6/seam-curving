#include <vector>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include "Image.hpp"

#ifndef SEAMCARVER_HPP
#define SEAMCARVER_HPP

/**
 * @class SeamCarver
 * @brief Performs seam carving on an Image.
 */
class SeamCarver {
private:
    Image image_;

    /**
     * @brief Compute energy map (gradient magnitude).
     */
    std::vector<std::vector<int>> computeEnergy() const {
        int h = image_.getHeight();
        int w = image_.getWidth();
        std::vector<std::vector<int>> energy(h, std::vector<int>(w));
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                int val = image_.getPixel(i, j);
                int e = 0;
                if (i > 0)   e += abs(val - image_.getPixel(i - 1, j));
                if (i < h - 1) e += abs(val - image_.getPixel(i + 1, j));
                if (j > 0)   e += abs(val - image_.getPixel(i, j - 1));
                if (j < w - 1) e += abs(val - image_.getPixel(i, j + 1));
                energy[i][j] = e;
            }
        }
        return energy;
    }

    /**
     * @brief Find min-energy vertical seam via DP (leftmost tie-break).
     */
    std::vector<int> findVerticalSeam(const std::vector<std::vector<int>>& energy) const {
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

public:
    explicit SeamCarver(const Image& img) : image_(img) {}

    /**
     * @brief Remove N vertical seams.
     */
    void removeVerticalSeams(int count) {
        for (int k = 0; k < count; ++k) {
            auto E = computeEnergy();
            auto seam = findVerticalSeam(E);
            image_.removeSeam(seam);
        }
    }

    /**
     * @brief Remove N horizontal seams via transpose.
     */
    void removeHorizontalSeams(int count) {
        for (int k = 0; k < count; ++k) {
            image_.transpose();
            removeVerticalSeams(1);
            image_.transpose();
        }
    }

    /** @brief Get processed Image. */
    Image getResult() const { return image_; }
};

#endif // !SEAMCARVER_HPP

