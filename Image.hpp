#include <vector>
#include <string>
#include <cstdlib>

#ifndef IMAGE_HPP
#define IMAGE_HPP

/**
 * @class Image
 * @brief Represents a image, preserving comments.
 */
class Image {
private:
    int width_, height_, maxValue_;
    bool isColor_;
    std::vector<std::string> comments_;
    std::vector<std::vector<int>> gray_;                           // grayscale pixels
    std::vector<std::vector<std::array<int,3>>> color_;             // color pixels [row][col]

public:
    /**
     * @brief Load a P2 PGM, capturing comment lines.
     * @param filename Path to input PGM file.
     * @throws runtime_error on I/O or format error.
     */
    explicit Image(const std::string& filename); 

    /**
     * @brief Write image to a P2 PGM, presvers comments and matching whitespace.
     * @param filename Path to output file.
     * @throws runtime_error on I/O error.
     */
    void write(const std::string& filename) const; 

    /** @brief Get image width. */
    int getWidth() const; 

    /** @brief Get image height. */
    int getHeight() const;

    bool isColor()  const;

    /**
     * @brief Access grayscale pixel (if P2) or convert color to gray via average (for energy).
     */
    int grayValue(int r, int c) const;

    /**
     * @brief Access pixel at (row, col).
     * @param row Row index.
     * @param col Column index.
     * @return Grayscale value.
     */
    int getPixel(int row, int col) const; 

    /**
     * @brief Remove a vertical seam (one column per row).
     * @param seam Vector of columns to remove for each row.
     */
    void removeSeam(const std::vector<int>& seam); 

    /**
     * @brief Transpose image (swap rows & columns).
     */
    void transpose(); 
};

#endif // !IMAGE_HPP

