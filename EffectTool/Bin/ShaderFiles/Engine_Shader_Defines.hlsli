
sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = WRAP;
	AddressV = WRAP;
};


sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
};


RasterizerState		RS_Default
{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState		RS_Cull_CW
{
	FillMode = SOLID;
	CullMode = FRONT;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoCull
{
    FillMode = SOLID;
    CullMode = NONE;
};

RasterizerState		RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState	DSS_None_Test_None_Write
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};


//vector		vSrcColor = vector(1.f, 0.f, 0.f, 0.5f);
//vector		vDestColor = vector(0.f, 1.f, 0.f, 1.f);

//vector		vResult = vSrcColor.rgb * vSrcColor.a + 
//	vDestColor * (1.f - vSrcColor.a);

BlendState		BS_Default
{
	BlendEnable[0] = false;
};

BlendState		BS_AlphaBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState		BS_Blend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;	
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};