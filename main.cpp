/**
 * @file seam_carving.cpp
 * @brief Object-oriented seam carving implementation for PGM (P2) and PPM (P3) images.
 *
 * Usage:
 *   seam_carving <input_file> <num_vertical> [<num_horizontal>]
 *
 * - input_file: Path to a PGM or PPM image (P2 or P3 format).
 * - num_vertical: Number of vertical seams to remove.
 * - num_horizontal: Number of horizontal seams to remove (default 0).
 */

#include <algorithm>
#include <array>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @class Image
 * @brief Abstract interface for image operations required by seam carving.
 */
class Image {
public:
    virtual ~Image() = default;

    /** @brief Read image data from file. */
    virtual void read(const std::string &filename) = 0;

    /** @brief Write image data to file. */
    virtual void write(const std::string &filename) const = 0;

    /** @brief Compute energy map of current image. */
    virtual std::vector<std::vector<int>> computeEnergy() const = 0;

    /** @brief Remove a single vertical seam. */
    virtual void removeVerticalSeam(const std::vector<int> &seam) = 0;

    /** @brief Remove a single horizontal seam. */
    virtual void removeHorizontalSeam(const std::vector<int> &seam) = 0;

    /** @brief Get current image width. */
    virtual int width() const = 0;

    /** @brief Get current image height. */
    virtual int height() const = 0;
};

/**
 * @class PGMImage
 * @brief Grayscale image in P2 (PGM) format.
 */
class PGMImage : public Image {
public:
    void read(const std::string &filename) override;
    void write(const std::string &filename) const override;
    std::vector<std::vector<int>> computeEnergy() const override;
    void removeVerticalSeam(const std::vector<int> &seam) override;
    void removeHorizontalSeam(const std::vector<int> &seam) override;
    int width() const override { return width_; }
    int height() const override { return height_; }

private:
    int width_{0}, height_{0}, maxVal_{0};
    std::vector<std::vector<int>> pixels_;

    /**
     * @brief Transpose a 2D int matrix.
     */
    std::vector<std::vector<int>>
    transpose(const std::vector<std::vector<int>> &mat) const {
        int h = static_cast<int>(mat.size());
        int w = static_cast<int>(mat[0].size());
        std::vector<std::vector<int>> out(w, std::vector<int>(h));
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                out[j][i] = mat[i][j];
            }
        }
        return out;
    }
};

/**
 * @class PPMImage
 * @brief Color image in P3 (PPM) format.
 */
class PPMImage : public Image {
public:
    void read(const std::string &filename) override;
    void write(const std::string &filename) const override;
    std::vector<std::vector<int>> computeEnergy() const override;
    void removeVerticalSeam(const std::vector<int> &seam) override;
    void removeHorizontalSeam(const std::vector<int> &seam) override;
    int width() const override { return width_; }
    int height() const override { return height_; }

private:
    int width_{0}, height_{0}, maxVal_{0};
    std::vector<std::vector<std::array<int,3>>> pixels_;

    /**
     * @brief Compute per-channel energy for given color channel.
     */
    std::vector<std::vector<int>>
    computeChannelEnergy(int channel) const {
        std::vector<std::vector<int>> energy(height_, std::vector<int>(width_));
        for (int i = 0; i < height_; ++i) {
            for (int j = 0; j < width_; ++j) {
                int v = pixels_[i][j][channel];
                int e = 0;
                if (i > 0)          e += std::abs(v - pixels_[i-1][j][channel]);
                if (i < height_-1)  e += std::abs(v - pixels_[i+1][j][channel]);
                if (j > 0)          e += std::abs(v - pixels_[i][j-1][channel]);
                if (j < width_-1)   e += std::abs(v - pixels_[i][j+1][channel]);
                energy[i][j] = e;
            }
        }
        return energy;
    }

    /**
     * @brief Transpose a 2D matrix of RGB pixels.
     */
    std::vector<std::vector<std::array<int,3>>>
    transpose(const std::vector<std::vector<std::array<int,3>>> &mat) const {
        int h = static_cast<int>(mat.size());
        int w = static_cast<int>(mat[0].size());
        std::vector<std::vector<std::array<int,3>>> out(w, std::vector<std::array<int,3>>(h));
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                out[j][i] = mat[i][j];
            }
        }
        return out;
    }
};

/**
 * @class SeamCarver
 * @brief Performs seam removal operations on an Image.
 */
class SeamCarver {
public:
    explicit SeamCarver(std::unique_ptr<Image> img)
        : image_(std::move(img)) {}

    /**
     * @brief Remove specified numbers of vertical and horizontal seams.
     */
    void carve(int numV, int numH) {
        for (int i = 0; i < numV; ++i) {
            auto energy = image_->computeEnergy();
            auto seam   = findMinVerticalSeam(energy);
            image_->removeVerticalSeam(seam);
        }
        for (int i = 0; i < numH; ++i) {
            auto energy = image_->computeEnergy();
            auto seam   = findMinHorizontalSeam(energy);
            image_->removeHorizontalSeam(seam);
        }
    }

    /** @brief Write processed image to file. */
    void save(const std::string &filename) const {
        image_->write(filename);
    }

private:
    std::unique_ptr<Image> image_;

    std::vector<int>
    findMinVerticalSeam(const std::vector<std::vector<int>> &energy) const {
        int h = static_cast<int>(energy.size());
        int w = static_cast<int>(energy[0].size());
        std::vector<std::vector<int>> M(h, std::vector<int>(w));
        // accumulate
        for (int j = 0; j < w; ++j) M[0][j] = energy[0][j];
        for (int i = 1; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                int best = M[i-1][j];
                if (j > 0)       best = std::min(best, M[i-1][j-1]);
                if (j+1 < w)     best = std::min(best, M[i-1][j+1]);
                M[i][j] = energy[i][j] + best;
            }
        }
        // backtrack
        std::vector<int> seam(h);
        seam[h-1] = static_cast<int>(
            std::min_element(M[h-1].begin(), M[h-1].end()) - M[h-1].begin()
        );
        for (int i = h-2; i >= 0; --i) {
            int prev = seam[i+1];
            int best = M[i][prev];
            int offset = 0;
            if (prev > 0 && M[i][prev-1] <= best) { best = M[i][prev-1]; offset = -1; }
            if (prev+1 < w && M[i][prev+1] < best) { best = M[i][prev+1]; offset = +1; }
            seam[i] = prev + offset;
        }
        return seam;
    }

    std::vector<int>
    findMinHorizontalSeam(const std::vector<std::vector<int>> &energy) const {
        int h = static_cast<int>(energy.size());
        int w = static_cast<int>(energy[0].size());
        std::vector<std::vector<int>> trans(w, std::vector<int>(h));
        for (int i = 0; i < h; ++i)
            for (int j = 0; j < w; ++j)
                trans[j][i] = energy[i][j];
        return findMinVerticalSeam(trans);
    }
};

// ---- PGMImage implementation ----
void PGMImage::read(const std::string &filename) {
    std::ifstream fin(filename);
    if (!fin) throw std::runtime_error("Cannot open file " + filename);

    std::string line;
    // Magic
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    if (line != "P2") throw std::runtime_error("Unsupported PGM format");
    // Dimensions
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    { std::istringstream iss(line); iss >> width_ >> height_; }
    // Max value
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    maxVal_ = std::stoi(line);
    // Pixels
    pixels_.assign(height_, std::vector<int>(width_));
    for (int i = 0; i < height_; ++i)
        for (int j = 0; j < width_; ++j)
            fin >> pixels_[i][j];
}

void PGMImage::write(const std::string &filename) const {
    std::ofstream fout(filename);
    fout << "P2\n" << width_ << " " << height_ << "\n" << maxVal_ << "\n";
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            fout << pixels_[i][j]
                 << (j+1<width_ ? ' ' : '\n');
        }
    }
}

std::vector<std::vector<int>> PGMImage::computeEnergy() const {
    std::vector<std::vector<int>> energy(height_, std::vector<int>(width_));
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            int e = 0;
            if (i>0)       e += std::abs(pixels_[i][j] - pixels_[i-1][j]);
            if (i<height_-1) e += std::abs(pixels_[i][j] - pixels_[i+1][j]);
            if (j>0)       e += std::abs(pixels_[i][j] - pixels_[i][j-1]);
            if (j<width_-1) e += std::abs(pixels_[i][j] - pixels_[i][j+1]);
            energy[i][j] = e;
        }
    }
    return energy;
}

void PGMImage::removeVerticalSeam(const std::vector<int> &seam) {
    for (int i = 0; i < height_; ++i)
        pixels_[i].erase(pixels_[i].begin() + seam[i]);
    --width_;
}

void PGMImage::removeHorizontalSeam(const std::vector<int> &seam) {
    auto trans = transpose(pixels_);
    for (int i = 0; i < width_; ++i)
        trans[i].erase(trans[i].begin() + seam[i]);
    pixels_ = transpose(trans);
    --height_;
}

// ---- PPMImage implementation ----
void PPMImage::read(const std::string &filename) {
    std::ifstream fin(filename);
    if (!fin) throw std::runtime_error("Cannot open file " + filename);

    std::string line;
    // Magic
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    if (line != "P3") throw std::runtime_error("Unsupported PPM format");
    // Dimensions
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    { std::istringstream iss(line); iss >> width_ >> height_; }
    // Max
    do { std::getline(fin, line); } while (line.empty() || line[0]=='#');
    maxVal_ = std::stoi(line);
    // Pixels
    pixels_.assign(height_, std::vector<std::array<int,3>>(width_));
    for (int i = 0; i < height_; ++i)
        for (int j = 0; j < width_; ++j)
            fin >> pixels_[i][j][0]
                >> pixels_[i][j][1]
                >> pixels_[i][j][2];
}

void PPMImage::write(const std::string &filename) const {
    std::ofstream fout(filename);
    fout << "P3\n" << width_ << " " << height_ << "\n" << maxVal_ << "\n";
    for (int i = 0; i < height_; ++i) {
        for (int j = 0; j < width_; ++j) {
            const auto &px = pixels_[i][j];
            fout << px[0] << ' ' << px[1] << ' ' << px[2]
                 << (j+1<width_ ? ' ' : '\n');
        }
    }
}

std::vector<std::vector<int>> PPMImage::computeEnergy() const {
    auto rE = computeChannelEnergy(0);
    auto gE = computeChannelEnergy(1);
    auto bE = computeChannelEnergy(2);
    std::vector<std::vector<int>> total(height_, std::vector<int>(width_));
    for (int i = 0; i < height_; ++i)
        for (int j = 0; j < width_; ++j)
            total[i][j] = rE[i][j] + gE[i][j] + bE[i][j];
    return total;
}

void PPMImage::removeVerticalSeam(const std::vector<int> &seam) {
    for (int i = 0; i < height_; ++i)
        pixels_[i].erase(pixels_[i].begin() + seam[i]);
    --width_;
}

void PPMImage::removeHorizontalSeam(const std::vector<int> &seam) {
    auto transposed = transpose(pixels_);
    for (int col = 0; col < width_; ++col)
        transposed[col].erase(transposed[col].begin() + seam[col]);
    pixels_ = transpose(transposed);
    --height_;
}

// ---- main ----
int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_file> <num_vertical> [<num_horizontal>]\n";
        return 1;
    }
    std::string input = argv[1];
    int numV = std::stoi(argv[2]);
    int numH = (argc == 4 ? std::stoi(argv[3]) : 0);

    auto dot = input.find_last_of('.');
    if (dot == std::string::npos) {
        std::cerr << "Error: Unknown file extension. Use .pgm or .ppm\n";
        return 1;
    }
    std::string ext = input.substr(dot+1);
    std::unique_ptr<Image> img;
    if (ext == "pgm" || ext == "PGM") {
        img = std::make_unique<PGMImage>();
    } else if (ext == "ppm" || ext == "PPM") {
        img = std::make_unique<PPMImage>();
    } else {
        std::cerr << "Error: Unsupported extension: " << ext << '\n';
        return 1;
    }

    try {
        img->read(input);
        SeamCarver carver(std::move(img));
        carver.carve(numV, numH);
        std::string out = input.substr(0, dot)
                          + "_processed_" + std::to_string(numV)
                          + "_" + std::to_string(numH)
                          + "." + ext;
        carver.save(out);
        std::cout << "Saved: " << out << '\n';
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

