#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "Image.hpp"

 /**
 * @brief Load a P2 PGM, capturing comment lines.
 * @param filename Path to input PGM file.
 * @throws runtime_error on I/O or format error.
 */
Image::Image(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open input file");

    // Read magic number
    std::string magic;
    in >> magic;
    if (magic != "P2") throw std::runtime_error("Invalid PGM magic (expected 'P2')");

    // Consume remainder of magic line
    std::string line;
    std::getline(in, line);

    // Capture any comment lines (preserve exact text)
    while (in.peek() == '#') {
        std::getline(in, line);
        comments_.push_back(line);
    }

    // Read dimensions and max gray value
    in >> width_ >> height_ >> maxValue_;
    if (width_ <= 0 || height_ <= 0 || maxValue_ <= 0) {
        throw std::runtime_error("Invalid image dimensions or max gray value");
    }

    // Load pixel data
    pixels_.assign(height_, std::vector<int>(width_));
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            if (!(in >> pixels_[i][j])) {
                throw std::runtime_error("Insufficient pixel data in PGM");
            }
        }
    }
}

/**
 * @brief Write image to a P2 PGM, re-emitting preserved comments and matching whitespace.
 * @param filename Path to output file.
 * @throws runtime_error on I/O error.
 */
void Image::write(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Cannot open output file");

    // Magic
    out << "P2\n";
    // Original comments
    for (const auto& c : comments_) {
        out << c << "\n";
    }
    // Dimensions and max gray value
    out << width_ << " " << height_ << "\n";
    out << maxValue_ << "\n";

    // Pixel rows: include trailing space on each line
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            out << pixels_[i][j] << ' ';
        }
        out << '\n';
    }
}

/** @brief Get image width. */
int Image::getWidth() const { return width_; }
/** @brief Get image height. */
int Image::getHeight() const { return height_; }

/**
 * @brief Access pixel at (row, col).
 * @param row Row index.
 * @param col Column index.
 * @return Grayscale value.
 */
int Image::getPixel(int row, int col) const {
    return pixels_[row][col];
}

/**
 * @brief Remove a vertical seam (one column per row).
 * @param seam Vector of columns to remove for each row.
 */
void Image::removeSeam(const std::vector<int>& seam) {
    for (int i = 0; i < height_; ++i) {
        pixels_[i].erase(pixels_[i].begin() + seam[i]);
    }
    --width_;
}

/**
 * @brief Transpose image (swap rows & columns).
 */
void Image::transpose() {
    std::vector<std::vector<int>> tmp(width_, std::vector<int>(height_));
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            tmp[j][i] = pixels_[i][j];
        }
    }
    pixels_.swap(tmp);
    std::swap(width_, height_);
}

