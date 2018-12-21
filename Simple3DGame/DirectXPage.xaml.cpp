//
// DirectXPage.xaml.cpp
// Implementation of the DirectXPage class.
//

#include "pch.h"
#include "DirectXPage.xaml.h"

using namespace Simple3DGame;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;


using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Store;

//using namespace Windows::Phone::UI::Input;

DirectXPage::DirectXPage():
	m_windowVisible(true),
	m_coreInput(nullptr)
{
	InitializeComponent();

	// Register event handlers for page lifecycle.
	CoreWindow^ window = Window::Current->CoreWindow;

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

	swapChainPanel->CompositionScaleChanged += 
		ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);

	swapChainPanel->SizeChanged +=
		ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

	
	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
	m_deviceResources->SetSwapChainPanel(swapChainPanel);

	Cities = ref new Platform::Collections::Vector<City^>();
	
	Players = ref new Platform::Collections::Vector<PlayerList^>();

	Paints = ref new Platform::Collections::Vector<PaintList^>();

	SetPaintList();
	// Register our SwapChainPanel to get independent input pointer events
	/*
	auto workItemHandler = ref new WorkItemHandler([this] (IAsyncAction ^)
	{
		// The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
		m_coreInput = swapChainPanel->CreateCoreIndependentInputSource(
			Windows::UI::Core::CoreInputDeviceTypes::Mouse |
			Windows::UI::Core::CoreInputDeviceTypes::Touch |
			Windows::UI::Core::CoreInputDeviceTypes::Pen
			);

		// Register for pointer events, which will be raised on the background thread.
		m_coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerPressed);
		m_coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerMoved);
		m_coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &DirectXPage::OnPointerReleased);

		// Begin processing input messages as they're delivered.
		m_coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	});
	*/
	// Run task on a dedicated high priority background thread.
	//m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);


	// Initialize the license info for use in the app that is uploaded to the Store.
	// uncomment for release
	//   licenseInformation = CurrentApp.LicenseInformation;

	// get current app object using the simulator interface

	


	bAppInTrialMode = false;

	

	/*
	auto licenseInformation = Windows::ApplicationModel::Store::CurrentApp::LicenseInformation;
	bAppInTrialMode = licenseInformation->IsTrial;
	if (bAppInTrialMode == true)
	{
		//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
	{
		//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	licenseInformation->LicenseChanged += ref new Windows::ApplicationModel::Store::LicenseChangedEventHandler( this, &DirectXPage::OnLicenseChanged);
	*/
	//if(licenseInformation->IsTrial==true)

	// Register for the license state change event.
	//licenseInformation.LicenseChanged += new LicenseChangedEventHandler(licenseChangedEventHandler);
	//bAppInTrialMode = true;

	//if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
	///{
	//	HardwareButtons::BackPressed +=
	//		ref new EventHandler<BackPressedEventArgs^>(this, &DirectXPage::OnHardwareBackButtonPressed);
	//}

	m_main = std::unique_ptr<Simple3DGameMain>(new Simple3DGameMain(m_deviceResources,this));

	m_main->m_sceneRenderer->GetStage()->bAppInTrialMode = bAppInTrialMode;

	if (m_main->m_sceneRenderer->GetStage()->game_settings.shader_mode == 1)
	{
		shader_type->Text = "High";
	}
	else
	{
		shader_type->Text = "Low";
	}
		
	if (m_main->m_sceneRenderer->GetStage()->game_settings.music == 1)
	{
		music_on->Text = "On";
	}
	else
	{
		music_on->Text = "Off";
	}

	if (m_main->m_sceneRenderer->GetStage()->game_settings.screen_controls == 1)
	{
		controls_on->Text = "Show";
	}
	else
	{
		controls_on->Text = "Hide";
	}

	m_main->StartRenderLoop();
	
}

void Simple3DGame::DirectXPage::PurchaseFull()
{

	create_task(CurrentApp::RequestAppPurchaseAsync(false)).then([this](task<Platform::String^> currentTask)
	{
		try
		{
			currentTask.get();
			auto licenseInformation = CurrentApp::LicenseInformation;
			if (licenseInformation->IsActive && !licenseInformation->IsTrial)
			{
				m_main->m_sceneRenderer->GetStage()->bResetLevelUI = true;
				//rootPage->NotifyUser("You successfully upgraded your app to the fully-licensed version.", NotifyType::StatusMessage);
			}
			else
			{
				//rootPage->NotifyUser("You still have a trial license for this app.", NotifyType::ErrorMessage);
			}

			bAppInTrialMode = licenseInformation->IsTrial;
			if (bAppInTrialMode == true)
			{
				//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
			}
			else
			{
				//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			}
		}
		catch (Platform::Exception^ exception)
		{
			//rootPage->NotifyUser("The upgrade transaction failed. You still have a trial license for this app.", NotifyType::ErrorMessage);
		}
	});
}

void Simple3DGame::DirectXPage::Full_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{


}


void DirectXPage::OnLicenseChanged()
{
	return;

	auto licenseInformation = Windows::ApplicationModel::Store::CurrentApp::LicenseInformation;

	bAppInTrialMode = licenseInformation->IsTrial;
	
	m_main->m_sceneRenderer->GetStage()->bAppInTrialMode = bAppInTrialMode;

	if (bAppInTrialMode == true)
	{
		//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
	{
		//full_game_button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
	
}


/*
void DirectXPage::OnHardwareBackButtonPressed(
	_In_ Platform::Object^ sender,
	_In_ BackPressedEventArgs^ args
	)
{
	//  0 - start
	//  1 - name choose
	//  2 - name entry
	//  3 - game screen

	if (ui_state == 0)
	{
		// The game is not currently in active play mode, so take the default behavior
		// for the hardware back button.
		args->Handled = false;
	}
	else
	{
		if (ui_state == 1)
		{
			ShowStartScreen();
		}
		else
		{
			if (ui_state == 2)
			{
				ShowStartScreen();
			}
			else
			{
				if (ui_state == 3)
				{
					SetPause();
				}
			}

		}
		//exit(0);
		// The game is currently in active play mode, so hitting the hardware back button
		// will cause the game to pause.
		//m_pausePressed = true;
		args->Handled = true;
	}

}
*/

void DirectXPage::SmallEverything() // for low rez phones
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{

		GameTitle->FontSize=39;
		button_right->Width = 60;
		button_right->Height = 60;
		button_left->Width = 60;
		button_left->Height = 60;
		button_grab->Height = 60;
		button_grab->Width = 60;

		right_buttons->Height = 130;
		right_buttons->Width = 120;

		button_respot->Width = 60;
		button_respot->Height = 80;

		button_up->Height = 160;
		button_up->Width = 60;

		button_down->Height = 60;
		button_down->Width = 60;

		left_buttons->Height = 280;
		left_buttons->Width = 60;
		
		//names_list->Height = 220;
			//button_respot->Height
	})
	);
}



void DirectXPage::SetScreenControls(int mode)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, mode]()
	{
		if (mode == 1)
		{
			left_buttons->Visibility = Windows::UI::Xaml::Visibility::Visible;
			right_buttons->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else
		{
			left_buttons->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			right_buttons->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
	})
		);
}


void DirectXPage::SetPaintList()
{
	int i;

	Paints = ref new Platform::Collections::Vector<PaintList^>();

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "Save&Play";
	new_paint->Index = -2;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "Clear";
	new_paint->Index = -1;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "melon";
	new_paint->Index = 0;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "pear";
	new_paint->Index = 1;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "heart";
	new_paint->Index = 2;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "banana";
	new_paint->Index = 3;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "ring";
	new_paint->Index = 4;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "coin";
	new_paint->Index = 5;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "rocket";
	new_paint->Index = 6;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "mush red";
	new_paint->Index = 7;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "mush green";
	new_paint->Index = 8;
	Paints->Append(new_paint);
	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "mush blue";
	new_paint->Index = 9;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "tree 1";
	new_paint->Index = 10;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "tree 2";
	new_paint->Index = 11;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "tree 3";
	new_paint->Index = 12;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "tree 4";
	new_paint->Index = 13;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "tropic plant";
	new_paint->Index = 14;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "fern";
	new_paint->Index = 15;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "grass";
	new_paint->Index = 16;
	Paints->Append(new_paint);


	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "hemi";
	new_paint->Index = 17;
	Paints->Append(new_paint);




	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "plane 1";
	new_paint->Index = 20;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "plane 2";
	new_paint->Index = 21;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "plane 3";
	new_paint->Index = 22;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "rock";
	new_paint->Index = 23;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "big rock";
	new_paint->Index = 24;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "huge rock";
	new_paint->Index = 25;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "arrow";
	new_paint->Index = 26;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "loop";
	new_paint->Index = 27;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "vert";
	new_paint->Index = 28;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "stump";
	new_paint->Index = 29;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p ball";
	new_paint->Index = 50;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p barrel";
	new_paint->Index = 51;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p crate";
	new_paint->Index = 52;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p boxsci";
	new_paint->Index = 53;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p plank";
	new_paint->Index = 54;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p oil";
	new_paint->Index = 55;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p column";
	new_paint->Index = 56;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "p cone";
	new_paint->Index = 57;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "dice";
	new_paint->Index = 58;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "skull";
	new_paint->Index = 59;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "cat";
	new_paint->Index = 60;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "duck";
	new_paint->Index = 61;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "log";
	new_paint->Index = 62;
	Paints->Append(new_paint);

	new_paint = ref new PaintList();
	new_paint->Image = "Assets/arow.png";
	new_paint->Name = "goat";
	new_paint->Index = 63;
	Paints->Append(new_paint);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{

		items_list->ItemsSource = Paints;
	})
		);
}


void DirectXPage::SetLevelList()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{

		MyList->ItemsSource = Cities;
	})
		);
}

void DirectXPage::ClearLevelList()
{
	Cities = ref new Platform::Collections::Vector<City^>();
}

void DirectXPage::AddLevelListItem(Platform::String^ Image, Platform::String^ Name, int Score, bool enabled, int _level_no)
{
	new_city = ref new City();
	new_city->Image = Image;

	if (bAppInTrialMode == true && _level_no > 3)
	{
		new_city->opaic = L"0.6";
		new_city->Image2 = L"Assets/FullGame.png";
	}
	else
	{
		if (enabled == false)
		{
			new_city->opaic = L"0.6";
			new_city->Image2 = L"Assets/Lock.png";
		}
		else
		{
			new_city->opaic = L"1.0";
			new_city->Image2 = L"";
		}
	}
	new_city->Name = Name;
	new_city->Score = Score;
	new_city->enabled = enabled;
	new_city->level_no = _level_no;

	if (Score == 0 && enabled == true)
	{
		new_city->new_visible = "Visible";
	}
	else
	{
		new_city->new_visible = "Collapsed";
	}

	Cities->Append(new_city);
}



void DirectXPage::SetEditMode()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		right_chooser->Visibility = Windows::UI::Xaml::Visibility::Visible;
		right_buttons->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		//names_list->ItemsSource = Players;
	}));
}





void DirectXPage::SetPlayerList()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		names_list->ItemsSource = Players;
	}));
}

void DirectXPage::ClearPlayerList()
{
	Players = ref new Platform::Collections::Vector<PlayerList^>();
}

void DirectXPage::AddPlayerListItem(int index, Platform::String^ Name, int _score, int _level)
{
	new_player = ref new PlayerList();
	new_player->Index = index;
	new_player->Name = Name;
	new_player->score = _score;
	new_player->current_level = _level;
	Players->Append(new_player);
}


/*
void DirectXPage::SetLevelList()
{
	return;
	level_info* p_level_data = m_main->m_sceneRenderer->GetStage()->GetLevelData();

	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, p_level_data]()
	{
		level_info* level_data = (level_info*)p_level_data;
		Cities->Clear();
		City^ new_city = ref new City();
		int i;
		char fname[40];

		for (i = 0; i < 30; i++)
		{
			sprintf(fname, "Assets/LevelMaps/%d/ss.png", i);

			std::string s_str = std::string(fname);
			std::wstring wid_str = std::wstring(s_str.begin(), s_str.end());
			const wchar_t* w_char = wid_str.c_str();

			new_city->Image = ref new Platform::String(w_char);

			new_city->Score = level_data[i].high_score;
			new_city->Name = "Level 1";

		}
		
		MyList->ItemsSource = Cities;

	})
		);

}
*/

void DirectXPage::SetTitle(Platform::String^ objective)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, objective]()
	{
		title->Text = objective;

	})
	);

	
}

void DirectXPage::SetScore(int points)
{
	static int score;

	if (points != score)
	{
		score = points;
	
		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this, points]()
		{
			static const int bufferLength = 20;
			static char16 wsbuffer[bufferLength];

			int length = swprintf_s(wsbuffer, bufferLength, L"%d", points);
			points_box->Text = ref new Platform::String(wsbuffer, length);

		})
		);
	}

}


void DirectXPage::SetTimeFloat(float time)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, time]()
	{
		static const int bufferLength = 20;
		static char16 wsbuffer[bufferLength];

		int mins = int(time) / 60;
		int secs = int(time) % 60;

		int length = swprintf_s(wsbuffer, bufferLength, L"%0.2d:%0.2d", mins, secs);
		time_text->Text = ref new Platform::String(wsbuffer, length);
	})
		);
}

void DirectXPage::SetCountDown(int time)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, time]()
	{
		static const int bufferLength = 20;
		static char16 wsbuffer[bufferLength];

		start_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);


		int length = swprintf_s(wsbuffer, bufferLength, L"%d", time);
		start_box->Text = ref new Platform::String(wsbuffer, length);
	})
	);
}

void DirectXPage::SetCountDownScore(int score,int col)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, score,col]()
	{
		static const int bufferLength = 20;
		static char16 wsbuffer[bufferLength];

		if (col == 0)
		{
			start_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Yellow);

		}

		if (col == 1)
		{
			//high_score->Visibility = Windows::UI::Xaml::Visibility::Visible;
			start_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::GreenYellow);
		}


		int length = swprintf_s(wsbuffer, bufferLength, L"%d", score);
		start_box->Text = ref new Platform::String(wsbuffer, length);
	})
		);
}


void DirectXPage::SetCountDown(Platform::String^ objective)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, objective]()
	{
		
		start_box->Text = objective;
		start_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);

	})
	);
}


void DirectXPage::SetCountParameters(float scale,float opaicity,bool b_visible)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this, scale,opaicity, b_visible]()
		{
			//start_box->RenderTransform->SetValue()
			auto scaleTransform = ref new Windows::UI::Xaml::Media::ScaleTransform();
			scaleTransform->ScaleX = scale;
			scaleTransform->ScaleY = scale;
			start_box->RenderTransform = scaleTransform;




			start_box->Opacity = opaicity;
			if (b_visible == true)
			{
				start_box->Visibility = Windows::UI::Xaml::Visibility::Visible;
			}
			else
			{
				start_box->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			}
		})
	);
}


void DirectXPage::SetRockets(int rockets)
{
	static int _rockets;

	if (rockets != _rockets)
	{
		_rockets = rockets;

		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this, rockets]()
		{
			static const int bufferLength = 20;
			static char16 wsbuffer[bufferLength];

			int length = swprintf_s(wsbuffer, bufferLength, L"%d", rockets);
			rockets_box->Text = ref new Platform::String(wsbuffer, length);

		})
		);
	}

}



void DirectXPage::SetRings(int rings_at,int rings_to)
{

		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this, rings_at, rings_to]()
		{
			static const int bufferLength = 20;
			static char16 wsbuffer[bufferLength];

			int length = swprintf_s(wsbuffer, bufferLength, L"%d/%d", rings_at , rings_to);
			rings_box->Text = ref new Platform::String(wsbuffer, length);

			if (rings_at == rings_to)
			{
				rings_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Green);
			}
			else
			{
				rings_box->Foreground = ref new SolidColorBrush(Windows::UI::Colors::Red);
			}

		})
		);

}


DirectXPage::~DirectXPage()
{
	// Stop rendering and processing events on destruction.
	m_main->StopRenderLoop();
	m_coreInput->Dispatcher->StopProcessEvents();
}

// Saves the current state of the app for suspend and terminate events.
void DirectXPage::SaveInternalState(IPropertySet^ state)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->Trim();

	// Stop rendering when the app is suspended.
	m_main->StopRenderLoop();

	// Put code to save app state here.
}

// Loads the current state of the app for resume events.
void DirectXPage::LoadInternalState(IPropertySet^ state)
{
	// Put code to load app state here.

	// Start rendering when the app is resumed.
	m_main->StartRenderLoop();
}

// Window event handlers.

void DirectXPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
	if (m_windowVisible)
	{
		m_main->StartRenderLoop();
	}
	else
	{
		m_main->StopRenderLoop();
	}
}

// DisplayInformation event handlers.

void DirectXPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetDpi(sender->LogicalDpi);
	m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	if (m_main->m_sceneRenderer->GetStage()->bAllLoaded == true && m_main->m_sceneRenderer->GetStage()->start_render_delay <=0.0f)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
		m_main->CreateWindowSizeDependentResources();
	}
}


void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->ValidateDevice();
}

// Called when the app bar button is clicked.
void DirectXPage::AppBarButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	// Use the app bar if it is appropriate for your app. Design the app bar, 
	// then fill in event handlers (like this one).
}

void DirectXPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
	// When the pointer is pressed begin tracking the pointer movement.
	m_main->StartTracking();
}

void DirectXPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
	// Update the pointer tracking code.
	if (m_main->IsTracking())
	{
		m_main->TrackingUpdate(e->CurrentPoint->Position.X);
	}
}

void DirectXPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
	// Stop tracking pointer movement when the pointer is released.
	m_main->StopTracking();
}

void DirectXPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
	m_main->CreateWindowSizeDependentResources();
}

void DirectXPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	critical_section::scoped_lock lock(m_main->GetCriticalSection());
	m_deviceResources->SetLogicalSize(e->NewSize);
	m_main->CreateWindowSizeDependentResources();
}





void Simple3DGame::DirectXPage::button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
}



void Simple3DGame::DirectXPage::button_up_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyUp(true);

	Windows::UI::Input::PointerPoint^ point = e->GetCurrentPoint((Windows::UI::Xaml::UIElement^)sender);
	m_main->m_sceneRenderer->SetSpeedPos(140.0f-point->RawPosition.Y);
//	float pointy = point->RawPosition.Y;
//	float pointx = point->RawPosition.X;

	button_up->Opacity = 0.7f;
}


void Simple3DGame::DirectXPage::button_up_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyUp(false);

	//Windows::UI::Input::PointerPoint^ point = e->GetCurrentPoint((Windows::UI::Xaml::UIElement^)sender);
	//m_main->m_sceneRenderer->SetSpeedPos(140.0f - point->RawPosition.Y);

	button_up->Opacity = 0.2f;
}

void Simple3DGame::DirectXPage::button_up_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	//Windows::UI::Input::PointerPoint^ point = e->GetCurrentPoint(sender);// Pointer;
	Windows::UI::Input::PointerPoint^ point = e->GetCurrentPoint((Windows::UI::Xaml::UIElement^)sender);
	m_main->m_sceneRenderer->SetSpeedPos(140.0f - point->RawPosition.Y);

}


void Simple3DGame::DirectXPage::button_down_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyDown(true);
	button_down->Opacity = 0.7f;
}


void Simple3DGame::DirectXPage::button_down_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyDown(false);
	button_down->Opacity = 0.2f;
}


void Simple3DGame::DirectXPage::button_left_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyLeft(true);
	button_left->Opacity = 0.7f;
}


void Simple3DGame::DirectXPage::button_left_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyLeft(false);
	button_left->Opacity = 0.2f;
}


void Simple3DGame::DirectXPage::button_right_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyRight(true);
	button_right->Opacity = 0.7f;
}


void Simple3DGame::DirectXPage::button_right_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->GetMoveLook()->KeyRight(false);
	button_right->Opacity = 0.2f;
}


void Simple3DGame::DirectXPage::button_menu_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	//m_main->
	SetPause();

}

void Simple3DGame::DirectXPage::HideHUD()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
}

void Simple3DGame::DirectXPage::ShowHUD()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Visible;
		game_title->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
}

void Simple3DGame::DirectXPage::ForcePause(bool pause_to)
{
	if (pause_to == true)
	{
		m_main->m_sceneRenderer->GetStage()->bPaused = true;

		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			pause_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
			game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			if (m_main->m_sceneRenderer->GetStage()->current_level > 0)
			{
				MyList->UpdateLayout();
				MyList->ScrollIntoView(MyList->Items->First());
				MyList->ScrollIntoView(MyList->Items->GetAt(m_main->m_sceneRenderer->GetStage()->current_level - 1));
			}
		}));
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->bPaused = false;
		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			MyList->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			game_hud->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}));
	}
}

void Simple3DGame::DirectXPage::SetPause()
{
	//m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(Menu)
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		if (m_main->m_sceneRenderer->GetStage()->bPaused == true && m_main->m_sceneRenderer->GetStage()->bLevelComplete==false)
		{
			m_main->m_sceneRenderer->GetStage()->bPaused = false;
			game_title->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			pause_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			game_hud->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}
		else
		{
			m_main->m_sceneRenderer->GetStage()->bPaused = true;
			game_title->Visibility = Windows::UI::Xaml::Visibility::Visible;
			pause_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
			game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

			const size_t cSize = strlen(m_main->m_sceneRenderer->GetStage()->current_playername) + 1;
			wchar_t* wc = new wchar_t[cSize];
			mbstowcs(wc, m_main->m_sceneRenderer->GetStage()->current_playername, cSize);

			pause_player_name->Text = ref new Platform::String(wc);

			pause_player_name->Text = "Player name: " + pause_player_name->Text;

			if (m_main->m_sceneRenderer->GetStage()->current_level > 0)
			{
				MyList->UpdateLayout();
				MyList->ScrollIntoView(MyList->Items->First());
				MyList->ScrollIntoView(MyList->Items->GetAt(m_main->m_sceneRenderer->GetStage()->current_level - 1));
			}
		}
	}));
}

void Simple3DGame::DirectXPage::ScrollToLevel(int l)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this,l]()
	{
		MyList->UpdateLayout();
		MyList->ScrollIntoView(MyList->Items->First());
		MyList->ScrollIntoView(MyList->Items->GetAt(l));

		//MyList->MakeVisible()
		//MyList->ItemsPanel->sc
	}));
}

void Simple3DGame::DirectXPage::button_respot_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->DoRespot();
}


void Simple3DGame::DirectXPage::button_grab_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Banana->GrabPressed();
	button_grab->Opacity = 0.7f;
}


void Simple3DGame::DirectXPage::button_grab_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Banana->Grabreleased();
	button_grab->Opacity = 0.2f;
}


void Simple3DGame::DirectXPage::button_ab_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	button_ab->Opacity = 0.7f;
	//m_main->m_sceneRenderer->GetStage()->m_Banana->ABPressed();
	m_main->GetMoveLook()->KeyFast(true);
}


void Simple3DGame::DirectXPage::button_ab_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	button_ab->Opacity = 0.2f;
	m_main->GetMoveLook()->KeyFast(false);
	//m_main->m_sceneRenderer->GetStage()->m_Banana->ABReleased();
}


void Simple3DGame::DirectXPage::button_level_back_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->ChangeLevel(-1);
}


void Simple3DGame::DirectXPage::button_level_for_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->ChangeLevel(+1);
}



void Simple3DGame::DirectXPage::MyList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

}



void Simple3DGame::DirectXPage::MyList_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::ListView^ view = (Windows::UI::Xaml::Controls::ListView^)sender;

	City^ city = (City^)view->SelectedItem;

	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	
	if(((bAppInTrialMode==false || city->level_no<4) && city->enabled==true ) || m_main->m_sceneRenderer->GetStage()->bDesignMode==true)
	{
		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			//m_main->m_sceneRenderer->GetStage()->bPaused = false;
			
		}));

		if (m_main->m_sceneRenderer->GetStage()->bDesignMode == true)
		{


			if(m_main->m_sceneRenderer->GetStage()->bSaveLandRay==true)
			{ 
				m_main->m_sceneRenderer->GetStage()->MakeRayMap();
			}

			m_main->m_sceneRenderer->GetStage()->SaveAll();
		}
		m_main->m_sceneRenderer->GetStage()->bPaused = false;
		m_main->m_sceneRenderer->GetStage()->ChangeLevelTo(city->level_no);
	}
	else
	{
		if (bAppInTrialMode == true && city->level_no > 3)
		{
			PurchaseFull();
		}
		// could play a sound
	}
}





void Simple3DGame::DirectXPage::ShowStartScreen()
{
	ui_state = 0;
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		game_title->Visibility = Windows::UI::Xaml::Visibility::Visible;
		start_screen->Visibility = Windows::UI::Xaml::Visibility::Visible;
		name_enter_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		choose_name_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		start_box_grid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		if (m_main->m_sceneRenderer->GetStage()->total_player_names == 0)
		{
			continue_button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			new_player_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
			new_player_button->Text = "New Game";
		}
		else
		{
			if (m_main->m_sceneRenderer->GetStage()->total_player_names < 15)
			{
				continue_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
				new_player_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
				new_player_button->Text = "New Player";
			}
			else
			{
				continue_button->Visibility = Windows::UI::Xaml::Visibility::Visible;
				new_player_button->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
				new_player_button->Text = "New Player";
			}
		}
	}));
}


void Simple3DGame::DirectXPage::ShowGameScreen()
{
	ui_state = 3;
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Visible;
		game_title->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		start_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		name_enter_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		choose_name_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		start_box_grid->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}));
}

void Simple3DGame::DirectXPage::ShowNameChoose()
{
	ui_state = 1;
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		start_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		name_enter_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		game_title->Visibility = Windows::UI::Xaml::Visibility::Visible;
		choose_name_screen->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}));
}

void Simple3DGame::DirectXPage::ShowNameEntry()
{
	ui_state = 2;
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		game_hud->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		start_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		game_title->Visibility = Windows::UI::Xaml::Visibility::Visible;
		name_enter_screen->Visibility = Windows::UI::Xaml::Visibility::Visible;
		choose_name_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		player_name->Text = "";
		player_name->Focus(Windows::UI::Xaml::FocusState::Keyboard);
	}));
}


void Simple3DGame::DirectXPage::SetFadeOp(float val)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this,val]()
	{
		fade_screen->Visibility = Windows::UI::Xaml::Visibility::Visible;
		fade_screen->Opacity = val;
	}));
}

void Simple3DGame::DirectXPage::StopFade()
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		fade_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
}


void Simple3DGame::DirectXPage::continue_button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	ShowNameChoose();
	//m_main->m_sceneRenderer->GetStage()->SetGameState(STATE_PLAYING);
}


void Simple3DGame::DirectXPage::new_player_button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	//m_main->m_sceneRenderer->GetStage()->m_audio.Start();
	ShowNameEntry();
}



void Simple3DGame::DirectXPage::Button_name_ok(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Platform::String^ name_str = player_name->Text;
	std::wstring fooW(name_str->Begin());
	std::string fooA(fooW.begin(), fooW.end());
	char charStr[30];
	
	strcpy(charStr, (char*)(fooA.c_str()));

	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);

	bool bValid = true;
	int i;

	for (i = 0; i < strlen(charStr); i++)
	{
		if ((charStr[i]>'9' || charStr[i]<'0') && (charStr[i]>'Z' || charStr[i]<'A') && (charStr[i]>'z' || charStr[i]<'a'))
		{
			charStr[i] = '_';
			//bValid = false;
			//textBlock1a->Visibility = Windows::UI::Xaml::Visibility::Visible;
			//textBlock1a->Text = "please use only numbers and letters";
		}
	}

	if (bValid == true)
	{
		if (m_main->m_sceneRenderer->GetStage()->CheckName((char *)charStr) == true)
		{
			textBlock1a->Visibility = Windows::UI::Xaml::Visibility::Visible;
			textBlock1a->Text = "name already in use";
		}
		else
		{
			m_main->m_sceneRenderer->GetStage()->AddName((char *)charStr);
			m_main->m_sceneRenderer->GetStage()->bPaused = true;
			m_main->m_sceneRenderer->GetStage()->SetGameStateTo(START_PLAYING,1.0f);
		}
	}
}

void Simple3DGame::DirectXPage::player_name_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	if (e->Key == Windows::System::VirtualKey::Enter)
	{
		Button_name_ok(sender, e);

	}
}



void Simple3DGame::DirectXPage::name_list_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (choose_name_screen->Visibility == Windows::UI::Xaml::Visibility::Visible)
	{
		Windows::UI::Xaml::Controls::ListView^ view = (Windows::UI::Xaml::Controls::ListView^)sender;

		PlayerList^ playr = (PlayerList^)view->SelectedItem;

		m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);

		m_main->m_sceneRenderer->GetStage()->SetPlayer(playr->Index);
		/*
		Dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			game_hud->Visibility = Windows::UI::Xaml::Visibility::Visible;
			start_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			name_enter_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			choose_name_screen->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}));*/
		m_main->m_sceneRenderer->GetStage()->SetGameStateTo(START_PLAYING, 1.0f);
	}

}





void Simple3DGame::DirectXPage::TextBlock_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	SetPause();
}


void Simple3DGame::DirectXPage::TextBlock_PointerEntered_1(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}));
}



void Simple3DGame::DirectXPage::exit_yes_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	/*
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	m_main->m_sceneRenderer->GetStage()->SetGameState(STATE_START);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
	*/
}


void Simple3DGame::DirectXPage::exit_no_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	/*
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
	*/
}


void Simple3DGame::DirectXPage::exit_yes_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	m_main->m_sceneRenderer->GetStage()->SetGameState(STATE_START);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
}


void Simple3DGame::DirectXPage::exit_no_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));
}


void Simple3DGame::DirectXPage::shader_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	if (m_main->m_sceneRenderer->GetStage()->game_settings.shader_mode == 1)
	{
		m_main->m_sceneRenderer->GetStage()->SetShaderMode(0);
		shader_type->Text = "Low";
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->SetShaderMode(1);
		shader_type->Text = "High";
	}
}


void Simple3DGame::DirectXPage::music_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	if (m_main->m_sceneRenderer->GetStage()->game_settings.music == 1)
	{
		m_main->m_sceneRenderer->GetStage()->SetMusicMode(0);
		music_on->Text = "Off";
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->SetMusicMode(1);
		music_on->Text = "On";
	}
}

void Simple3DGame::DirectXPage::controls_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	if (m_main->m_sceneRenderer->GetStage()->game_settings.screen_controls == 1)
	{
		m_main->m_sceneRenderer->GetStage()->SetScreenControls(0);
		controls_on->Text = "Hide";
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->SetScreenControls(1);
		controls_on->Text = "Show";
	}
}



void Simple3DGame::DirectXPage::ok_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	m_main->m_sceneRenderer->GetStage()->SaveSettings();
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		settings_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}));
}


void Simple3DGame::DirectXPage::TextBlock_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}


void Simple3DGame::DirectXPage::Options_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);

	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		settings_area->Visibility = Windows::UI::Xaml::Visibility::Visible;
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}));

}


void Simple3DGame::DirectXPage::items_list_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	Windows::UI::Xaml::Controls::ListView^ view = (Windows::UI::Xaml::Controls::ListView^)sender;

	PaintList^ paint = (PaintList^)view->SelectedItem;
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);

	if (paint->Index == -2)
	{
		m_main->m_sceneRenderer->GetStage()->SaveAll();
		m_main->m_sceneRenderer->GetStage()->bPaused = false;
		m_main->m_sceneRenderer->GetStage()->bDesignMode = false;

		right_chooser->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		right_buttons->Visibility = Windows::UI::Xaml::Visibility::Visible;

		m_main->m_sceneRenderer->GetStage()->m_Fruit->bDesignMode = false;
		m_main->m_sceneRenderer->GetStage()->m_Statics->bDesignPointer = false;
		m_main->m_sceneRenderer->GetStage()->m_Boxes->bDesignPointer = false;

		m_main->m_sceneRenderer->GetStage()->ChangeLevelTo(m_main->m_sceneRenderer->GetStage()->current_level);
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->SetDesignItem(paint->Index);
	}
}


void Simple3DGame::DirectXPage::items_list_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{

}


void Simple3DGame::DirectXPage::ray_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//m_main->m_sceneRenderer->GetStage()->bRayCheck = (bool)ray_check->IsChecked;
	/*x
	Platform::IBox<bool>^ iboxBool = ray_check->IsChecked;
	if (iboxBool->Value == true)
	{
		m_main->m_sceneRenderer->GetStage()->bRayCheck = true;
	}
	else
	{
		m_main->m_sceneRenderer->GetStage()->bRayCheck = false;
	}
	*/
	m_main->m_sceneRenderer->GetStage()->bRayCheck = true;
}


void Simple3DGame::DirectXPage::ray_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bRayCheck = false;
}


void Simple3DGame::DirectXPage::fix_height_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Statics->bIgnoreHeight = true;
	m_main->m_sceneRenderer->GetStage()->m_Boxes->bIgnoreHeight = true;
}


void Simple3DGame::DirectXPage::fix_height_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Statics->bIgnoreHeight = false;
	m_main->m_sceneRenderer->GetStage()->m_Boxes->bIgnoreHeight = false;
}


void Simple3DGame::DirectXPage::player_pos_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bPlayerPosition = true;
}


void Simple3DGame::DirectXPage::player_pos_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bPlayerPosition = false;
}


void Simple3DGame::DirectXPage::cam_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->SetFreeCamera(true);
}


void Simple3DGame::DirectXPage::cam_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->SetFreeCamera(false);
}


void Simple3DGame::DirectXPage::grid_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->SetSnapGrid(true);
}


void Simple3DGame::DirectXPage::grid_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->SetSnapGrid(false);
}


void Simple3DGame::DirectXPage::cont_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bContinualPlace = true;
}


void Simple3DGame::DirectXPage::cont_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bContinualPlace = false;
}


void Simple3DGame::DirectXPage::itempo_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Boxes->bHidePlacements = false;
}


void Simple3DGame::DirectXPage::itempo_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_Boxes->bHidePlacements = true;
}


void Simple3DGame::DirectXPage::rotate_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bRotateRay = true;
	m_main->m_sceneRenderer->GetStage()->m_Trees->bRotateRay = true;
}


void Simple3DGame::DirectXPage::rotate_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->bRotateRay = false;
	m_main->m_sceneRenderer->GetStage()->m_Trees->bRotateRay = false;
}




void Simple3DGame::DirectXPage::Continue_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	SetPause();
}


void Simple3DGame::DirectXPage::Exit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	m_main->m_sceneRenderer->GetStage()->m_audio.PlaySoundEffect(SFXMenuClick);
	Dispatcher->RunAsync(
		CoreDispatcherPriority::Normal,
		ref new DispatchedHandler([this]()
	{
		pause_area->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		quit_quest->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}));
}













void Simple3DGame::DirectXPage::MyList_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{

}
