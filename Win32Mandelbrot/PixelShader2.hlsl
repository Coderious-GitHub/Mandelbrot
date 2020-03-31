cbuffer cBuffer
{
    double2 C;
    double width;
    double height;
    float2 screen;
    uint maxIte;
};


// Pixel Shader
float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    double real, imag;
    double real2, imag2;
    double xAdjust = position[0] / screen.x * width;
    double yAdjust = position[1] / screen.y * height;
    uint ite = 0;
    float4 CalcColor = { 0.0f , 0.0f, 0.0f, 1.0f };

    real = C.x + xAdjust;
    imag = C.y + yAdjust;

    //cardioid / bulb checking optimization    
    //Cardioid
    double tempImag2 = imag * imag;
    double temp = real - 0.25;
    double q = temp * temp + tempImag2;
    double a = q * (q + temp);
    double b = 0.25 * tempImag2;
    if (a < b)
        return CalcColor;

    //Period-2 bulb
    temp = real + 1.0;
    temp = temp * temp + tempImag2;
    if (temp < 0.0625)
        return CalcColor;


    for (uint i = 0; i < maxIte; i++)
    {
        real2 = real * real;
        imag2 = imag * imag;

        if (real2 + imag2 > 4.0)
        {
            break;
        }
        else {
            imag = (2 * real * imag + C.y + yAdjust);
            real = (real2 - imag2 + C.x + xAdjust);
            ite++;
        }
    }

    //Ultra Fractal
  if (ite != maxIte)
   {
        int colorNr = ite % 16;

        switch (colorNr)
        {
        case 0:
        {
            CalcColor[0] = 66.0f / 255.0f;
            CalcColor[1] = 30.0f / 255.0f;
            CalcColor[2] = 15.0f / 255.0f;

            break;
        }
        case 1:
        {
            CalcColor[0] = 25.0f / 255.0f;
            CalcColor[1] = 7.0f / 255.0f;
            CalcColor[2] = 26.0f / 255.0f;
            break;
        }
        case 2:
        {
            CalcColor[0] = 9.0f / 255.0f;
            CalcColor[1] = 1.0f / 255.0f;
            CalcColor[2] = 47.0f / 255.0f;
            break;
        }

        case 3:
        {
            CalcColor[0] = 4.0f / 255.0f;
            CalcColor[1] = 4.0f / 255.0f;
            CalcColor[2] = 73.0f / 255.0f;
            break;
        }
        case 4:
        {
            CalcColor[0] = 0.0f / 255.0f;
            CalcColor[1] = 7.0f / 255.0f;
            CalcColor[2] = 100.0f / 255.0f;
            break;
        }
        case 5:
        {
            CalcColor[0] = 12.0f / 255.0f;
            CalcColor[1] = 44.0f / 255.0f;
            CalcColor[2] = 138.0f / 255.0f;
            break;
        }
        case 6:
        {
            CalcColor[0] = 24.0f / 255.0f;
            CalcColor[1] = 82.0f / 255.0f;
            CalcColor[2] = 177.0f / 255.0f;
            break;
        }
        case 7:
        {
            CalcColor[0] = 57.0f / 255.0f;
            CalcColor[1] = 125.0f / 255.0f;
            CalcColor[2] = 209.0f / 255.0f;
            break;
        }
        case 8:
        {
            CalcColor[0] = 134.0f / 255.0f;
            CalcColor[1] = 181.0f / 255.0f;
            CalcColor[2] = 229.0f / 255.0f;
            break;
        }
        case 9:
        {
            CalcColor[0] = 211.0f / 255.0f;
            CalcColor[1] = 236.0f / 255.0f;
            CalcColor[2] = 248.0f / 255.0f;
            break;
        }
        case 10:
        {
            CalcColor[0] = 241.0f / 255.0f;
            CalcColor[1] = 233.0f / 255.0f;
            CalcColor[2] = 191.0f / 255.0f;
            break;
        }
        case 11:
        {
            CalcColor[0] = 248.0f / 255.0f;
            CalcColor[1] = 201.0f / 255.0f;
            CalcColor[2] = 95.0f / 255.0f;
            break;
        }
        case 12:
        {
            CalcColor[0] = 255.0f / 255.0f;
            CalcColor[1] = 170.0f / 255.0f;
            CalcColor[2] = 0.0f / 255.0f;
            break;
        }
        case 13:
        {
            CalcColor[0] = 204.0f / 255.0f;
            CalcColor[1] = 128.0f / 255.0f;
            CalcColor[2] = 0.0f / 255.0f;
            break;
        }
        case 14:
        {
            CalcColor[0] = 153.0f / 255.0f;
            CalcColor[1] = 87.0f / 255.0f;
            CalcColor[2] = 0.0f / 255.0f;
            break;
        }
        case 15:
        {
            CalcColor[0] = 106.0f / 255.0f;
            CalcColor[1] = 52.0f / 255.0f;
            CalcColor[2] = 3.0f / 255.0f;
            break;
        }
        }
    }

    return CalcColor;
}