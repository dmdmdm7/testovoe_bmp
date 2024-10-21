using FXPT2DOT30 = int;

struct CIEXYZ{
    FXPT2DOT30 ciexyzX;
    FXPT2DOT30 ciexyzY;
    FXPT2DOT30 ciexyzZ;
};

struct CIEXYZTRIPLE{
    CIEXYZ ciexyzRed;
    CIEXYZ ciexyzGreen;
    CIEXYZ ciexyzBlue;
};

struct BITMAPFILEHEADER{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
};

struct BITMAPINFOHEADER{
    unsigned int biSize;
    unsigned int biWidth;
    unsigned int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPelsPerMeter;
    unsigned int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
    unsigned int biRedMask;
    unsigned int biGreenMask;
    unsigned int biBlueMask;
    unsigned int biAlphaMask;
    unsigned int biCSType;
    CIEXYZTRIPLE biEndpoints;
    unsigned int biGammaRed;
    unsigned int biGammaGreen;
    unsigned int biGammaBlue;
    unsigned int biIntent;
    unsigned int biProfileData;
    unsigned int biProfileSize;
    unsigned int biReserved;
};

struct RGBQUAD{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
};