package org.gradle;

import java.awt.Dimension;
import java.nio.file.Paths;
import java.util.ArrayList;

import com.badlogic.gdx.graphics.Texture;

public class GameStage {
	public static final int DEFAULT_BOSS_HEALTH = 20;
	public static final int DEFAULT_BIRD_HEALTH = 15;
	public static final int DEFAULT_ARROW_SPEED = 15;
	public static final int DEFAULT_BIRD_SPEED = 8;
	public static final int DEFAULT_BOOMERANG_SPEED = 12;
	public static final int DEFAULT_KILLS_BEFORE_MUTINY = 10;
	public static final int DEFAULT_ROCK_SPEED = 10;
	public static final int DEFAULT_MINION_SPEED = 10;
	
	/** Scale constants to be used during texture drawing */
	public float scalex;
	public float scaley;
	
	public int index;
	
	public String name;
	public Dimension pixelSize;
	
	/** Background texture for the stage. Set in GameMode. May be null. */
	public Texture background;
	
	public String imageFormat;
	
	public float arrowSpeed;
	public float rockSpeed;
	public float boomerangSpeed;
	public float birdSpeed;
	public float minionSpeed;
	
	public int killsBeforeMutiny;
	
	public String fullBackgroundPath() {
		return Paths.get(Constants.ASSETS_PATH() + assetsBackgroundPath(false)).toString();
	}
	
	public String assetsBackgroundPath(boolean isAssetManager) {
		if (isAssetManager) {
			return assetsBackgroundPath(false).replace("\\", "/");
		} else {
			return Constants.BACKGROUNDS_PATH() + name + "." + imageFormat;
		}
	}
	
	public String defaultSavePath() {
		return Paths.get(Constants.STAGES_PATH() + name + Constants.DOTTED_STAGE_FILE_EXTENSION()).toAbsolutePath().toString();
	}

	// GameObjects
	public BirdSite birdSite;
	public ArrayList<BossSite> bossSites;
	public ArrayList<MinionCamp> minionCamps;
	public ArrayList<TreeSite> trees;
	public ArrayList<RockSite> rocks;
	
	/** Default constructor needed for Json parsing */
	public GameStage() {}
	
	public GameStage(String stageName, Dimension stageSize) {
		name = stageName;
		pixelSize = stageSize;
		initialize();
	}
	
	/* public GameStage(GameStage stage) {
		scalex = stage.scalex;
		scaley = stage.scaley;
		name = stage.name;
		pixelSize = stage.pixelSize;
		imageFormat = stage.imageFormat;
		arrowSpeed = stage.arrowSpeed;
		birdSite = stage.birdSite;
		bossSites = stage.bossSites;
		minionCamps = stage.minionCamps;
		trees = stage.trees;
		rocks = stage.rocks;
	} */
	
	private void initialize() {
		index = 0;
		killsBeforeMutiny = DEFAULT_KILLS_BEFORE_MUTINY;
		imageFormat = null;
		arrowSpeed = 1f;
		rockSpeed = 1f;
		birdSite = null;
		bossSites = new ArrayList<BossSite>();
		minionCamps = new ArrayList<MinionCamp>();	
		trees = new ArrayList<TreeSite>();
		rocks = new ArrayList<RockSite>();
	}
	
	public BossType getBossType() {
		return bossSites.get(0).type;
	}
	
	public int getIndex() {
		try {
			return index;
		} catch (NullPointerException e) {
			return 0;
		}
	}
	
	public void setIndex(int i) {
		index= i;
	}
	
}
