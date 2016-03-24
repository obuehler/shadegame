package groop.shade.editor;

import java.awt.BorderLayout;
import java.awt.Toolkit;
import java.io.File;
import java.util.ArrayList;

import com.badlogic.gdx.Gdx;

public class Constants {

	static {
		BORDER_LAYOUT = new BorderLayout();
		FILE_SEPARATOR = File.separator;
		ASSETS_FOLDER_NAME = "assets";
		STAGES_FOLDER_NAME = "stages";
		EXPORT_IMAGE_FORMAT = "png";
		STAGE_FILE_EXTENSION = "stomp";
		DEFAULT_SCALE = 50.0f;
		BOSSES_PATH = "images/level-select/bosses/";
		STANDARD_WIDTH = standardWidth();
		STANDARD_HEIGHT = standardHeight();
		/*
		 * LwjglApplicationConfiguration config = new
		 * LwjglApplicationConfiguration(); // These are the configuration
		 * attributes config.width = standardWidth(); config.height =
		 * standardHeight(); config.resizable = false; config.title =
		 * "Troll Stomp"; config.fullscreen = false; // RETINA MACS DO NOT
		 * SUPPORT FULLSCREEN (LWJGL Bug) config.addIcon(ASSETS_PATH() +
		 * "images/icon128.png" , Files.FileType.Internal);
		 * config.addIcon(ASSETS_PATH() + "images/icon32.png",
		 * Files.FileType.Internal); config.addIcon(ASSETS_PATH() +
		 * "images/icon16.png", Files.FileType.Internal); STANDARD_CONFIG =
		 * config;
		 */

		ArrayList<String> temp = new ArrayList<String>();
		try {
			for (File f : new File(STAGES_PATH()).getAbsoluteFile().listFiles()) {
				temp.add(f.getName().substring(0, f.getName().lastIndexOf(".")));
			}
		} catch (NullPointerException e) {
			temp.add("level1");
			temp.add("level1scrim");
			temp.add("level1tutorial");
			temp.add("level2battle");
			temp.add("level2skrimage");
			temp.add("level2tutorial");
			temp.add("skrang1");
			temp.add("level4battle");
			temp.add("level5tut");
			temp.add("level5battle");
			temp.add("level5scrim");
		}
		STAGE_NAMES = temp;
	}

	public static final int STANDARD_WIDTH;
	public static final int STANDARD_HEIGHT;
	public static final ArrayList<String> STAGE_NAMES;

	/** Scaling used from screen to Box2d */
	public static final float DEFAULT_SCALE;

	public static final BorderLayout BORDER_LAYOUT;
	public static final String FILE_SEPARATOR;
	public static final String ASSETS_FOLDER_NAME;
	public static final String STAGES_FOLDER_NAME;
	public static final String STAGE_FILE_EXTENSION;
	public static final String EXPORT_IMAGE_FORMAT;
	public static final String BOSSES_PATH;
	// public static final LwjglApplicationConfiguration STANDARD_CONFIG;

	public static final String BACKGROUNDS_PATH() {
		return "images" + FILE_SEPARATOR + "backgrounds" + FILE_SEPARATOR;
	}

	// public static final String ASSETS_PATH() { return ".." + FILE_SEPARATOR +
	// "core" + FILE_SEPARATOR + ASSETS_FOLDER_NAME + FILE_SEPARATOR; }
	// public static final String ASSETS_PATH() {return "bin" + FILE_SEPARATOR;
	// }
	public static final String ASSETS_PATH() {
		return "";
	}

	public static final String STAGES_PATH() {
		return ASSETS_PATH() + STAGES_FOLDER_NAME + FILE_SEPARATOR;
	}

	public static final String DOTTED_STAGE_FILE_EXTENSION() {
		return "." + STAGE_FILE_EXTENSION;
	}

	public static int standardWidth() {
		return (Toolkit.getDefaultToolkit().getScreenSize().width * 3) / 4;
	}

	public static int standardHeight() {
		return (Toolkit.getDefaultToolkit().getScreenSize().height * 3) / 4;
	}

	public static float yRatio() {
		return (float) Gdx.graphics.getHeight() / (float) STANDARD_HEIGHT;
	}

	public static float xRatio() {
		return (float) Gdx.graphics.getWidth() / (float) STANDARD_WIDTH;
	}

}