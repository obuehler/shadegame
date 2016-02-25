# Cornell-Cocos
Cocos2d-x Extensions for CS/INFO 4152

How to start a new game
-----------------------

1. Download the code from [cocos2d download site][4]
2. Run `setup.py`
3. Run the `cocos` script

Example:

    $ cd cocos2d-x
    $ ./setup.py
    $ source FILE_TO_SAVE_SYSTEM_VARIABLE
    $ cocos new MyGame -p com.your_company.mygame -l cpp -d NEW_PROJECTS_DIR
    $ cd NEW_PROJECTS_DIR/MyGame

### Build and run new project for win32 ###

    $ cocos run -p win32

### Build and run new project for Windows 10.0 and Windows Phone 10.0 ###

    $ cocos new MyGame -p com.your_company.mygame -l cpp -d projects

* Visual Studio 2015 (Release version) and Windows 10.0 (Release version) are required for Windows 10.0 Universal Windows Platform (UWP) Apps. RC versions of Visual Studio and Windows 10 are no longer supported.
