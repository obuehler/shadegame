package groop.shade.editor;

import java.awt.Dimension;
import java.nio.file.Paths;
import java.util.ArrayList;

import com.badlogic.gdx.graphics.Texture;

public class GameStage {
	/** Scale constants to be used during texture drawing */
	public float scalex;
	public float scaley;
	
	public String name;

	public int index;
	public Dimension pixelSize;
	
	// GameObjects
	public PlayerSite playerSite;
	public CasterSite casterSite;
	public ArrayList<TreeSite> pedestrians;
	public ArrayList<StaticObjectSite> staticObjects;
	public ArrayList<TreeSite> cars;

	/** Background texture for the stage. Set in GameMode. May be null. */
	public Texture background;

	public String imageFormat;

	public String fullBackgroundPath() {
		return Paths.get(assetsBackgroundPath(false)).toString();
	}

	public String assetsBackgroundPath(boolean isAssetManager) {
		if (isAssetManager) {
			return assetsBackgroundPath(false).replace("\\", "/");
		} else {
			return Constants.BACKGROUNDS_PATH() + name + "." + imageFormat;
		}
	}

	public String defaultSavePath() {
		return Paths.get(Helpers.relativeSavePath(name)).toAbsolutePath()
				.toString();
	}

	/** Default constructor needed for Json parsing */
	public GameStage() {
	}

	public GameStage(String stageName, Dimension stageSize) {
		name = stageName;
		pixelSize = stageSize;
		initialize();
	}

	private void initialize() {
		imageFormat = null;
		index = 0;
		playerSite = null;
		casterSite = null;
		pedestrians = new ArrayList<TreeSite>();
		cars = new ArrayList<TreeSite>();
		staticObjects = new ArrayList<StaticObjectSite>();
	}
	
	public int getIndex() {		
		try {		
			return index;		
		} catch (NullPointerException e) {
			return 0;		
		}		
	}		
		
	public void setIndex(int i) {		
		index = i;		
	}

}
