// $nombredeproyecto$.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"
#include "2DPrinter.h"

MY2DPRINTER_API bool _2DPrinter::Init(SDL_Renderer* renderer, SDL_Rect* camera)
{
	CleanUp();

	if (!renderer || !camera)
		return false;

	this->renderer = renderer;
	this->camera = camera;

	return true;
}

MY2DPRINTER_API bool _2DPrinter::CleanUp()
{
	if (DrawingQueue.empty())
		return true;

	while (!DrawingQueue.empty())
	{
		DrawingElem* dElem = DrawingQueue.top();
		DrawingQueue.pop();
		delete dElem;
	}

	return true;
}

MY2DPRINTER_API bool _2DPrinter::PrepareSprite(Info info)
{
	SDL_Point offset = info.customOffset;
	SDL_Point pivot = info.customPivot;
	SDL_Rect SquaretoBlit = info.SquareToBlit;
	if (SquaretoBlit.h == 0 || SquaretoBlit.w == 0)
	{
		SDL_QueryTexture(info.textureToBlit, 0, 0, &SquaretoBlit.w, &SquaretoBlit.h);
		info.SquareToBlit.w = SquaretoBlit.w;
		info.SquareToBlit.h = SquaretoBlit.h;
	}


	switch (info.OFFSET_MODE)
	{
		case Pivots::UPPER_LEFT:
		{
			offset = { 0,0 };
			break;
		}
		case Pivots::UPPER_CENTER:
		{
			offset = { SquaretoBlit.w / 2, 0 };
			break;
		}
		case Pivots::UPPER_RIGHT:
		{
			offset = { SquaretoBlit.w, 0 };
			break;
		}
		case Pivots::CENTER_LEFT:
		{
			offset = { 0, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::CENTER:
		{
			offset = { SquaretoBlit.w / 2, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::CENTER_RIGHT:
		{
			offset = { SquaretoBlit.w, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::LOWER_LEFT:
		{
			offset = { 0, SquaretoBlit.h };
			break;
		}
		case Pivots::LOWER_CENTER:
		{
			offset = { SquaretoBlit.w / 2, SquaretoBlit.h };
			break;
		}
		case Pivots::LOWER_RIGHT:
		{
			offset = { SquaretoBlit.w, SquaretoBlit.h };
			break;
		}
		case Pivots::CUSTOM_PIVOT:
		{
			offset = info.customOffset;
			break;
		}
	}

	switch (info.PIVOT_MODE)
	{
		case Pivots::UPPER_LEFT:
		{
			pivot = { 0,0 };
			break;
		}
		case Pivots::UPPER_CENTER:
		{
			pivot = { SquaretoBlit.w / 2, 0 };
			break;
		}
		case Pivots::UPPER_RIGHT:
		{
			pivot = { SquaretoBlit.w, 0 };
			break;
		}
		case Pivots::CENTER_LEFT:
		{
			pivot = { 0, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::CENTER:
		{
			pivot = { SquaretoBlit.w / 2, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::CENTER_RIGHT:
		{
			pivot = { SquaretoBlit.w, SquaretoBlit.h / 2 };
			break;
		}
		case Pivots::LOWER_LEFT:
		{
			pivot = { 0, SquaretoBlit.h };
			break;
		}
		case Pivots::LOWER_CENTER:
		{
			pivot = { SquaretoBlit.w / 2, SquaretoBlit.h };
			break;
		}
		case Pivots::LOWER_RIGHT:
		{
			pivot = { SquaretoBlit.w, SquaretoBlit.h };
			break;
		}
		case Pivots::CUSTOM_PIVOT:
		{
			pivot = info.customPivot;
			break;
		}
	}

	Sprite* sprite = new Sprite(info.position, info.textureToBlit, SquaretoBlit, info.layer, offset, pivot, info.degAngle, info.color);
	DrawingQueue.push(sprite);

	return true;
}

MY2DPRINTER_API bool _2DPrinter::PrintQueue()
{
	while (!DrawingQueue.empty())
	{
		DrawingElem* delem = DrawingQueue.top();
		switch (delem->type)
		{
			case DrawingElem::DElemType::SPRITE:
			{
				Sprite* sprite = (Sprite*)delem;
				SDL_SetTextureColorMod(sprite->texture, sprite->color.r, sprite->color.g, sprite->color.b);
				SDL_SetTextureAlphaMod(sprite->texture, sprite->color.a);
				Blit(sprite);
				SDL_SetTextureColorMod(sprite->texture, 255, 255, 255);
				break;
			}
			case DrawingElem::DElemType::QUAD:
			{
				/*Quad* quad = (Quad*)delem;
				App->render->DrawQuad(quad->rect, quad->color.r, quad->color.g, quad->color.b, quad->color.a, quad->filled, quad->use_camera);*/
				break;
			}
		}

		DrawingQueue.pop();
		delete delem;
	}
	return true;

}

MY2DPRINTER_API bool _2DPrinter::Blit(DrawingElem* dElem)
{
	bool ret = true;
	int speed = 1;
	int scale = 1;
	SDL_Rect rect;

	if (dElem->type != DrawingElem::DElemType::SPRITE)
	{
		return false;
	}

	Sprite* sElem = (Sprite*)dElem;

	rect.x = (int)(camera->x * speed) + sElem->pos.x * scale;
	rect.y = (int)(camera->y * speed) + sElem->pos.y * scale;
	
	if (sElem->SquaretoBlit.h != 0 && sElem->SquaretoBlit.w != 0)
	{
		rect.w = sElem->SquaretoBlit.w;
		rect.h = sElem->SquaretoBlit.h;
	}
	else
	{
		SDL_QueryTexture((SDL_Texture*)sElem->texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (sElem->pivot.x != INT_MAX && sElem->pivot.y != INT_MAX)
	{
		pivot.x = sElem->pivot.x;
		pivot.y = sElem->pivot.y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, (SDL_Texture*)sElem->texture, &sElem->SquaretoBlit, &rect, sElem->angle, p, SDL_FLIP_NONE) != 0)
	{
		ret = false;
	}
	return ret;
}


// Constructor de clase exportada.
// Consulte 2DPrinter.h para obtener información sobre la definición de la clase
