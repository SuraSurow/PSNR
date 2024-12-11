#include "CImg-3.5.0_pre12042411/CImg.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>

using namespace cimg_library;
//      NIE KOMPILOWAĆ W IDE TYLKO W KONSOLI UŻYWAJĄC PONIŻSZEJ KOMENDY
//    g++ -o TeoriaKodowania6 main.cpp -lX11

int main() {
    std::vector<char*> imgs = {
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/airplane.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/baboonTMW.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/balloon.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/BARB.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/BARB2.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/camera256.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/couple256.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/GOLD.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/lennagrey.bmp",
        "/home/bolo/CLionProjects/TeoriaKodowania6/cmake-build-debug/peppersTMW.bmp"
    };

    const int MAX_VALUE = 256;
    std::ofstream csvFileEntropy("results_entropy.csv");
    std::ofstream csvFilePSNR("results_psnr.csv");

    // Nagłówki CSV
    csvFileEntropy << "Nazwa Pliku,Entropia przed mod.,1 Bit[dB],2 Bity[dB],3 Bity[dB],4 Bity[dB],5 Bitów[dB],6 Bitów[dB]\n";
    csvFilePSNR << "Nazwa Pliku,PSNR po odj. 1 najmłodszego bitu,PSNR po odj. 2 najmłodszych bitów,PSNR po odj. 3 najmłodszych bitów,PSNR po odj. 4 najmłodszych bitów,PSNR po odj. 5 najmłodszych bitów,PSNR po odj. 6 najmłodszych bitów\n";

    for (int i = 0; i < imgs.size(); i++) {
        const char* const imagePath = imgs[i];
        CImg<unsigned char> image(imagePath);

        int width = image.width();
        int height = image.height();
        int totalPixels = width * height;

        // Obliczenie entropii przed modyfikacjami
        std::vector<int> histogramGrayOriginal(MAX_VALUE, 0);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                unsigned char grayValue = image(x, y, 0, 0);
                histogramGrayOriginal[grayValue]++;
            }
        }

        double entropyOriginal = 0.0;
        for (int k = 0; k < MAX_VALUE; k++) {
            if (histogramGrayOriginal[k] > 0) {
                double p = static_cast<double>(histogramGrayOriginal[k]) / totalPixels;
                entropyOriginal -= p * std::log2(p);
            }
        }

        std::cout << "Nazwa obrazu: " << imagePath << ", Entropia przed modyfikacjami: " << entropyOriginal << std::endl;

        std::vector<double> vectorEntropy;
        std::vector<double> vectorPSNR;
        vectorEntropy.push_back(entropyOriginal);

        for (int j = 1; j <= 6; j++) {
            CImg<unsigned char> modifiedImage = image;
            std::vector<int> histogramGray(MAX_VALUE, 0);

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    unsigned char grayValue = image(x, y, 0, 0);
                    unsigned char newGrayValue = (grayValue >> j) << j;

                    modifiedImage(x, y, 0, 0) = newGrayValue;
                    histogramGray[newGrayValue]++;
                }
            }

            double entropyGray = 0.0;
            for (int k = 0; k < MAX_VALUE; k++) {
                if (histogramGray[k] > 0) {
                    double p = static_cast<double>(histogramGray[k]) / totalPixels;
                    entropyGray -= p * std::log2(p);
                }
            }

            vectorEntropy.push_back(entropyGray);

            // Liczenie PSNR
            double mse = 0.0;
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    unsigned char originalGray = image(x, y, 0, 0);
                    unsigned char newGray = modifiedImage(x, y, 0, 0);
                    double diff = static_cast<double>(originalGray) - static_cast<double>(newGray);
                    mse += diff * diff;
                }
            }
            mse /= (width * height);

            double PSNR = 10.0 * std::log10((255.0 * 255.0) / mse);
            vectorPSNR.push_back(PSNR);
        }

        // Zapis wyników entropii do CSV
        csvFileEntropy << std::filesystem::path(imagePath).filename().string();
        for (double entropy : vectorEntropy) {
            csvFileEntropy << "," << entropy;
        }
        csvFileEntropy << "\n";

        // Zapis wyników PSNR do CSV
        csvFilePSNR << std::filesystem::path(imagePath).filename().string();
        for (double psnr : vectorPSNR) {
            csvFilePSNR << "," << psnr;
        }
        csvFilePSNR << "\n";
    }

    csvFileEntropy.close();
    csvFilePSNR.close();

    std::cout << "Wyniki zapisane do plików 'results_entropy.csv' i 'results_psnr.csv'" << std::endl;

    return 0;
}
