// Step 1: Create the effect file.
// This effect file maps a 2-D texture map onto the object.

texture tTexture0;
texture tTexture1;
texture aTexture0;
texture aTexture1;
texture tColorMap;
texture tAlphaMap;

int numTextures=2;
string sTexture0="..\\Maps\\snowisland\\snow2.bmp";//"..\\maps\\canyon\\grass.bmp";//"..\\art\\areas\\greens\\ground.bmp";
string sTexture1="..\\Maps\\snowisland\\dirt.bmp";//"..\\maps\\canyon\\cliff.bmp";

//cliff="..\Maps\snowisland\dirt.bmp"
//grass="..\Maps\snowisland\snow2.bmp"
technique T0
{
    pass P0
    {        
        pixelshader  = 
		asm
		{
			ps.1.1
			def c1, 0.2f, 0.2f, 0.2f, 1.0f
			tex t0
			tex t1
			tex t2
			tex t3
			mov r1.a, t2.a
			lrp r0, r1.a, t0, t1
			mul r0, r0, t3
			mul r0, r0, v0
			mul r1, r0, c1
			add r0, r0, r1
		};
		
        fvf          = XYZ | Normal | Tex2;
        vertexshader = NULL;
 
        Lighting     = True;
        CullMode     = CCW;
		ZEnable      = True;
		AlphaBlendEnable = False;


		MagFilter[0] = Anisotropic;
		MipFilter[0] = Anisotropic;
		MinFilter[0] = Anisotropic;
		MagFilter[1] = Anisotropic;
		MipFilter[1] = Anisotropic;
		MinFilter[1] = Anisotropic;

		AddressU[0]  = Wrap;
		AddressV[0]  = Wrap;
 
        texture[0]   = < tTexture0 >;
        texture[1]   = < tTexture1 >;
        texture[2]   = < tAlphaMap >;
        texture[3]   = < tColorMap >;

    }

}