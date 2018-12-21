#include "pch.h"
#include "TouchClass.h"


using namespace DirectXApp1;
using namespace DirectX;
using namespace DX;


TouchClass::TouchClass()
{
	int i;
	bReleased = false;

	for (i = 0; i<10; i++)
	{
		touch_points[i].bActive = false;
		touch_points[i].id = -10;
	}
	touches_down = 0;

	last_down = 100.0f;
	last_jump = 0.0f;
	bJump = false;
}

TouchClass::~TouchClass()
{

}

void TouchClass::AddTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds)
{
	bool bfound = false;

	if (last_down<20.0f)
	{
		if (last_jump <= 1.0f)
		{
			bJump = true;
			last_jump = 50.0f;

		}
	}
	last_down = 0.0f;

	for (int i = 0; i<10; i++)
	{
		if (bfound == false)
		{
			if (touch_points[i].bActive == false)
			{
				bfound = true;
				touch_points[i].bActive = true;
				touch_points[i].id = id;
				touch_points[i].original_position = DX::PointToTouch(point, bounds);
				touch_points[i].current_position = touch_points[i].original_position;
				touch_points[i].touch_number = touches_down++;
				break;
			}
		}
	}

}

void TouchClass::Update(float timeDelta)
{
	last_down += 1.0f;
	if (last_jump>0.0f)
		last_jump -= 1.0f;

}

void TouchClass::MoveTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds)
{
	for (int i = 0; i<10; i++)
	{
		if (touch_points[i].id == id && touch_points[i].bActive == true)
		{
			touch_points[i].current_position = PointToTouch(point, bounds);

			//char str[40];
			//sprintf_s(str , "%f  %f\n" , touch_points[i].current_position.x , touch_points[i].current_position.y );

			//OutputDebugStringA(str);

		}
	}
}

void TouchClass::RemoveTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds)
{
	for (int i = 0; i<10; i++)
	{
		if (touch_points[i].id == id)
		{
			touch_points[i].bActive = false;

			if (true) // touch_points[i].current_position.x==0.0f && touch_points[i].current_position.y==0.0f )
			{
				bReleased = true;

				hitDirection.x = touch_points[i].current_position.x - touch_points[i].original_position.x;
				hitDirection.y = touch_points[i].current_position.y - touch_points[i].original_position.y;
			}
		}
	}
}

DirectX::XMFLOAT2* TouchClass::GetSwipeVector()
{
	int i;
	int lowest = 999;
	int low_index = -1;
	static DirectX::XMFLOAT2 ret_vec;

	for (i = 0; i<10; i++)
	{
		if (touch_points[i].bActive == true)
		{
			if (touch_points[i].touch_number<lowest)
			{
				low_index = i;
				lowest = touch_points[i].touch_number;
			}
		}
	}

	if (low_index != -1)
	{
		ret_vec.x = touch_points[low_index].current_position.x - touch_points[low_index].original_position.x;
		ret_vec.y = touch_points[low_index].current_position.y - touch_points[low_index].original_position.y;

		btVector3 current(touch_points[low_index].current_position.x, touch_points[low_index].current_position.y, 0.0f);
		btVector3 original(touch_points[low_index].original_position.x, touch_points[low_index].original_position.y, 0.0f);
		btVector3 new_current = original - current;
		new_current.normalize();
		new_current *= 0.08f;

		btScalar dist = current.distance(original);

		if (dist > 0.08f)
		{
			current = current + new_current;
			touch_points[low_index].original_position.x = current.getX();
			touch_points[low_index].original_position.y = current.getY();
			ret_vec.x = touch_points[low_index].current_position.x - touch_points[low_index].original_position.x;
			ret_vec.y = touch_points[low_index].current_position.y - touch_points[low_index].original_position.y;
		}

		/*
		if( ret_vec.x>0.6 )
		{
		touch_points[low_index].original_position.x = touch_points[low_index].current_position.x - 0.01;
		}
		if( ret_vec.x<-0.6 )
		{
		touch_points[low_index].original_position.x = touch_points[low_index].current_position.x + 0.01;
		}

		if( ret_vec.y>0.6 )
		{
		touch_points[low_index].original_position.y = touch_points[low_index].current_position.y - 0.01;
		}
		if( ret_vec.y<-0.6 )
		{
		touch_points[low_index].original_position.y = touch_points[low_index].current_position.y + 0.01;
		}
		*/

		//touch_points[low_index].original_position.y = touch_points[low_index].current_position.y;


	}
	else
	{
		ret_vec.x = 0.0f;
		ret_vec.y = 0.0f;
	}

	return &ret_vec;
}