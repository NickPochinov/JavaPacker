#include <windows.h>
#pragma pack(push, 1)
struct ICONDIR {
    WORD idReserved;
    WORD idType;
    WORD idCount;
};

struct ICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct GRPICONDIRENTRY {
    BYTE bWidth, bHeight, bColorCount, bReserved;
    WORD wPlanes, wBitCount;
    DWORD dwBytesInRes;
    WORD nID;
};
#pragma pack(pop)

struct GroupHeader {
    WORD idReserved;
    WORD idType;
    WORD idCount;
};