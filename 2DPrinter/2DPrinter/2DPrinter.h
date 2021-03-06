#ifdef MY2DPRINTER_EXPORTS
#define MY2DPRINTER_API __declspec(dllexport)
#else
#define MY2DPRINTER_API __declspec(dllimport)
#endif

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/x86/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x86/SDL2main.lib")

#include "SDL/include/SDL_render.h"
#include <list>

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

/*
*	\brief: The father class. In the future more things will be 
		    available to print, not only sprites. (geometrical forms, etc).
*/
class MY2DPRINTER_API DrawingElem
{
	friend class Compare;
	friend class _2DPrinter;
protected:
	enum class DElemType
	{
		NONE = -1,
		SPRITE,
		GEO_FORM
	} type = DElemType::NONE;

	SDL_Point pos = { 0,0 };
	SDL_Texture* texture = nullptr;
	int layer = 0;
	SDL_Point pivot = { 0,0 };
	float angle = 0.0f;
	SDL_Color color = { 0,0,0,0 };
	SDL_Point offset = { 0,0 };
	SDL_Rect SquaretoBlit = { 0,0,0,0 };

public:

	DrawingElem(DElemType type, SDL_Point pos, SDL_Texture* texture, int layer, SDL_Point offset, SDL_Point pivot, float angle, SDL_Color color, SDL_Rect SquaretoBlit) : type(type), pos(pos), texture(texture), layer(layer), offset(offset), pivot(pivot), angle(angle), color(color), SquaretoBlit(SquaretoBlit)
	{}
};

/*
*	\brief: The sprite class.
*/
class MY2DPRINTER_API Sprite : public DrawingElem
{
	friend class Compare;
	friend class _2DPrinter;

public:

	Sprite(SDL_Point& pos, SDL_Texture* texture, SDL_Rect SquaretoBlit, int layer, SDL_Point offset, SDL_Point pivot, float angle, SDL_Color color) : DrawingElem(DrawingElem::DElemType::SPRITE, pos, texture, layer, offset, pivot, angle, color, SquaretoBlit)
	{}
};

/*
*	\brief: Contains all relevant data about a sprite.
*/
struct MY2DPRINTER_API Info
{
	//* x,y drawing coordinates, camera dependent. (The real pos will be camera.x + position.x)
	SDL_Point position;
	SDL_Texture* textureToBlit;

	//* The section of the texture to blit on screen.
	//  if w or h are 0, the whole texture is printed.
	//  {0,0,0,0} by default.
	SDL_Rect SquareToBlit;

	//* The layer where the sprite will be printed on. 
	//  Higher layers will be printed after the lower ones.
	//  If some sprites are in the same layer, they will be ordered depending of their lowest pixels. The one which has the lowest pixels will be printed after the others.
	int layer = 0;
	
	//* Choose between different predefined offset-modes or a custom one. By default it is set in the Upper-Left corner of the sprite.
	//* The real position where the sprite will be printed in is position - offset.
	Pivots OFFSET_MODE = Pivots::UPPER_LEFT;

	//* Set your own custom offset. It only works if you set up the OFFSET_MODE as CUSTOM_PIVOT.
	//* The x,y coordinates have to be the amount of pixels from the UPPER-LEFT corner to the x,y coord of the offset.
	SDL_Point customOffset = { 0,0 };

	//* Choose between different predefined pivot-modes or a custom one. By default it is set in the Center of the sprite.
	//* The pivot is the point which the sprite will be rotated around.
	Pivots PIVOT_MODE = Pivots::CENTER;

	//* Set your own custom pivot. It only works if you set up the PIVOT_MODE as CUSTOM_PIVOT.
	//* The x,y coordinates have to be the amount of pixels from the UPPER-LEFT corner to the x,y coord of the pivot.
	SDL_Point customPivot = { 0,0 };

	//* The amount of angles (in degrees) the sprite will be rotated. In the clockwise direction, due to SDL system.
	float degAngle = 0;

	//* The RGB color used to tint the sprite. The alfa value will control the transparency of the sprite, not the tint.
	SDL_Color color = { 255,255,255,255 };
};

/*
*	\brief: The comparison class.
*/
class MY2DPRINTER_API Compare
{
public:
	bool operator () (DrawingElem* first, DrawingElem* second);
};

class MY2DPRINTER_API CustomList
{
	std::list<DrawingElem*> DElemList;

public:
	void PushOrderedElem(DrawingElem* elemtoPush);
	bool empty()
	{
		return DElemList.empty();
	}
	DrawingElem* front()
	{
		return DElemList.front();
	}
	void remove(DrawingElem* elemtoRemove)
	{
		DElemList.remove(elemtoRemove);
	}
};

/*
*	\brief: The printer itself. You must create and destroy it dynamically, and call its methods properly.
*/
class MY2DPRINTER_API _2DPrinter
{
public:
	_2DPrinter() {}
	~_2DPrinter();

	/*
	*   \brief: Reset the queue and set everything ready.
	*   \note: Requires a SDL_Renderer* and a SDL_Rect* to work.
	*/
	bool Init(SDL_Renderer* renderer, SDL_Rect* camera);

	/*
	*   \brief: This method prints everything prepared in queue and erases all memory safely.
	*   \note:  You may want to call this once every frame.
	*/
	bool PrintQueue();

	/*
	*   \brief: Cleans safely the queue. Its called automatically when the printer object is deleted.
	*/
	bool CleanUp();

	/*
	*   \brief:	Add a sprite into the queue.
	*   \note:	Requires a filled struct with all the config set.
	*/
	bool PrepareSprite(Info info);
	
	//Not implemented yet
	bool PrintQuad(SDL_Rect rect, SDL_Color color, bool filled = false, bool use_camera = false) { return true; }

private:
	bool Blit(DrawingElem* dElem);

private:
	CustomList DrawingQueue;
	
	SDL_Renderer* renderer = nullptr;
	SDL_Rect* camera = nullptr;
};

typedef _2DPrinter Printer2D;

