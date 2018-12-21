//
// DirectXPage.xaml.h
// Declaration of the DirectXPage class.
//

#pragma once

#include "DirectXPage.g.h"

#include "Common\DeviceResources.h"
#include "Simple3DGameMain.h"

namespace Simple3DGame
{
	/// <summary>
	/// A page that hosts a DirectX SwapChainPanel.
	/// </summary>

	//public Person MyPerson{ get; private set; } =
	//	new Person{ FirstName = "Bob", LastName = "Jones" };

	[Windows::UI::Xaml::Data::Bindable]
	public ref class City sealed
	{
	public:
		property Platform::String^ Image;
		property Platform::String^ Image2;
		property Platform::String^ Name;
		property int Score;
		property bool enabled;
		property int level_no;
		property Platform::String^ opaic;
		property Platform::String^ new_visible;
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class PlayerList sealed
	{
	public:
		property int Index;
		property Platform::String^ Name;
		property int score;
		property int current_level;
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class PaintList sealed
	{
	public:
		property int Index;
		property Platform::String^ Name;
		property Platform::String^ Image;
	};


	public ref class DirectXPage sealed : Simple3DGame::IGameUIControl
	{
	public:
		DirectXPage();
		virtual ~DirectXPage();

		void SetPaintList();

		void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
		void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);

		virtual void SetTitle(Platform::String^ objective);
		virtual void SetScore(int points);
		virtual void SetRockets(int rockets);
		virtual void SetRings(int rings_at, int rings_to);
		virtual void SetPause();
		virtual void ForcePause(bool pause_to);
		virtual void SetTimeFloat(float time);
		virtual void SetCountDown(int time);
		virtual void SetCountDown(Platform::String^ objective);
		virtual void SetCountDownScore(int score, int col);
		virtual void SetCountParameters(float scale, float opaicity, bool b_visible);
		virtual void SetLevelList();
		virtual void AddLevelListItem(Platform::String^ Image, Platform::String^ Name, int Score, bool enabled,int _level_no);
		virtual void ClearLevelList();
		virtual void ScrollToLevel(int l);

		virtual void ShowStartScreen();
		virtual void ShowGameScreen();
		virtual void ShowNameChoose();
		virtual void ShowNameEntry();

		virtual void SetPlayerList();
		virtual void AddPlayerListItem(int index, Platform::String^ Name, int _score, int _level);
		virtual void ClearPlayerList();

		virtual void SetFadeOp(float val);
		virtual void StopFade();

		virtual void HideHUD();
		virtual void ShowHUD();

		virtual void SetEditMode();

		virtual void SmallEverything();

		virtual void SetScreenControls(int mode);


		void PurchaseFull();

	private:

		int ui_state;

		bool bAppInTrialMode;

		

		Windows::Foundation::Collections::IVector<City^>^ Cities;
		City^ new_city;// = ref new City();

		Windows::Foundation::Collections::IVector<PlayerList^>^ Players;
		PlayerList^ new_player;// = ref new City();

		Windows::Foundation::Collections::IVector<PaintList^>^ Paints;
		PaintList^ new_paint;// = ref new City();

		// XAML low-level rendering event handler.
		void OnRendering(Platform::Object^ sender, Platform::Object^ args);

		void OnLicenseChanged();

		// Window event handlers.
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);

		// DisplayInformation event handlers.
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

		// Other event handlers.
		void AppBarButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Object^ args);
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);



		// Track our independent input on a background worker thread.
		Windows::Foundation::IAsyncAction^ m_inputLoopWorker;
		Windows::UI::Core::CoreIndependentInputSource^ m_coreInput;

		// Independent input handling functions.
		void OnPointerPressed(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void OnPointerReleased(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);


		// Hardware back button is only present on some device families such as Phone.
		//void OnHardwareBackButtonPressed(
		///	_In_ Platform::Object^ sender,
		//	_In_ Windows::Phone::UI::Input::BackPressedEventArgs^ args
		//	);

		// Resources used to render the DirectX content in the XAML page background.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<Simple3DGameMain> m_main; 
		bool m_windowVisible;
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void image_Copy_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void image_Copy_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void image_Copy_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_up_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_up_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_down_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_down_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_left_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_left_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_right_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_right_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_menu_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_respot_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_grab_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_grab_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_ab_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_ab_PointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_level_back_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void button_level_for_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void MyList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void MyList_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e);
		void continue_button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void new_player_button_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void name_list_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void Button_name_ok(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void player_name_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void TextBlock_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void TextBlock_PointerEntered_1(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void exit_yes_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void exit_no_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void exit_yes_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void exit_no_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void exit_yes_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void exit_no_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void shader_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void ok_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void TextBlock_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Options_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void items_list_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void items_list_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void ray_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ray_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void fix_height_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void fix_height_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void player_pos_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void player_pos_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cam_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cam_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void grid_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void grid_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cont_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void cont_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void itempo_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void itempo_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void rotate_check_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void rotate_check_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TextBlock_PointerEntered(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Continue_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Exit_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void button_up_PointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void Full_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void music_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void controls_tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void MyList_DoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
};
}

