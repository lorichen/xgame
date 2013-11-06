#include "stdafx.h"
#include "EnumTileEntities.h"

void EnumRect::EnumTileArea(const RECT& rcTileArea)
{
	int nTileRow = rcTileArea.bottom;
	int nTileCol = rcTileArea.right*2;
	for (int row=0; row<nTileRow; row++)
	{
		POINT ptCurTile;
		ptCurTile.x = rcTileArea.left + row;
		ptCurTile.y = rcTileArea.top - row;
		for (int col=0; col<nTileCol; col++)
		{
			if (retTrue != OnFoundTile(ptCurTile.x, ptCurTile.y))
				return;	

			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}
}