/**
 * @file seam_carver.cpp
 * @brief Implements seam carving for PGM images using dynamic programming.
 *
 * Reads a PGM (P2) image, preserves any initial comment lines, removes specified vertical
 * and horizontal seams, and writes the resized image to a new PGM file matching original
 * formatting (including comments and whitespace) so that plain `diff` shows no differences.
 */

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <cstdlib>

/**
 * @class Image
 * @brief Represents a grayscale PGM (P2) image, preserving comments.
 */
class Image {
private:
    int width_;                             ///< number of columns
    int height_;                            ///< number of rows
    int maxValue_;                          ///< maximum gray value
    std::vector<std::string> comments_;     ///< header comment lines (with '#')
    std::vector<std::vector<int>> pixels_;  ///< pixel data [row][col]

public:
    /**
     * @brief Load a P2 PGM, capturing comment lines.
     * @param filename Path to input PGM file.
     * @throws runtime_error on I/O or format error.
     */
    explicit Image(const std::string& filename) {
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
    void write(const std::string& filename) const {
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
    int getWidth() const { return width_; }
    /** @brief Get image height. */
    int getHeight() const { return height_; }

    /**
     * @brief Access pixel at (row, col).
     * @param row Row index.
     * @param col Column index.
     * @return Grayscale value.
     */
    int getPixel(int row, int col) const {
        return pixels_[row][col];
    }

    /**
     * @brief Remove a vertical seam (one column per row).
     * @param seam Vector of columns to remove for each row.
     */
    void removeSeam(const std::vector<int>& seam) {
        for (int i = 0; i < height_; ++i) {
            pixels_[i].erase(pixels_[i].begin() + seam[i]);
        }
        --width_;
    }

    /**
     * @brief Transpose image (swap rows & columns).
     */
    void transpose() {
        std::vector<std::vector<int>> tmp(width_, std::vector<int>(height_));
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                tmp[j][i] = pixels_[i][j];
            }
        }
        pixels_.swap(tmp);
        std::swap(width_, height_);
    }
};

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

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.pgm> <#vertical> <#horizontal>\n";
        return EXIT_FAILURE;
    }
    std::string infile = argv[1];
    int numV = std::atoi(argv[2]);
    int numH = std::atoi(argv[3]);

    try {
        Image img(infile);
        if (numV >= img.getWidth() || numH >= img.getHeight()) {
            std::cerr << "Error: requested seams (" << numV << "," << numH
                      << ") exceed dimensions (" << img.getWidth()
                      << "," << img.getHeight() << ")\n";
            return EXIT_FAILURE;
        }
        SeamCarver sc(img);
        sc.removeVerticalSeams(numV);
        sc.removeHorizontalSeams(numH);
        auto res = sc.getResult();

        auto pos = infile.find_last_of('.');
        std::string base = pos == std::string::npos ? infile : infile.substr(0, pos);
        std::string outfile = base + "_processed_" + std::to_string(numV)
                            + "_" + std::to_string(numH) + ".pgm";
        res.write(outfile);
        std::cout << "Saved: " << outfile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

