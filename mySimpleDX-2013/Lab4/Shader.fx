float4x4 matWorldViewProj; // Wymnozone macierze
struct VS_INPUT // Striktura wejsciowa Vertex Shadera
{
	float4  Pos     : POSITION; // Pozycja 
	float4  color   : COLOR; // Pozycja 

};

struct VS_OUTPUT // Struktura wyjsciowa vertex Shadera i wejsciowa Pixel Shadera
{
	float4  Pos     : POSITION;
	float4  color   : COLOR; // Pozycja 


};

struct PS_OUTPUT // Struktura wyjsciowa Pixel Shadera
{
	float4 Color : COLOR; // kolor
};

////////////////////////////////////////

VS_OUTPUT VShade(VS_INPUT In) // Ta funkcja wykonuje sie jako pierwsza. Dostaje od aplikacji pozycje i kolor wierzcholka
{
	VS_OUTPUT Out = (VS_OUTPUT)0; // Zerujemy strukture wyjsciowa
	Out.Pos = mul(In.Pos, matWorldViewProj); // Jako pozycje wyjsciowa ustawiamy lokalna pozycje wierzcholka przemnozona przez macierze Swiata, Widoku i Projekcji
	Out.color = In.color;
	return Out; // Zwracamy wypelniona strukture
}

PS_OUTPUT PShade(VS_OUTPUT In) // Ta funkcja wykonuje sie jako nastepna. Dostaje te dane, ktore zwrocila poprzednia funkcja
{
	PS_OUTPUT Out = (PS_OUTPUT)0; // Zerujemy strukture wyjsciowa
	Out.Color = In.color;

	return Out; // Zwracamy wypelniona strukture
}

technique tec0
{
	pass P0
	{
		VertexShader = compile vs_2_0 VShade(); // Ustawiamy funkcje Vertex shadera oraz wersje shadera, ktorej bedziemy uzywac (2.0)
		PixelShader = compile ps_2_0 PShade(); // Ustawiamy funkcje Pixel shadera oraz wersje shadera, ktorej bedziemy uzywac (2.0)

		ZEnable = True; // Wlaczamy z-bufor
		ZWriteEnable = True; // Wlaczamy zapis do z-bufora
		CullMode = None; // Wylaczamy usuwanie krawedzie odwroconych tylem do kamery
	}
}