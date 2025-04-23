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
    std::vector<std::vector<int>> computeEnergy() const; 

    /**
     * @brief Find min-energy vertical seam via DP (leftmost tie-break).
     */
    std::vector<int> findVerticalSeam(const std::vector<std::vector<int>>& energy) const; 

public:
    explicit SeamCarver(const Image& img);

    /**
     * @brief Remove N vertical seams.
     */
    void removeVerticalSeams(int count); 

    /**
     * @brief Remove N horizontal seams via transpose.
     */
    void removeHorizontalSeams(int count); 

    /** @brief Get processed Image. */
    Image getResult() const; 
};

#endif // !SEAMCARVER_HPP

