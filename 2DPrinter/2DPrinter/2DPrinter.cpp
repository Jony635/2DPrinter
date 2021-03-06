#include "stdafx.h"
#include "2DPrinter.h"

MY2DPRINTER_API _2DPrinter::~_2DPrinter()
{
	CleanUp();
}

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
		DrawingElem* dElem = DrawingQueue.front();
		DrawingQueue.remove(dElem);
		delete dElem;
	}

	return true;
}

MY2DPRINTER_API bool Compare::operator()(DrawingElem* first, DrawingElem* second)
{
	{
		if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::SPRITE)
		{
			Sprite* first_Sprite = (Sprite*)first;
			Sprite* second_Sprite = (Sprite*)second;
			if (first_Sprite->layer != second_Sprite->layer)
				return first_Sprite->layer > second_Sprite->layer;
			else
				return first_Sprite->pos.y + first_Sprite->SquaretoBlit.h > second_Sprite->pos.y + second_Sprite->SquaretoBlit.h;
		}

		//WIP: Quads support

		//else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::QUAD)
		//{
		//	/*Quad* firstQuad = (Quad*)first;
		//	Quad* secondQuad = (Quad*)second;
		//	return firstQuad->rect.y + firstQuad->rect.h > secondQuad->rect.y + secondQuad->rect.h;*/
		//	return false;
		//}
		//else if (first->type == DrawingElem::DElemType::QUAD && second->type == DrawingElem::DElemType::SPRITE)
		//{
		//	return true;
		//}
		//else if (first->type == DrawingElem::DElemType::SPRITE && second->type == DrawingElem::DElemType::QUAD)
		//{
		//	return false;
		//}

	}
}

MY2DPRINTER_API bool _2DPrinter::PrepareSprite(Info info)
{
	if (!renderer || !camera)
		return false;

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
	DrawingQueue.PushOrderedElem(sprite);

	return true;
}

MY2DPRINTER_API bool _2DPrinter::PrintQueue()
{
	while (!DrawingQueue.empty())
	{
		DrawingElem* delem = DrawingQueue.front();

		if (renderer && camera)
		{
			switch (delem->type)
			{
				case DrawingElem::DElemType::SPRITE:
				{
					Sprite* sprite = (Sprite*)delem;
					SDL_SetTextureColorMod(sprite->texture, sprite->color.r, sprite->color.g, sprite->color.b);
					SDL_SetTextureAlphaMod(sprite->texture, sprite->color.a);
					Blit(sprite);
					SDL_SetTextureColorMod(sprite->texture, 255, 255, 255);
					SDL_SetTextureAlphaMod(sprite->texture, 255);
					break;
				}
			}
		}

		DrawingQueue.remove(delem);
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

MY2DPRINTER_API void CustomList::PushOrderedElem(DrawingElem* elemtoPush)
{
	std::list<DrawingElem*>::iterator it;
	Compare compare;
	for (it = DElemList.begin(); it != DElemList.end(); ++it)
	{
		if (compare(*it, elemtoPush))
		{
			DElemList.insert(it, elemtoPush);
			break;
		}
	}
}