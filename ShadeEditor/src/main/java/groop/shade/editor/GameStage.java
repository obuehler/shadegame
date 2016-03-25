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
	public Dimension pixelSize;

	/** Background texture for the stage. Set in GameMode. May be null. */
	public Texture background;

	public String imageFormat;

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
		return Paths.get(Constants.STAGES_PATH() + name + Constants.DOTTED_STAGE_FILE_EXTENSION()).toAbsolutePath()
				.toString();
	}

	// GameObjects
	public PlayerSite playerSite;
	public CasterSite casterSite;
	public ArrayList<TreeSite> trees;

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
		playerSite = null;
		casterSite = null;
		trees = new ArrayList<TreeSite>();
	}

}
