#include "stdafx.h"

#include "legacy.h"


const size_t VertexTypeSizeList[] =
{
    sizeof(float),  // D3DDECLTYPE_FLOAT1
    sizeof(float),  // D3DDECLTYPE_FLOAT2
    sizeof(float),  // D3DDECLTYPE_FLOAT3
    sizeof(float),  // D3DDECLTYPE_FLOAT4
    sizeof(char),   // D3DDECLTYPE_D3DCOLOR
    sizeof(char),   // D3DDECLTYPE_UBYTE4
    sizeof(short),  // D3DDECLTYPE_SHORT2
    sizeof(short),  // D3DDECLTYPE_SHORT4
    sizeof(char),   // D3DDECLTYPE_UBYTE4N
    sizeof(short),  // D3DDECLTYPE_SHORT2N
    sizeof(short),  // D3DDECLTYPE_SHORT4N
    sizeof(short),  // D3DDECLTYPE_USHORT2N
    sizeof(short),  // D3DDECLTYPE_USHORT4N
    sizeof(int),    // D3DDECLTYPE_UDEC3
    sizeof(int),    // D3DDECLTYPE_DEC3N
    sizeof(short),  // D3DDECLTYPE_FLOAT16_2
    sizeof(short)   // D3DDECLTYPE_FLOAT16_4
};

const size_t VertexSizeList[] =
{
    1,  // D3DDECLTYPE_FLOAT1
    2,  // D3DDECLTYPE_FLOAT2
    3,  // D3DDECLTYPE_FLOAT3
    4,  // D3DDECLTYPE_FLOAT4
    4,  // D3DDECLTYPE_D3DCOLOR
    4,  // D3DDECLTYPE_UBYTE4
    2,  // D3DDECLTYPE_SHORT2
    4,  // D3DDECLTYPE_SHORT4
    4,  // D3DDECLTYPE_UBYTE4N
    2,  // D3DDECLTYPE_SHORT2N
    4,  // D3DDECLTYPE_SHORT4N
    2,  // D3DDECLTYPE_USHORT2N
    4,  // D3DDECLTYPE_USHORT4N
    1,  // D3DDECLTYPE_UDEC3
    1,  // D3DDECLTYPE_DEC3N
    2,  // D3DDECLTYPE_FLOAT16_2
    4   // D3DDECLTYPE_FLOAT16_4
};


//-----------------------------------------------------------------------------
size_t
GetDeclLength
        ( VertexFormat const   *decl
        )
{
    VertexFormat const *element;
    for (element = decl; element->stream != 0xff; ++element);
    return element - decl;
}


//-----------------------------------------------------------------------------
size_t
GetDeclVertexSize
        ( VertexFormat const   *decl
        , uint32_t              stream)
{
    size_t size = 0;
    for (int i = 0; i < 64; ++i)
    {
        VertexFormat const &desc = decl[i];

        if (desc.stream == 0xff)
            break;

        size += VertexSizeList[desc.type] * VertexTypeSizeList[desc.type];
    }
    return size;
}
