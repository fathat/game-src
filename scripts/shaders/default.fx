// Step 1: Create the effect file.
// This effect file maps a 2-D texture map onto the object.

texture tTexture0;

int numTextures=1;
string sTexture0="..\\maps\\canyon\\grassnew.bmp";
technique T0
{
    pass P0
    {        
        pixelshader  = NULL;
        fvf          = XYZ | Normal | Tex1;
        vertexshader = NULL;
 
        Lighting     = True;
        CullMode     = CCW;
	ZEnable      = True;
	AlphaBlendEnable = True;


	MagFilter[0] = Anisotropic;
	MipFilter[0] = Anisotropic;
	MinFilter[0] = Anisotropic;

	AddressU[0]  = Wrap;
	AddressV[0]  = Wrap;
 
        texture[0]   = < tTexture0 >;
	TexCoordIndex[0] = 0;
        ColorOp[0]   = Modulate;
        ColorArg1[0] = Texture;
        ColorArg2[0] = Diffuse;
	AlphaOp[0]   = SelectArg1;
        AlphaArg1[0] = Texture;
        ColorOp[1]   = Disable;


    }
}