const int perm[256] =
{
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95,
    96, 53, 194, 233, 7, 225, 140, 36, 103, 30,
    69, 142, 8, 99, 37, 240, 21, 10, 23, 190,
    6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
    94, 252, 219, 203, 117, 35, 11, 32, 57, 177,
    33, 88, 237, 149, 56, 87, 174, 20, 125, 136,
    171, 168, 68, 175, 74, 165, 71, 134, 139, 48,
    27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
    60, 211, 133, 230, 220, 105, 92, 41, 55, 46,
    245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
    18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217,
    226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
    255, 82, 85, 212, 207, 206, 59, 227, 47, 16,
    58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
    119, 248, 152, 2, 44, 154, 163, 70, 221, 153,
    101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
    19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
    112, 104, 218, 246, 97, 228, 251, 34, 242, 193,
    238, 210, 144, 12, 191, 179, 162, 241, 81, 51,
    145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
    222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
    78, 66, 215, 61, 156, 180
};

int hash(int x, int y)
{
    return perm[(perm[x % 255] + y) % 255];
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float grad(int hash, float x, float y)
{
    int h = hash & 7; // Convert low 3 bits of hash code
    float u = h < 4 ? x : y; // into 8 simple gradient directions,
    float v = h < 4 ? y : x; // and compute the dot product with (x,y).
    return ((h & 1) != 0 ? -u : u) + ((h & 2) != 0 ? -2.0f * v : 2.0f * v);
}

float perlinNoise(float2 pos)
{
    // Calculate the integer coordinates
    int X = (int) floor(pos.x) & 255;
    int Y = (int) floor(pos.y) & 255;
    
    // Calculate the fractional part of pos
    float fx = pos.x - floor(pos.x);
    float fy = pos.y - floor(pos.y);

    // Calculate fade curves for fx and fy
    float u = fade(fx);
    float v = fade(fy);

    // Hash coordinates of the 4 square corners
    int aa = hash(X, Y);
    int ab = hash(X, Y + 1);
    int ba = hash(X + 1, Y);
    int bb = hash(X + 1, Y + 1);

    // And add blended results from the 4 corners of the square
    float res = lerp(
        lerp(grad(aa, fx, fy), grad(ba, fx - 1, fy), u),
        lerp(grad(ab, fx, fy - 1), grad(bb, fx - 1, fy - 1), u),
        v
    );

    return res;
}


//PS_IN output = (PS_IN) 0;

//float3 start = input[0].vStartpos;
//float3 end = input[0].vEndpos;
//float3 direction = normalize(end - start);
//float length = distance(start, end);

//int numSegments = input[0].iNumSegments;
//float segmentLength = length / numSegments;
//float size = input[0].fThickness * 0.5f; // 사각형의 크기

//    for (
//int i = 0;i <
//numSegments; i++)
//    {
//float t = (float) i / (float) (numSegments - 1);
//float3 pos = lerp(start, end, t);

//        // 빌보딩을 위한 방향 계산
//float3 look = normalize(g_vCamPosition - pos);
//float3 right = normalize(cross(float3(0, 1, 0), look));
//float3 up = normalize(cross(look, right));

//        // 선의 방향을 고려한 회전 적용
//float3 adjustedRight = normalize(cross(direction, up));
//        up = normalize(cross(adjustedRight, look));

//        // 사각형의 네 꼭지점 계산
//float3 vertices[4] =
//{
//    pos + (-adjustedRight - up) * size,
//            pos + (adjustedRight - up) * size,
//            pos + (-adjustedRight + up) * size,
//            pos + (adjustedRight + up) * size
//};

//        for (
//int j = 0;j < 4; j++)
//        {
//            output.vPosition = mul(float4(vertices[j], 1.0f), g_ViewMatrix);
//            output.vPosition = mul(output.vPosition, g_ProjMatrix);
//            output.vTexcoord = float2((j == 1 || j == 3) ? 1 : 0, (j == 2 || j == 3) ? 1 : 0);
//            output.fLifeTime = 1.0f -
//t;  // 시작점에서 1, 끝점에서 0
//            outputStream.Append(output);
//        }
//        outputStream.RestartStrip();
//    }