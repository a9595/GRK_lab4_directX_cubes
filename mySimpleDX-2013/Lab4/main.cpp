//////////////////////////////////////////////////////////////////////////
#include <windows.h> //Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra WinAPI
#include <d3d9.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
#include <d3dx9.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
#include <math.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra matematyczne
#include <D3D9Types.h> // Plik naglowkowy zawieraj¹cy definicje funkcji, typy danych oraz makra DirectX
//#pragma comment(lib, "dxerr9.lib") // Dolaczamy biblioteke dxerr9.lib
#pragma comment(lib, "dxErr.lib") // Dolaczamy biblioteke dxErr.lib
#pragma comment(lib, "d3d9.lib") // Dolaczamy biblioteke d3d9.lib
#pragma comment(lib, "d3dx9.lib") // Dolaczamy biblioteke d3dx9.lib
#pragma comment(lib, "winmm.lib") // Dolaczamy biblioteke winmm.lib
#define SR(x) if(x){ x->Release(); x = NULL;} // makro do zwalniania zasobow
#define SAFE_RELEASE(x) {if(x) x->Release();x=NULL;} // makro do zwalniania zasobow
#define SAFE_DELETE(x) {if(x) delete(x);x=NULL;} // makro do usuwania zasobow
#define SDEL(x) {if(x) delete(x);x=NULL;} // makro do usuwania zasobow


//MYCODE
D3DXVECTOR3 m_vTranslation = D3DXVECTOR3(0, 0, 10);



LPDIRECT3D9 lpD3D = NULL; //Obiekt D3D 


LPDIRECT3DDEVICE9 lpD3DDevice = NULL; // Urzadzenie D3D

// Struktura wierzcholka zawierajaca jedynie pozycje oraz kolor
struct PlaneVert{

	D3DXVECTOR4 pos; // Pozycja 3d zapisana w postaci wektora 4D
};

LPDIRECT3DVERTEXBUFFER9		m_ptrVertexBuffer; // Bufor wierzcholkow
int numVert = 3; // Liczba wierzcholkow

LPDIRECT3DVERTEXDECLARATION9	m_pVertexDeclaration; // Deklaracja wierzcholkow. W niej podajemy z czego sklada sie struktura wierzcholka

LPD3DXEFFECT m_pEffect; //Wskaznik na efekt


//Macierze swiata, widoku oraz projekcji
D3DXMATRIX m_matWorld;
D3DXMATRIX m_matView;
D3DXMATRIX m_matProj;

int m_iTimer = 0;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//////////////////////////////////////////////////////////////////////////
// Funkcja inicjalizujaca efekt. Tworzy efekt z pliku Shader.fx, tworzy deklaracje wierzcholka itd.
//////////////////////////////////////////////////////////////////////////


bool InitEffect()
{


	LPD3DXBUFFER pErrorMsgs = NULL; // Bufor na ewentualne bledy
	DWORD dwFlags = 0;  // Flagi
	dwFlags |= D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER | D3DXSHADER_PREFER_FLOW_CONTROL;

	// Deklarujemy ze w strukturze wierzcholka znajduje sie jedynie pozycja zapisana na 4 float'ach oraz kolor takze na 4 float'ach
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};

	lpD3DDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration); // Tworzymy Deklaracje


	HRESULT hr;
	// Tworzymy efekt z pliku podajac urzadzenie, nazwe pliku, wskaznik na efekt i wskaznik na bufor bledow
	hr = D3DXCreateEffectFromFile(lpD3DDevice,
		L"shader.fx",
		NULL,
		NULL,
		0,
		NULL,
		&m_pEffect,
		&pErrorMsgs);
	// Jezeli nie udalo nam sie stworzyc efektu to wyrzucamy na ekran informacje z bledem
	if (FAILED(hr))
	{
		if (pErrorMsgs != NULL)
		{
			LPVOID pCompilErrors = pErrorMsgs->GetBufferPointer();

			char* tmp = (char*)pCompilErrors;
			size_t size = strlen(tmp) + 1;


			short* psMyName = new short[size];

			for (int i = 0; psMyName[i] = tmp[i++];);  //interesujaca pêtla kopiujaca stringa zakonczonego NULLem
			MessageBox(NULL, ((LPCWSTR)psMyName), L"EffectContainer Fx Compile Error", MB_OK | MB_ICONEXCLAMATION);
			delete[] psMyName;
		}
		else
			MessageBox(NULL, L"Nie mozna otworzyc pliku shader.fx", L"EffectContainer Fx Compile Error", MB_OK | MB_ICONEXCLAMATION);

		return false;
	}
	SR(pErrorMsgs);

	return true;
}
//////////////////////////////////////////////////////////////////////////
// Funkcja odpowiedzialna za wyswietlanie
//////////////////////////////////////////////////////////////////////////
int Render()
{
	// Czyscimy caly ekran, aby nie pozostaly zadne smieci po poprzedniej klatce. Czyscimy tez z-bufor
	if (lpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0) != D3D_OK){
		// Jezeli nie udalo sie wyczyscic ekranu to wyswietlamy blad
		MessageBox(0, L"Error while clearing device - will exit", L"Error", 0);

		return false;
	}

	lpD3DDevice->BeginScene(); //Rozpoczyna scene. Renderowac mozna tylko po jej rozpoczeciu
	// Ustawia deklaracje wierzcholkow


	m_pEffect->SetMatrix("matWorldViewProj", &(m_matWorld*m_matView*m_matProj)); // Przekazujemy do shader'a wymnozone macierze

	lpD3DDevice->SetVertexDeclaration(m_pVertexDeclaration);
	UINT uPasses;
	if (SUCCEEDED(m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE)))
	{
		for (UINT uPass = 0; uPass < uPasses; uPass++)
		{
			// Rozpoczynamy przejscie
			m_pEffect->BeginPass(uPass);

			lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


			// konczymy przejscie
			m_pEffect->EndPass();

		}
		m_pEffect->End();
	}



	lpD3DDevice->SetStreamSource(0, m_ptrVertexBuffer, 0, sizeof(PlaneVert));
	lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, numVert-2);

	lpD3DDevice->EndScene(); //konczymy scene. Po tym wywolaniu nie wolno juz nic renderowac
	lpD3DDevice->Present(0, 0, 0, 0); //Wyswietlamy na ekranie to co wyrenderowalismy do backbufora


	return 1;
}

//////////////////////////////////////////////////////////////////////////
//	Update logiki
//////////////////////////////////////////////////////////////////////////
void Update(){


	D3DXMATRIX matTrans;
	D3DXMATRIX matRot;
	D3DXMATRIX matScale;
	++m_iTimer;
	D3DXMatrixTranslation(&matTrans, 0, 0, 10);
	D3DXMatrixRotationYawPitchRoll(&matRot, (float)m_iTimer / 100, 0, 0);
	D3DXMatrixScaling(&matScale, 4, 4, 4);
	m_matWorld = matScale*matRot*matTrans;


}

//////////////////////////////////////////////////////////////////////////
// Funkcja inicjujaca DirectX
// Tworzy urzadzenie, bufor wierzcholkow itd
//////////////////////////////////////////////////////////////////////////
bool InitDx(HWND hWnd)
{
	//Tworzymy obiekt D3D
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!lpD3D){
		// Jesli sie nie udalo to wyswietlamy blad
		MessageBox(0, L"D3D object initialization error - no DirectX runtime?", L"Error", 0);
		return false;
	}

	// ustawiamy parametry wyswietlania. Mozna tu ustawic rozdzielczosc, formaty buforow itd
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));				//Czyscimy aby nie miec w parametrach smieci z pamieci
	pp.Windowed = true;						// Wyswietlamy w okienkt
	pp.BackBufferFormat = D3DFMT_UNKNOWN;   // Nieznany format backbufora
	pp.SwapEffect = D3DSWAPEFFECT_COPY;		// wybieramy sposob zamiany back i frontbufora
	pp.EnableAutoDepthStencil = true;		// ustawiamy automatyczny bufor glebokosci 
	pp.AutoDepthStencilFormat = D3DFMT_D16;	// ustawiamy  format 16 bitowy bufora glebokosci

	// Probujemy stworzyc urzadzenie DirectX. W zaleznosci od mozliwosci karty graficznej mozemy nie moc stworzyc niektorych 
	// konfiguracji urzadzenia. Probujemy zatem od najlepszej do najgorszej konfiguracji
	if (lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &lpD3DDevice) != D3D_OK){
		MessageBox(0, L"Error while creating device (hardware vertex processing) - attempting to create software device", L"Error", 0);

		if (lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &lpD3DDevice) != D3D_OK){
			MessageBox(0, L"Error while creating  device (software vertex processing) - attempting to create reference device", L"Error", 0);

			if (lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &lpD3DDevice) != D3D_OK){
				MessageBox(0, L"Error while creating REF device  - exiting", L"Error", 0);
				return false;
			}

		}
	}

	lpD3DDevice->CreateVertexBuffer(numVert * sizeof(PlaneVert),
		D3DUSAGE_DYNAMIC,
		D3DFVF_XYZ, D3DPOOL_DEFAULT,
		&m_ptrVertexBuffer, NULL);

	PlaneVert *pVertices;

	m_ptrVertexBuffer->Lock(0, numVert * sizeof(PlaneVert),
		(void**)&pVertices, D3DLOCK_DISCARD);


	pVertices[0].pos = D3DXVECTOR4(-0.5, -0.5, 0, 1);
	pVertices[1].pos = D3DXVECTOR4(0.5, -0.5, 0, 1);
	pVertices[2].pos = D3DXVECTOR4(0, 0.5, 0, 1);

	m_ptrVertexBuffer->Unlock();



	//Inicializacja macierzy
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixLookAtLH(&m_matView, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 10), &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 800 / 600, 1, 1400);

	//lpD3DDevice->SetStreamSource(0, m_ptrVertexBuffer, 0, sizeof(PlaneVert));





	// wywoalujemy funkcje InitEffect()
	return InitEffect();


}
//////////////////////////////////////////////////////////////////////////
// Funckja kasujaca obiekty i zwalniajaca zasoby.
//////////////////////////////////////////////////////////////////////////
int DelDx()
{
	SR(m_ptrVertexBuffer);
	SR(m_pEffect);
	SR(m_pVertexDeclaration);
	SR(lpD3DDevice);
	SR(lpD3D);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
// Glowna funkcja w aplikacjach Win32. Tworzone jest tu okno i wywolywana petla wiadomosci
//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG  msg;
	HWND hWnd;
	WNDCLASS wndClass;


	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//Styl okna - bedzie sie odmalowywalo przy kazdym przesunieciu lub zmianie rozdzielczosci   
	wndClass.lpfnWndProc = WndProc;								//Wskazujemy procedure przetwarzajaca komunikaty okna
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;							//Ustawiamy w oknie instancje naszego programu
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// Ladujemy z zasobow systemowych 
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// Domyslny kursor i ikone
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// Kolor tla okna
	wndClass.lpszMenuName = NULL;									// Nie tworzymy menu
	wndClass.lpszClassName = TEXT("SimpleDX");				// Nazwa klasy okna, wyswietlana w naglowku okna

	RegisterClass(&wndClass);		//Rejestrujemy klase okna w systemie


	hWnd = CreateWindow(
		TEXT("SimpleDX"),   // nazwa klasy okna
		TEXT("SimpleDX"),  // tytul okna
		WS_OVERLAPPEDWINDOW,      // styl
		CW_USEDEFAULT,            // pozycja x okna
		CW_USEDEFAULT,            // pozycja y okna
		800,					  // szerokosc okna 
		600,					  // wysokosc okna
		NULL,
		NULL,
		hInstance,
		NULL);



	RECT rect = { 0, 0, 800, 600 }; //Tworzymy prostokat o wymiarach 800x600
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), FALSE); //Skalujemy okno tak aby obszar roboczy faktycznie mial 800x600px
	SetWindowPos(hWnd, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);

	ShowWindow(hWnd, nCmdShow);	//Wyswietlamy okno
	UpdateWindow(hWnd);
	// Jesli nie uda sie zainicjowac DirectX to wychodzimy z programu
	if (!InitDx(hWnd)){
		return(0);
	}
	// Petla wiadomosci
	do
	{
		// sprawdzamy czy jest wiadomosc.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Jesli jest wiadomosc to ja tlumaczy
			TranslateMessage(&msg);
			// I przkazujemy do okna
			DispatchMessage(&msg);
		}
		else // Jesli nie ma wiadomosci
		{
			Update();
			if (!Render()){ // Probujemy renderowac. Ale jesli sie nie uda
				DelDx(); // to usuwamy zasoby
				return 0; // i wychodzimy z programu
			}

		}
	} while (msg.message != WM_QUIT);

	// Usuwamy zasoby 
	DelDx();

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{


	switch (msg)
	{
	case WM_DESTROY: // Zamkniecie okna

		PostQuitMessage(0); // Ta funkcja dodaje do kolejki wiadomosci WM_QUIT
		return 0;

	case WM_KEYDOWN: // Wcisniecie klawisza

		switch (wParam){

		case VK_UP:
			m_vTranslation.y += 0.1;
			break;
		case VK_DOWN:
			m_vTranslation.y -= 0.1;
			break;
		case VK_RIGHT:
			m_vTranslation.x += 0.1;
			break;
		case VK_LEFT:
			m_vTranslation.x -= 0.1;
		
			break;
		case VK_SPACE:
			break;
		case VK_ESCAPE: // jesli klawisz ESC
			PostQuitMessage(0); // Wylaczamy program
			break;
		}

	}

	// wszystkie inne wiadomosci sa obslugiwane w sposob domyslny
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
