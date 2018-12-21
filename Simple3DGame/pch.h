#pragma once


#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

#define XAUDIO2_HELPER_FUNCTIONS 1
#include <xaudio2.h>
#include <xaudio2fx.h>

#include <memory>
#include <agile.h>
#include <concrt.h>
#include <collection.h>
#include "App.xaml.h"

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>

#include <stdio.h>
#include <vector>
#include <memory>

#include <ppltasks.h>
#include <agile.h>
#include <concrt.h>

#include <DirectXCollision.h>

#include "../Bullet/src/btBulletDynamicsCommon.h"


#define M_PI 3.14159265358979323846

#define FAR_DIST 80.0f
#define FIN_DIST -10.0f

#define BIT(x) (1<<(x))
enum collisiontypes {
	COL_NOTHING = 0, //<Collide with nothing
	COL_RAY = 1, //<Collide with nothing
	COL_WORLDSTUFF = 4 + 1, //<Collide with ships
	COL_FLOORPLANE = 8, //<Collide with walls
	COL_CAR = 16, //<Collide with powerups

	COL_BOXES = 32 + 1, //<Collide with powerups
	COL_TREES = 64 + 1,
	COL_WALLS = 128,
	COL_CARBODY = 256 + 1
}; // +1 collide with ray cast
