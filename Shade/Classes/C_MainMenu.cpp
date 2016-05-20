#include <string>
#include <cornell.h>
#include "C_MainMenu.h"

using namespace cocos2d;
using namespace std;

#define TUTORIAL_KEY "tutorial"
#define TUTORIAL_FILE "levels/tutorial.shadl"
#define LEVEL_ONE_KEY "level1"
#define LEVEL_ONE_FILE "levels/level1.shadl"
#define LEVEL_TWO_KEY "level2"
#define LEVEL_TWO_FILE "levels/level2.shadl"
#define LEVEL_THREE_KEY "level3"
#define LEVEL_THREE_FILE "levels/level3.shadl"
#define LEVEL_FOUR_KEY "level4"
#define LEVEL_FOUR_FILE "levels/level4.shadl"
#define LEVEL_FIVE_KEY "level5"
#define LEVEL_FIVE_FILE "levels/level5.shadl"
#define LEVEL_SIX_KEY "level6"
#define LEVEL_SIX_FILE "levels/level6.shadl"
#define LEVEL_SEVEN_KEY "level7"
#define LEVEL_SEVEN_FILE "levels/level7.shadl"
#define LEVEL_EIGHT_KEY "level8"
#define LEVEL_EIGHT_FILE "levels/level8.shadl"
#define LEVEL_NINE_KEY "level9"
#define LEVEL_NINE_FILE "levels/level9.shadl"
#define MENU_BACKGROUND_KEY "mbackground"
#define TUTORIAL_BUTTON "tutbutt"

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
	_rootnode->setColor(Color3B::BLACK);

	// Determine the center of the screen
	Size dimen = _rootnode->getContentSize();
	Vec2 center(dimen.width / 2.0f, dimen.height / 2.0f);

	Texture2D * background = _assets->get<Texture2D>(MENU_BACKGROUND_KEY);
	Size bsize = background->getContentSize();

	// Create the scene graph
	_worldnode = Node::create();
	_backgroundnode = PolygonNode::createWithTexture(background);
	_backgroundnode->setScale(dimen.width / bsize.width, dimen.height / bsize.height);
	_backgroundnode->setPosition(center.x,center.y);
	_worldnode->addChild(_backgroundnode, 0);
	_worldnode->retain();
	_rootnode->addChild(_worldnode, 0);

	// Tutorial button
	_tutButt->setScale(_backgroundnode->getScaleX() * 0.5f, _backgroundnode->getScaleY() * 0.5f);
	_tutButt->setPosition(Vec2((_rootnode->getContentSize().width / 2.0f), (_rootnode->getContentSize().height * 0.93f)));
	_worldnode->addChild(_tutButt, 1);

	//Add buttons
	int xbutt = 3;
	int ybutt = 3;

	for (int i = 0; i < ybutt; i++) {
		for (int j = 0; j < xbutt; j++) {
			int num = i*xbutt + j;
			if (num < mainMenuButtons.size()) {
				MainMenuButton* button = mainMenuButtons[num];
				button->index = num;
				button->loadTextures("textures/menu/" + std::to_string(num) + "-01.png", "textures/menu/" + std::to_string(num) + "-01.png");
				button->setScale(_backgroundnode->getScaleX() * 0.75f, _backgroundnode->getScaleY() * 0.75f);
				button->setPosition(Vec2((dimen.width / 3.0f) + (dimen.width * j) / 6.0f, (dimen.height * 0.40f) + (dimen.width * (1 - i)) / 6.3f));
				button->setTouchEnabled(true);
				//Add event listener
				button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
					switch (type) {
					case ui::Widget::TouchEventType::ENDED:
						button = ((MainMenuButton*)sender);
						_currController = button->index;
						_activeController = button->getController();
						_rootnode->removeAllChildren();
						_activeController->initialize(_rootnode);
					}
				});
				_worldnode->addChild(button, 1);
				button->retain();
			}
		}
	}
	_active = true;
	return true;
}

void MainMenuController::update(float dt) {
	CCLOG("%d", _currController);
	if (_activeController != nullptr) {
		if (!_activeController->isActive()) {
			if (_activeController->nextLevel()) {
				_currController = mainMenuButtons.size() - 1 == _currController ? _currController : _currController + 1;
				_activeController = mainMenuButtons[_currController]->getController();
				_rootnode->removeAllChildren();
				_activeController->initialize(_rootnode);
				return;
			}
			else {
				_activeController = nullptr;
				_rootnode->addChild(_worldnode, 0);
				return;
			}
		}
		_activeController->update(dt);
	}
}

MainMenuController::~MainMenuController() {
	dispose();
}

/**
* Disposes of all (non-static) resources allocated to this mode.
*/
void MainMenuController::dispose() {
	if (_worldnode != nullptr) {
		_worldnode->release();
		_worldnode = nullptr;
	}
	if (_rootnode != nullptr) {
		_rootnode->release();
		_rootnode = nullptr;
	}
	if (_activeController != nullptr) {
		_activeController = nullptr;
	}
	for (MainMenuButton* b : mainMenuButtons) {
		b->dispose();
		b->release();
	}
	mainMenuButtons.clear();
}

void MainMenuController::preload() {
	// Tutorial button
	GameController* gc = GameController::create(TUTORIAL_KEY, TUTORIAL_FILE);
	gc->preload();
	_tutButt = MainMenuButton::create(gc);
	_tutButt->index = -1;
	_tutButt->loadTextures("textures/Tutorial/tutorial.png", "textures/Tutorial/tutorial.png");
	_tutButt->setTouchEnabled(true);
	//Add event listener
	_tutButt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type) {
		case ui::Widget::TouchEventType::ENDED:
			_tutButt = ((MainMenuButton*)sender);
			_currController = _tutButt->index;
			_activeController = _tutButt->getController();
			_rootnode->removeAllChildren();
			_activeController->initialize(_rootnode);
		}
	});
	_tutButt->retain();

	loadGameController(LEVEL_ONE_KEY, LEVEL_ONE_FILE);
	loadGameController(LEVEL_TWO_KEY, LEVEL_TWO_FILE);
	loadGameController(LEVEL_THREE_KEY, LEVEL_THREE_FILE);
	loadGameController(LEVEL_FOUR_KEY, LEVEL_FOUR_FILE);
	loadGameController(LEVEL_FIVE_KEY, LEVEL_FIVE_FILE);
	loadGameController(LEVEL_SIX_KEY, LEVEL_SIX_FILE);
	loadGameController(LEVEL_SEVEN_KEY, LEVEL_SEVEN_FILE);
	loadGameController(LEVEL_EIGHT_KEY, LEVEL_EIGHT_FILE);
	loadGameController(LEVEL_NINE_KEY, LEVEL_NINE_FILE);

	// Load the textures (Autorelease objects)
	_assets = AssetManager::getInstance()->getCurrent();
	TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();
	tloader->loadAsync(EXPOSURE_BAR, "textures/exposure_bar.png");
	tloader->loadAsync(EXPOSURE_FRAME, "textures/exposure_bar_frame.png");
	tloader->loadAsync(DUDE_TEXTURE, "textures/player_animation.png");
	tloader->loadAsync("dudepool", "textures/Level Pool/Shade_Swim_Animation.png");
	tloader->loadAsync(PEDESTRIAN_TEXTURE, "textures/Pedestrian.png");
	tloader->loadAsync(PEDESTRIAN_SHADOW_TEXTURE, "textures/Pedestrian_S.png");
	tloader->loadAsync(PEDESTRIAN_POOL_TEXTURE, "textures/Level Pool/Level2_RotationP_Animation_New.png");
	tloader->loadAsync(PEDESTRIAN_POOL_SHADOW_TEXTURE, "textures/Level2_RotationP_Animation_S.png");
	tloader->loadAsync(INDICATOR, "textures/indicator.png");
	tloader->loadAsync(CAR_TEXTURE, "textures/car_animation.png");
	tloader->loadAsync(CAR_SHADOW_TEXTURE, "textures/Car1_S.png");
	tloader->loadAsync(GOAL_TEXTURE, "textures/caster_animation.png");
	//tloader->loadAsync("goalpool", "textures/caster_animation.png");
    tloader->loadAsync(WIN_IMAGE, "textures/menu/Win Icon.png");
    tloader->loadAsync(LOSE_IMAGE, "textures/menu/lose_icon.png");
	tloader->loadAsync(WIN_TEXTURE, "textures/Shade_Win.png");
	tloader->loadAsync(LOSE_TEXTURE, "textures/Shade_Sun.png");

	_assets->loadAsync<Sound>(GAME_MUSIC, "sounds/DD_Main.mp3");
	_assets->loadAsync<Sound>(WIN_MUSIC, "sounds/win.mp3");
	_assets->loadAsync<Sound>(LOSE_MUSIC, "sounds/lose.mp3");
	_assets->loadAsync<Sound>(LATCH_SOUND, "sounds/latch.mp3");
	_assets->loadAsync<Sound>(SIGHTED_SOUND, "sounds/sighted.mp3");
	_assets->loadAsync<Sound>(RUN_SOUND, "sounds/run.mp3");

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
	tloader->loadAsync(MENU_BACKGROUND_KEY, "textures/menu/Level Background-01.png");
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
