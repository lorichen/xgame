//==========================================================================
/**
* @file	  : Radial.h
* @author : pk <pk163@163.com>
* created : 2008-4-11   21:25
* purpose : 射线枚举类，按射线行进的方向,依次返回路上的各个点
*/
//==========================================================================

#ifndef __RADIAL_H__
#define __RADIAL_H__

class Radial
{
	int mStepX,mStepY;	/// 步长，矢量
	int x,y;			/// 当前起始点
	int mDistX,mDistY;	/// 距离，标量
	int pk,c,d;
	int mLength;		/// 当前步数

public:
	Radial(int x1, int y1, int x2, int y2)
	{
		x = x1;
		y = y1;
		int dx = x2 - x1;
		int dy = y2 - y1;

		if (dx > 0)
			mStepX = 1;
		else if (dx < 0)
			mStepX = -1;

		if (dy > 0)
			mStepY = 1;
		else if (dy < 0)
			mStepY = -1;

		mDistX = dx * mStepX;
		mDistY = dy * mStepY;
		int twoDistX = 2 * mDistX;
		int twoDistY = 2 * mDistY;
		
		// 斜线靠近x轴
		if (mDistX >= mDistY)
		{
			c = twoDistY;
			d = twoDistY - twoDistX;
			pk = twoDistY - mDistX;
		}
		else // 斜线靠近y轴
		{
			c = twoDistX;
			d = twoDistX - twoDistY;
			pk = twoDistX - mDistY;
		}

		mLength = 0;
	}

	void move(long& lNowX, long& lNowY, int length)
	{
		while (length--)
			move(lNowX, lNowY);
	}

	void move(xs::Point& pt)
	{
		move(pt.x, pt.y);
	}

	void move(long& lNowX, long& lNowY)
	{
		mLength ++;

		// 垂直线
		if (mDistX == 0) 
		{
			y += mStepY;
			lNowX = x;
			lNowY = y;
			return;
		}
		
		// 水平线
		if (mDistY == 0) 
		{
			x += mStepX;
			lNowX = x;
			lNowY = y;
			return;
		}

		// 斜线(靠近x轴)
		if (mDistX >= mDistY)
		{
				x += mStepX;
				if (pk < 0)
				{
					pk += c;

					lNowX = x;
					lNowY = y;
					return;
				}
				else
				{
					pk += d;
					y += mStepY;

					lNowX = x;
					lNowY = y;
					return;
				}
		}
		else // 靠近y轴的斜线
		{

				y += mStepY;
				if (pk < 0)
				{
					pk += c;

					lNowX = x;
					lNowY = y;
					return;
				}
				else
				{
					pk += d;
					x += mStepX;

					lNowX = x;
					lNowY = y;
					return;
				}
		}
	}

	//  返回走过的长度
	int getLength() const			{ return mLength; }
	void setCurPos(int x_, int y_)	{ x = x_; y = y_; }
};

#endif // __RADIAL_H__