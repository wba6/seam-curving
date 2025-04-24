#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "Image.hpp"

/**
 * @brief Load P2 or P3 image, capturing comment lines.
 * @param filename Path to input file.
 * @throws runtime_error on I/O or format error.
 */
Image::Image(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open input file");

    std::string magic;
    in >> magic;
    if      (magic == "P2") isColor_ = false;
    else if (magic == "P3") isColor_ = true;
    else throw std::runtime_error("Invalid magic (expected P2 or P3)");

    std::string line;
    std::getline(in, line);  // finish magic line
    // capture comments
    while (in.peek() == '#') {
        std::getline(in, line);
        comments_.push_back(line);
    }
    in >> width_ >> height_ >> maxValue_;
    if (width_<=0 || height_<=0 || maxValue_<=0)
        throw std::runtime_error("Invalid dimensions or max value");

    if (!isColor_) {
        gray_.assign(height_, std::vector<int>(width_));
        for (int i = 0; i < height_; ++i)
            for (int j = 0; j < width_; ++j)
                if (!(in >> gray_[i][j]))
                    throw std::runtime_error("Insufficient gray pixel data");
    } else {
        color_.assign(height_, std::vector<std::array<int,3>>(width_));
        for (int i = 0; i < height_; ++i)
            for (int j = 0; j < width_; ++j)
                for (int c = 0; c < 3; ++c)
                    if (!(in >> color_[i][j][c]))
                        throw std::runtime_error("Insufficient color pixel data");
    }
}

/**
 * @brief Write image in same format (P2 or P3), re-emitting comments.
 * @param filename Path to output file.
 * @throws runtime_error on I/O error.
 */
void Image::write(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Cannot open output file");

    // magic
    out << (isColor_ ? "P3" : "P2") << '\n';
    // comments
    for (const auto& c : comments_) out << c << '\n';
    // dimensions and max
    out << width_ << ' ' << height_ << '\n'
        << maxValue_ << '\n';

    // pixel data
    if (!isColor_) {
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                out << gray_[i][j] << ' ';
            }
            out << '\n';
        }
    } else {
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                const auto& p = color_[i][j];
                out << p[0] << ' ' << p[1] << ' ' << p[2] << ' ';
            }
            out << '\n';
        }
    }
}

int Image::getWidth()  const { return width_;  }
int Image::getHeight() const { return height_; }
bool Image::isColor()  const { return isColor_;  }

/**
 * @brief Access grayscale pixel (if P2) or convert color to gray via average (for energy).
 */
int Image::grayValue(int r, int c) const {
    if (!isColor_) return gray_[r][c];
    auto p = color_[r][c];
    return (p[0] + p[1] + p[2]) / 3;
}

/**
 * @brief Remove one vertical seam.
 */
void Image::removeSeam(const std::vector<int>& seam) {
    if (!isColor_) {
        for (int i = 0; i < height_; ++i)
            gray_[i].erase(gray_[i].begin() + seam[i]);
    } else {
        for (int i = 0; i < height_; ++i)
            color_[i].erase(color_[i].begin() + seam[i]);
    }
    --width_;
}

/**
 * @brief Transpose image (rows <-> cols).
 */
void Image::transpose() {
    if (!isColor_) {
        std::vector<std::vector<int>> tmp(width_, std::vector<int>(height_));
        for (int i = 0; i < height_; ++i)
            for (int j = 0; j < width_; ++j)
                tmp[j][i] = gray_[i][j];
        gray_.swap(tmp);
    } else {
        std::vector<std::vector<std::array<int,3>>> tmp(
            width_, std::vector<std::array<int,3>>(height_));
        for (int i = 0; i < height_; ++i)
            for (int j = 0; j < width_; ++j)
                tmp[j][i] = color_[i][j];
        color_.swap(tmp);
    }
    std::swap(width_, height_);
}
