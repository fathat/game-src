#version 110

uniform sampler2D stage1, stage2, stage3, stage4;

void main()
{
	//vec4 texel1, texel2, texel3;
	vec4 result, fog_color;
	float fog;

	//read in all four textures
	const vec4 base = texture2D( stage1, gl_TexCoord[0].st );
	const vec4 texel2 = texture2D( stage2, gl_TexCoord[0].st );
	const vec4 alpha2 = texture2D( stage2, gl_TexCoord[1].st );
	const vec4 texel3 = texture2D( stage3, gl_TexCoord[0].st );
	const vec4 alpha3 = texture2D( stage3, gl_TexCoord[1].st );
	const vec4 texel4 = texture2D( stage4, gl_TexCoord[0].st );
	const vec4 alpha4 = texture2D( stage4, gl_TexCoord[1].st );
	
	//mix the textures based on alpha
	result.rgb = mix( base.rgb, texel2.rgb, alpha2.a )
	result.rgb = mix( result.rgb, texel3.rgb, alpha3.a )
	result.rgb = mix( result.rgb, texel4.rgb, alpha4.a )
	
	//mix in the fog
	fog = clamp( (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale, 0.0, 1.0 );
	result.r = mix(gl_Fog.color.r, result.r, fog );
	result.g = mix(gl_Fog.color.g, result.g, fog );
	result.b = mix(gl_Fog.color.b, result.b, fog );
	result.a = texel3.b;

	gl_FragColor = result;
}

