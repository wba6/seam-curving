/**
 * @file seam_carver.cpp
 * @brief Implements seam carving for PGM images using dynamic programming.
 *
 * Reads a PGM (P2) image, preserves any initial comment lines, removes specified vertical
 * and horizontal seams, and writes the resized image to a new PGM file matching original
 * formatting (including comments and whitespace) so that plain `diff` shows no differences.
 */

#include <string>
#include <array>
#include <iostream>
#include <cstdlib>
#include "Image.hpp"
#include "SeamCarver.hpp"

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
        std::string base = (pos==std::string::npos ? infile : infile.substr(0,pos));
        std::string ext  = (pos==std::string::npos ? ".pgm" : infile.substr(pos));
        std::string outfile = base + "_processed_" + std::to_string(numV)
                            + "_" + std::to_string(numH) + ext;
        res.write(outfile);
        std::cout << "Saved: " << outfile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

