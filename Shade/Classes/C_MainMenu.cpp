#include <string>
#include <cornell.h>
#include "C_Gameplay.h"

using namespace cocos2d;
using namespace std;

// Define constants here
/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   60.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  20.0f


MainMenuController::MainMenuController() :
	_rootnode(nullptr),
	_worldnode(nullptr),
	_active(false)

bool MainMenuController::init(RootLayer* root) {
	return init(root, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

bool MainMenuController::init(RootLayer* root, const Rect& rect) {
	// Create the scene graph
	_worldnode = Node::create();

	root->addChild(_worldnode, 0);
	_rootnode = root;
	_rootnode->retain();

	_active = true;
	return true;
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
}

void MainMenuController::preload() {
	// Load the textures (Autorelease objects)

	_assets = AssetManager::getInstance()->getCurrent();
	TextureLoader* tloader = (TextureLoader*)_assets->access<Texture2D>();


	// Cars
	tloader->loadAsync("car1", "textures/Car1.png");
	tloader->loadAsync("car1s", "textures/Car1_S.png");
	tloader->loadAsync("car2", "textures/Car2.png");
	tloader->loadAsync("car2s", "textures/Car2_S.png");
}

/**
* Clear all memory when exiting.
*/
void MainMenuController::stop() {
}
