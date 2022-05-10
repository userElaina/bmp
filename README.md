# BMP

#### BMP File Header

```c
typedef struct tagBITMAPFILEHEADER{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
}BITMAPFILEHEADER,*LPBITMAPFILEHEADER,*PBITMAPFILEHEADER;
```

`sizeof(BITMAPFILEHEADER)=14=0x0E`.

##### bfType

```java
0x00 0x01
42 4D
BM
const short
```

The file type; must be `BM`.

##### bfSize

```java
0x02   0x05
XX XX XX XX
int
```

The size, in bytes (**Little-Endian**), of the bitmap file.

```py
bfSize=int(file.size).to_bytes(4,'little')
```

##### bfReserved1 bfReserved2

```java
0x06   0x09
00 00 00 00
const int
```

Reserved; must be zero.

##### bfOffBits

```java
0x0A   0x0D
XX XX XX XX
int
```

The offset, in bytes, from the beginning of the `BITMAPFILEHEADER` structure to the bitmap bits.

For 24-rgb-bpps, `bfOffBits=54=0x36`,

```java
0x0A   0x0D
36 00 00 00
const int
```

#### BMP Info Header

```c
typedef struct tagBITMAPINFOHEADER{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
}BITMAPINFOHEADER,*PBITMAPINFOHEADER;
```

For 24-bpps, `sizeof(BITMAPINFOHEADER)=40=0x28`.

##### biSize

```java
0x0E   0x11
XX XX 00 00
int
```

The number of bytes required by the structure.

For 24-bpps, `biSize=40=0x28`,

```java
0x0E   0x11
28 00 00 00
const int
```

##### biWidth biHeight

```java
0x12   0x15     0x16   0x19
XX XX XX XX     XX XX XX XX
int             int
```

The width and the height of the bitmap, in pixels.

If `biCompression` is BI_JPEG or BI_PNG, the `biWidth` / `biHeight` member specifies the width / heights of the decompressed JPEG or PNG image file, respectively.

If `biHeight` is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If `biHeight` is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. If `biHeight` is negative, indicating a top-down DIB, `biCompression` must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be compressed.

##### biPlanes

```java
0x1A 0x1B
01 00
const short
```

The number of planes for the target device. This value must be set to 1.

##### biBitCount

```java
0x1C 0x1D
XX 00
short
```

The number of bits-per-pixel. The biBitCount member of the `BITMAPINFOHEADER` structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be in $\{0,1,2,4,8,16,24,32\}.$

For 24-rgb-bpps, `biBitCount=24=0x18`,

```java
0x1C 0x1D
18 00
const short
```

##### biCompression

```java
0x1E   0x21
0X 00 00 00
int
```

The type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed). This member can be one of the following values.

```c
#define BI_RGB __MSABI_LONG(0)
#define BI_RLE8 __MSABI_LONG(1)
#define BI_RLE4 __MSABI_LONG(2)
#define BI_BITFIELDS __MSABI_LONG(3)
#define BI_JPEG __MSABI_LONG(4)
#define BI_PNG __MSABI_LONG(5)
```

For 24-rgb-bpps, `biCompression=BI_RGB=0=0x00`,

```java
0x1E   0x21
00 00 00 00
const int
```

##### biSizeImage
```java
0x22   0x25
XX XX XX XX
int
```
The size, in bytes, of the image. This may be set to zero for `BI_RGB` bitmaps.

If `biCompression` is `BI_JPEG` or `BI_PNG`, `biSizeImage` indicates the size of the JPEG or PNG image buffer, respectively.

##### biXPelsPerMeter biYPelsPerMeter

```java
0x26   0x29     0x2A   0x2D
XX XX XX XX     XX XX XX XX
int             int
```

The horizontal / vertical resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to select a bitmap from a resource group that best matches the characteristics of the current device.

##### biClrUsed

```java
0x2E   0x31
XX XX XX XX
int
```

The number of color indexes in the color table that are actually used by the bitmap.

For 24-rgb-bpps, `biClrUsed=0=0x00`,

```java
0x2E   0x31
00 00 00 00
const int
```

##### biClrImportant

```java
0x32   0x35
XX XX XX XX
int
```

The number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required.

For 24-rgb-bpps, `biClrImportant=0=0x00`,

```java
0x33   0x36
00 00 00 00
const int
```

#### Example

##### General

```c
const short bfType=0x4d42;
int         bfSize;
const int   bfReserved=0;
int         bfOffBits;
int         biSize;
int         biWidth;
int         biHeight;
const short biPlanes=1;
short       biBitCount;
int         biCompression;
int         biSizeImage;
int         biXPelsPerMeter;
int         biYPelsPerMeter;
int         biClrUsed;
int         biClrImportant;
```

##### 24-rgb-bpps

```c
const short bfType=0x4d42;
int         bfSize;
const int   bfReserved=0;
const int   bfOffBits=0x36;
const int   biSize=0x28;
int         biWidth;
int         biHeight;
const short biPlanes=1;
const short biBitCount=0x18;
const int   biCompression=0;
int         biSizeImage;
const int   biXPelsPerMeter=0;
const int   biYPelsPerMeter=0;
const int   biClrUsed=0;
const int   biClrImportant=0;
```

```java
42 4D XX XX     XX XX 00 00     00 00 36 00     00 00 28 00
       [  bfSize   ]
00 00 XX XX     XX XX XX XX     XX XX 01 00     18 00 00 00
       [  biWidth  ]   [ biHeight  ]
00 00 XX XX     XX XX 00 00     00 00 00 00     00 00 00 00
       [biSizeImage]
00 00 00 00     00 00
```

##### `7680x4320` 24-rgb-bpps

```java
biWidth=7680=0x1E00;
biHeight=1080=0x10E0;
biSizeImage=biWidth*biHeight*3=0x05EEC000;
bfSize=biSizeImage+0x36=0x05EEC036; // =94.9MB
```

```java
42 4D 36 C0     EE 05 00 00     00 00 36 00     00 00 28 00
       [  bfSize   ]
00 00 00 1E     00 00 E0 10     00 00 01 00     18 00 00 00
       [  biWidth  ]   [ biHeight  ]
00 00 00 C0     EE 05 00 00     00 00 00 00     00 00 00 00
       [biSizeImage]
00 00 00 00     00 00
```

```py
b'BM6\xc0\xee\x05\x00\x00\x00\x006\x00\x00\x00(\x00'
b'\x00\x00\x00\x1e\x00\x00\xe0\x10\x00\x00\x01\x00\x18\x00\x00\x00'
b'\x00\x00\x00\xc0\xee\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
b'\x00\x00\x00\x00\x00\x00'
```

### Reference
https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)
