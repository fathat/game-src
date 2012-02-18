// Step 1: Create the effect file.
// This effect file maps a 2-D texture map onto the object.

texture tTexture0;
texture tTexture1;
texture aTexture0;
texture aTexture1;
texture tColorMap;
texture tAlphaMap;

int numTextures=2;
string sTexture0="..\\maps\\canyon\\grass.bmp";//"..\\art\\areas\\greens\\ground.bmp";
string sTexture1="..\\maps\\canyon\\cliff.bmp";
technique T0
{
    pass P0
    {        
        pixelshader  = NULL;
        fvf          = XYZ | Normal | Tex2;
        vertexshader = NULL;
 
        Lighting     = True;
        CullMode     = CCW;
	ZEnable      = True;
	AlphaBlendEnable = True;


	MagFilter[0] = Anisotropic;
	MipFilter[0] = Anisotropic;
	MinFilter[0] = Anisotropic;
	MagFilter[1] = Anisotropic;
	MipFilter[1] = Anisotropic;
	MinFilter[1] = Anisotropic;

	AddressU[0]  = Wrap;
	AddressV[0]  = Wrap;
 
        texture[0]   = < tTexture0 >;
        texture[1]   = < tAlphaMap >;
	TexCoordIndex[0] = 0;
	TexCoordIndex[1] = 1;
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
	AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        ColorOp[1]   = Modulate;
        ColorArg1[1] = Texture;
        ColorArg2[1] = Current;
	AlphaOp[1]   = SelectArg1;
	AlphaArg1[1] = Texture;
    }

}