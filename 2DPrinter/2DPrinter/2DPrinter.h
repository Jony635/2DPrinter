// El siguiente bloque ifdef muestra la forma estándar de crear macros que facilitan 
// de un archivo .dll más sencillo. Todos los archivos de este archivo .dll se compilan con MY2DPRINTER_EXPORTS
// definido en la línea de comandos. Este símbolo no debe definirse en ningún proyecto
// que use este archivo DLL. De este modo, otros proyectos cuyos archivos de código fuente incluyan el archivo 
// interpretan que las funciones MY2DPRINTER_API se importan de un archivo .dll, mientras que este archivo .dll interpreta los símbolos
// definidos en esta macro como si fueran exportados.

#ifdef MY2DPRINTER_EXPORTS
#define MY2DPRINTER_API __declspec(dllexport)
#else
#define MY2DPRINTER_API __declspec(dllimport)
#endif

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/x86/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x86/SDL2main.lib")

#include "SDL/include/SDL_render.h"

#include <queue>

enum class MY2DPRINTER_API Pivots
{
	INVALID = -1,
	UPPER_LEFT,
	UPPER_CENTER,
	UPPER_RIGHT,
	CENTER_LEFT,
	CENTER,
	CENTER_RIGHT,
	LOWER_LEFT,
	LOWER_CENTER,
	LOWER_RIGHT,
	CUSTOM_PIVOT
};

class MY2DPRINTER_API DrawingElem
{
	friend class Compare;
	friend class _2DPrinter;
protected:
	enum class DElemType
	{
		NONE = -1,
		SPRITE,
		QUAD
	} type = DElemType::NONE;

public:

	DrawingElem(DElemType type) : type(type) {}
};

class MY2DPRINTER_API Sprite : public DrawingElem
{
	friend class Compare;
	friend class _2DPrinter;
private:
	SDL_Point pos = { 0,0 };
	SDL_Texture* texture = nullptr;
	SDL_Rect SquaretoBlit = {0,0,0,0};
	int layer = 0;
	SDL_Point pivot = { 0,0 };
	float angle = 0.0f;
	int distToFeet = 0;
	SDL_Color color = { 0,0,0,0 };
	SDL_Point offset = { 0,0 };
	SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
	float speed = 1.0f;

public:

	Sprite(SDL_Point& pos, SDL_Texture* texture, SDL_Rect SquaretoBlit, int layer, SDL_Point offset, SDL_Point pivot, float angle, SDL_Color color) : DrawingElem(DrawingElem::DElemType::SPRITE), pos(pos), texture(texture), SquaretoBlit(SquaretoBlit), distToFeet(distToFeet), layer(layer), offset(offset), pivot(pivot), angle(angle), color(color)
	{
		this->blendMode = blendMode;
		this->speed = speed;
	}
};

struct MY2DPRINTER_API Info
{
	SDL_Point position;
	SDL_Texture* textureToBlit;
	SDL_Rect SquareToBlit;
	int layer = 0;
	Pivots OFFSET_MODE = Pivots::UPPER_LEFT;
	SDL_Point customOffset = { 0,0 };
	Pivots PIVOT_MODE = Pivots::CENTER;
	SDL_Point customPivot = { 0,0 };
	float degAngle = 0;
	SDL_Color color = { 255,255,255,255 };
};

class MY2DPRINTER_API Compare
{
public:
	bool operator () (DrawingElem* first, DrawingElem* second);
};


class MY2DPRINTER_API _2DPrinter
{
public:
	
	_2DPrinter() {}
	bool Init(SDL_Renderer* renderer, SDL_Rect* camera);
	bool PrintQueue();
	bool CleanUp();

	//Note: Angle required is in degrees, in clockwise direction
	bool PrepareSprite(Info info);
	
	//WIP
	//bool PrintQuad(SDL_Rect rect, SDL_Color color, bool filled = false, bool use_camera = false);


private:
	bool Blit(DrawingElem* dElem);

private:
	std::priority_queue<DrawingElem*, std::vector<DrawingElem*>, Compare> DrawingQueue;
	SDL_Renderer* renderer = nullptr;
	SDL_Rect* camera = nullptr;
	
};

