struct VOut
{
    float4 position : SV_POSITION;
    float2 texcoord: TEXCOORD;        // texture coordinates
};

VOut main(float4 position : POSITION, float2 texcoord : TEXCOORD)
{
    VOut output;

    output.position = position;
    output.texcoord = texcoord;    // set the texture coordinates, unmodified

    return output;
}
