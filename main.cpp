#include <iostream>
#include <fstream>
#include "structures.h"

template <typename t>
void read(std::ifstream &fp, t &result, std::size_t size){
    fp.read(reinterpret_cast<char*>(&result), size);
}

unsigned char bitextract(const unsigned int byte, const unsigned int mask);

class BMPReader{
public:
    BMPReader(): rgbInfo(nullptr){}
    void openBMP(const std::string& fileName){
        if (!readyFlag){
            fileStream.open(fileName, std::ifstream::binary);
            if (!fileStream){
                std::cout << "Error opening file '" << fileName << "'\n";
                return;
            }

            read(fileStream, fileHeader.bfType, sizeof(fileHeader.bfType));
            read(fileStream, fileHeader.bfSize, sizeof(fileHeader.bfSize));
            read(fileStream, fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1));
            read(fileStream, fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2));
            read(fileStream, fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits));

            if (fileHeader.bfType != 0x4D42){ // 42 - B, 4D - M
                std::cout << "Error: '" << fileName << "' isn`t BMP file\n";
                fileStream.close();
                return;
            }

            read(fileStream, fileInfoHeader.biSize, sizeof(fileInfoHeader.biSize));

            if (fileInfoHeader.biSize >= 12){
                read(fileStream, fileInfoHeader.biWidth, sizeof(fileInfoHeader.biWidth));
                read(fileStream, fileInfoHeader.biHeight, sizeof(fileInfoHeader.biHeight));
                read(fileStream, fileInfoHeader.biPlanes, sizeof(fileInfoHeader.biPlanes));
                read(fileStream, fileInfoHeader.biBitCount, sizeof(fileInfoHeader.biBitCount));
            }

            int bitsOnColor = 8, maskValue = (1 << bitsOnColor) - 1;

            if (fileInfoHeader.biSize >= 40){
                read(fileStream, fileInfoHeader.biCompression, sizeof(fileInfoHeader.biCompression));
                read(fileStream, fileInfoHeader.biSizeImage, sizeof(fileInfoHeader.biSizeImage));
                read(fileStream, fileInfoHeader.biXPelsPerMeter, sizeof(fileInfoHeader.biXPelsPerMeter));
                read(fileStream, fileInfoHeader.biYPelsPerMeter, sizeof(fileInfoHeader.biYPelsPerMeter));
                read(fileStream, fileInfoHeader.biClrUsed, sizeof(fileInfoHeader.biClrUsed));
                read(fileStream, fileInfoHeader.biClrImportant, sizeof(fileInfoHeader.biClrImportant));
            }

            fileInfoHeader.biRedMask = 0;
            fileInfoHeader.biGreenMask = 0;
            fileInfoHeader.biBlueMask = 0;

            if (fileInfoHeader.biSize >= 52){
                read(fileStream, fileInfoHeader.biRedMask, sizeof(fileInfoHeader.biRedMask));
                read(fileStream, fileInfoHeader.biGreenMask, sizeof(fileInfoHeader.biGreenMask));
                read(fileStream, fileInfoHeader.biBlueMask, sizeof(fileInfoHeader.biBlueMask));
            }

            if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0){
                fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
                fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
                fileInfoHeader.biBlueMask = maskValue;
            }

            if (fileInfoHeader.biSize >= 56){
                read(fileStream, fileInfoHeader.biAlphaMask, sizeof(fileInfoHeader.biAlphaMask));
            } else{
                fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
            }

            if (fileInfoHeader.biSize >= 108){
                read(fileStream, fileInfoHeader.biCSType, sizeof(fileInfoHeader.biCSType));
                read(fileStream, fileInfoHeader.biEndpoints, sizeof(fileInfoHeader.biEndpoints));
                read(fileStream, fileInfoHeader.biGammaRed, sizeof(fileInfoHeader.biGammaRed));
                read(fileStream, fileInfoHeader.biGammaGreen, sizeof(fileInfoHeader.biGammaGreen));
                read(fileStream, fileInfoHeader.biGammaBlue, sizeof(fileInfoHeader.biGammaBlue));
            }

            if (fileInfoHeader.biSize >= 124){
                read(fileStream, fileInfoHeader.biIntent, sizeof(fileInfoHeader.biIntent));
                read(fileStream, fileInfoHeader.biProfileData, sizeof(fileInfoHeader.biProfileData));
                read(fileStream, fileInfoHeader.biProfileSize, sizeof(fileInfoHeader.biProfileSize));
                read(fileStream, fileInfoHeader.biReserved, sizeof(fileInfoHeader.biReserved));
            }

            if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
                fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124){
                std::cout << "Unknown BMP format\n";
                fileStream.close();
                return;
            }

            if (fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32){
                std::cout << "Unsupported BMP bit count\n";
                fileStream.close();
                return;
            }

            if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3){
                std::cout << "Unsupported BMP compression\n";
                fileStream.close();
                return;
            }

            rgbInfo = new RGBQUAD* [fileInfoHeader.biHeight];
            if (!rgbInfo){
                std::cout << "Memory error\n";
                fileStream.close();
                return;
            }

            for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++){
                rgbInfo[i] = new RGBQUAD[fileInfoHeader.biWidth];
                if (!rgbInfo[i]){
                    std::cout << "Memory error\n";
                    readyFlag = true;
                    closeBMP();
                    return;
                }
            }

            int linePadding = (4 - (fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) % 4;
            unsigned int bufer;

            for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++){
                for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++){
                    read(fileStream, bufer, fileInfoHeader.biBitCount / 8);

                    rgbInfo[i][j].rgbRed = bitextract(bufer, fileInfoHeader.biRedMask);
                    rgbInfo[i][j].rgbGreen = bitextract(bufer, fileInfoHeader.biGreenMask);
                    rgbInfo[i][j].rgbBlue = bitextract(bufer, fileInfoHeader.biBlueMask);
                    rgbInfo[i][j].rgbReserved = bitextract(bufer, fileInfoHeader.biAlphaMask);
                }
                fileStream.seekg(linePadding, std::ios_base::cur);
            }
            readyFlag = true;
        }
        else std::cout << "Close previous BMP file before, please\n";
    }
    void displayBMP(){
        if (readyFlag){
            for (unsigned int i = fileInfoHeader.biHeight; i-- > 0; ){
                for (unsigned int j = 0; j < fileInfoHeader.biWidth; j++){
                        if (rgbInfo[i][j].rgbRed == 0 && rgbInfo[i][j].rgbGreen == 0 && rgbInfo[i][j].rgbBlue == 0){
                            std::cout << "*";
                        }
                        else if (rgbInfo[i][j].rgbRed == 255 && rgbInfo[i][j].rgbGreen == 255 && rgbInfo[i][j].rgbBlue == 255){
                            std::cout << " ";
                        }
                    }
                std::cout << '\n';
            }
        }
        else std::cout << "Please, enter BMP file before\n";
    }
    void closeBMP(){
        if (readyFlag){
            for (unsigned int i = 0; i < fileInfoHeader.biHeight; i++){
                if (!rgbInfo[i]) break; //last non empty
                delete[] rgbInfo[i];
            }
            delete[] rgbInfo;
            fileStream.close();
            readyFlag = false;
        }
        else std::cout << "Please, enter BMP file before\n";
    }
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER fileInfoHeader;
    RGBQUAD **rgbInfo;
    std::ifstream fileStream;
    bool readyFlag = false;
};

int main(int argc, char* argv[]){
    if (argc < 2){
        std::cout << "Usage " << argv[0] << " - enter filename\n";
        return -1;
    }
    BMPReader a;
    a.openBMP(argv[1]);
    a.displayBMP();
    a.closeBMP();
    return 0;
}

unsigned char bitextract(const unsigned int byte, const unsigned int mask){
    if (mask == 0) return 0;
    int buf = mask, count = 0;
    while (!(buf & 1)){
        buf >>= 1;
        count++;
    }
    return (byte & mask) >> count;
}