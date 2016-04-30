#include <string>
#include <cornell.h>
#include <cocos2d/cocos/ui/UIButton.h>
#include "C_MainMenu.h"

using namespace cocos2d;
using namespace ui;
using namespace std;

// Define constants here
/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   60.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  20.0f

#define LEVEL_ONE_KEY "level1"
#define LEVEL_ONE_FILE "levels/level1.shadl"


MainMenuController::MainMenuController() :
	_rootnode(nullptr),
	_worldnode(nullptr),
	_active(false),
	_activeController(nullptr)
{
}

bool MainMenuController::init(RootLayer* root) {
	return init(root, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

bool MainMenuController::init(RootLayer* root, const Rect& rect) {
	// Determine the center of the screen
	Size dimen = root->getContentSize();
	Vec2 center(dimen.width / 2.0f, dimen.height / 2.0f);

	Texture2D * background = _assets->get<Texture2D>("background");
	Size bsize = background->getContentSize();

	// Create the scene graph
	_worldnode = Node::create();
	_backgroundnode = PolygonNode::createWithTexture(background);
	_backgroundnode->setScale(dimen.width / bsize.width, dimen.height / bsize.height);
	_backgroundnode->setPosition(center.x,center.y);
	_worldnode->addChild(_backgroundnode, 0);

	root->addChild(_worldnode, 0);

	_rootnode = root;
	_rootnode->retain();
	//Add buttons
	Vec2 start = { 350,450 };
	int os = 170;
	int xbutt = 3;
	int ybutt = 3;

	int ypos = 0;
	for (int i = 0; i < ybutt; i++) {
		int xpos = 0;
		for (int j = 0; j < xbutt; j++) {
			Button * button = Button::create();
			Vec2 pos = { start.x + xpos, start.y - ypos };

			button->setTouchEnabled(true);
			button->loadTextures("textures/Owen.jpg", "textures/Owen.jpg");
			button->setPosition(pos);
			button->setScale(.8,.8);
			root->addChild(button, 1);
			GameController* gc = gameControllers[i*j + j];
			//Add event listener
			button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
				switch (type) {
				case Widget::TouchEventType::ENDED:
					_rootnode->removeAllChildren();
					_activeController = gc;
					gc->init(root);
				}
			});

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
}

void MainMenuController::preload() {
	// Load the textures (Autorelease objects)

	_assets = AssetManager::getInstance()->getCurrent();
	TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();


	// Background
	tloader->loadAsync("background", "textures/LevelChoice.png");
	// Button
	tloader->loadAsync("button", "textures/Owen.jpg");

	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));
	gameControllers.push_back(new GameController(string(LEVEL_ONE_KEY), string(LEVEL_ONE_FILE)));

}

/**
* Clear all memory when exiting.
*/
void MainMenuController::stop() {
}
