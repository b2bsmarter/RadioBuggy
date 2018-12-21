#pragma once

namespace DirectXApp1
{

	class TouchClass
	{
	public:
		TouchClass();
		~TouchClass();

		int total_points;
		int current_point;

		float last_down;
		bool bJump;
		float last_jump;

		int touches_down;

		typedef struct
		{
			bool bActive;
			int id;

			int touch_number;

			DirectX::XMFLOAT2 original_position;
			DirectX::XMFLOAT2 current_position;
		} touch_point;

		DirectX::XMFLOAT2 hitDirection;
		bool bReleased;

		touch_point touch_points[20];

		void AddTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds);
		void MoveTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds);
		void RemoveTouchPoint(int id, Windows::Foundation::Point point, Windows::Foundation::Size bounds);

		void Update(float timeDelta);

		DirectX::XMFLOAT2* GetSwipeVector();

		DirectX::XMFLOAT2 GetDifference(int id)
		{
			DirectX::XMFLOAT2 new_position;
			new_position.x = touch_points[id].current_position.x - touch_points[id].original_position.x;
			new_position.y = touch_points[id].current_position.y - touch_points[id].original_position.y;

			return new_position;
		}

		DirectX::XMFLOAT2 GetFirstDifference()
		{
			bool bFound = false;
			DirectX::XMFLOAT2 zero_move = DirectX::XMFLOAT2(0.0f, 0.0f);

			for (int i = 0; i < 10; i++)
			{
				if (bFound == false)
				{
					if (touch_points[i].bActive == true)
					{
						bFound = true;

						zero_move.x = touch_points[i].current_position.x - touch_points[i].original_position.x;
						zero_move.y = touch_points[i].current_position.y - touch_points[i].original_position.y;
					}
				}
			}
			return zero_move;
		}

	};

}