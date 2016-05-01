#include <string>
#include <cornell.h>
#include "C_MainMenu.h"

using namespace cocos2d;
using namespace std;

#define LEVEL_ONE_KEY "level1"
#define LEVEL_ONE_FILE "levels/level1.shadl"

MainMenuButton* MainMenuButton::create(GameController* gc) {
	MainMenuButton* q = new (std::nothrow) MainMenuButton();
	if (q && q->init(gc)) {
		q->autorelease();
		return q;
	}
	CC_SAFE_DELETE(q);
	return nullptr;
}

bool MainMenuButton::init(GameController* gc) {
	ui::Button::init();
	if (gc) {
		_controller = gc;
		_controller->retain();
		return true;
	}
	return false;
}

void MainMenuButton::dispose() {
	_controller->release();
	_controller = nullptr;
}

MainMenuController::MainMenuController() :
	_rootnode(nullptr),
	_worldnode(nullptr),
	_active(false),
	_activeController(nullptr)
{
}

bool MainMenuController::init(RootLayer* root) {

	_rootnode = root;
	_rootnode->retain();

	// Determine the center of the screen
	Size dimen = _rootnode->getContentSize();
	Vec2 center(dimen.width / 2.0f, dimen.height / 2.0f);

	Texture2D * background = _assets->get<Texture2D>("background");
	Size bsize = background->getContentSize();

	// Create the scene graph
	_worldnode = Node::create();
	_backgroundnode = PolygonNode::createWithTexture(background);
	_backgroundnode->setScale(dimen.width / bsize.width, dimen.height / bsize.height);
	_backgroundnode->setPosition(center.x,center.y);
	_worldnode->addChild(_backgroundnode, 0);

	_rootnode->addChild(_worldnode, 0);

	//Add buttons
	Vec2 start = { 350,450 };
	int os = 170;
	int xbutt = 3;
	int ybutt = 3;

	int ypos = 0;
	for (int i = 0; i < ybutt; i++) {
		int xpos = 0;
		for (int j = 0; j < xbutt; j++) {
			if (i * xbutt + j < mainMenuButtons.size()) {
				MainMenuButton* button = mainMenuButtons[i*xbutt + j];
				Vec2 pos = { start.x + xpos, start.y - ypos };
				button->loadTextures("textures/Owen.png", "textures/Owen.png");
				button->setPosition(pos);
				button->setTouchEnabled(true);
				//Add event listener
				button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
					switch (type) {
					case ui::Widget::TouchEventType::ENDED:
						_activeController = ((MainMenuButton*)sender)->getController();
						_rootnode->removeAllChildren();
						_activeController->init(_rootnode);
					}
				});
				_rootnode->addChild(button, 1);

				button->retain();
			}
			xpos = xpos + os;
		}
		ypos = ypos + os;
	}

	_active = true;
	return true;
}

void MainMenuController::update(float dt) {
	if (_activeController != nullptr)
		_activeController->update(dt);
}

MainMenuController::~MainMenuController() {
	dispose();
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void MainMenuController::dispose() {
	_worldnode = nullptr;
	_rootnode->release();
	_rootnode = nullptr;
	_activeController = nullptr;
	for (MainMenuButton* b : mainMenuButtons) {
		b->dispose();
		b->release();
	}
}

void MainMenuController::preload() {
	// Load the textures (Autorelease objects)
	_assets = AssetManager::getInstance()->getCurrent();
	TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
	tloader->loadAsync(EXPOSURE_BAR, "textures/exposure_bar.png");
	tloader->loadAsync(EXPOSURE_FRAME, "textures/exposure_bar_frame.png");
	tloader->loadAsync(DUDE_TEXTURE, "textures/ShadeDude.png");
	tloader->loadAsync(PEDESTRIAN_TEXTURE, "textures/pedestrian_td.png");
	tloader->loadAsync(PEDESTRIAN_SHADOW_TEXTURE, "textures/pedestrian_s_td.png");
	tloader->loadAsync(CAR_TEXTURE, "textures/Car1.png");
	tloader->loadAsync(CAR_SHADOW_TEXTURE, "textures/Car1_S.png");
	tloader->loadAsync(GOAL_TEXTURE, "textures/owner.png");
	_assets->loadAsync<Sound>(GAME_MUSIC, "sounds/DD_Main.mp3");
	_assets->loadAsync<Sound>(WIN_MUSIC, "sounds/DD_Victory.mp3");
	_assets->loadAsync<Sound>(LOSE_MUSIC, "sounds/DD_Failure.mp3");
	_assets->loadAsync<TTFont>(MESSAGE_FONT, "fonts/RetroGame.ttf");

	JSONReader reader;
	reader.initWithFile(STATIC_OBJECTS);
	if (!reader.startJSON()) {
		CCASSERT(false, "Failed to load static objects");
		return;
	}
	int count = reader.startArray("types");
	for (int index = 0; index < count; index++) {
		reader.startObject();
		string name = reader.getString("name");
		string imageFormat = reader.getString("imageFormat");
		string shadowImageFormat = reader.getString("shadowImageFormat");
		if (shadowImageFormat == "") {
			shadowImageFormat = imageFormat;
		}
		tloader->loadAsync(name + OBJECT_TAG, "textures/static_objects/" + name + "." + imageFormat);
		tloader->loadAsync(name + SHADOW_TAG, "textures/static_objects/" + name + "_S." + shadowImageFormat);
		reader.endObject();
		reader.advance();
	}
	reader.endArray();
	reader.endJSON();

	// Background
	tloader->loadAsync("background", "textures/LevelChoice.png");
	// Button
	tloader->loadAsync("button", "textures/Owen.png");

	loadGameController(LEVEL_ONE_KEY, LEVEL_ONE_FILE);
}

/**
* Clear all memory when exiting.
*/
void MainMenuController::stop() {
}

/** Helper to load gameplay controllers in preload */
void MainMenuController::loadGameController(const char * levelkey, const char * levelpath) {
	GameController* gc = GameController::create(levelkey, levelpath);
	gc->preload();
	MainMenuButton* b = MainMenuButton::create(gc);
	mainMenuButtons.push_back(b);
	b->retain();
	int i = 0;
}
