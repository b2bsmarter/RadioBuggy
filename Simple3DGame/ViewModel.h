#pragma once
namespace DirectXApp1
{
	ref class ViewModel sealed
	{
	private:
		int m_scoreUser;
		int m_scoreMachine;
		wchar_t TimeStr;
		int time_seconds;


	public:
		ViewModel() {};
		event Windows::Foundation::TypedEventHandler<Object^, Platform::String^>^ PropertyChanged;


		property int time_value
		{
			int get()
			{
				return time_seconds;
			}

			void set(int value)
			{
				if (time_seconds != value)
				{
					time_seconds = value;
					PropertyChanged(this, L"time_value");
				}
			}
		};

		property int ScoreUser
		{
			int get()
			{
				return m_scoreUser;
			}

			void set(int value)
			{
				if (m_scoreUser != value)
				{
					m_scoreUser = value;
					PropertyChanged(this, L"ScoreUser");
				}
			}
		};


		property int ScoreMachine
		{
			int get()
			{
				return m_scoreMachine;
			}

			void set(int value)
			{
				if (m_scoreMachine != value)
				{
					m_scoreMachine = value;

					PropertyChanged(this, L"ScoreMachine");
				}
			}
		};
	};

}



