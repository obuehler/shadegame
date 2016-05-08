package groop.shade.editor;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Toolkit;
import java.io.File;
import java.util.ArrayList;

import javax.swing.border.EmptyBorder;

import com.badlogic.gdx.Gdx;

public class Constants {

	static {
		BORDER_LAYOUT = new BorderLayout();
		FLOW_LAYOUT = new FlowLayout();
		FILE_SEPARATOR = File.separator;
		CONSTANTS_FOLDER_NAME = "constants";
		TEXTURES_FOLDER_NAME = "textures";
		STAGES_FOLDER_NAME = "levels";
		EXPORT_IMAGE_FORMAT = "png";
		STAGE_FILE_EXTENSION = "shadl";
		CONSTANTS_FILE_EXTENSION = "shadc";
		DEFAULT_SCALE = 50.0f;
		BOSSES_PATH = "images/level-select/bosses/";
		STANDARD_WIDTH = standardWidth();
		STANDARD_HEIGHT = standardHeight();
		CAR_ACTIONS = new String[]{"stop", "go", "left", "right"};
		PEDESTRIAN_ACTIONS = new String[]{"walk fast", "walk slow", "look around", "stand"};
		NO_BORDER = new EmptyBorder(0, 0, 0, 0);

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
	public static final FlowLayout FLOW_LAYOUT;
	public static final String FILE_SEPARATOR;
	public static final String TEXTURES_FOLDER_NAME;
	public static final String STAGES_FOLDER_NAME;
	public static final String STAGE_FILE_EXTENSION;
	public static final String EXPORT_IMAGE_FORMAT;
	public static final String BOSSES_PATH;
	public static final String CONSTANTS_FOLDER_NAME;
	public static final String CONSTANTS_FILE_EXTENSION;
	
	public static final String[] CAR_ACTIONS;
	public static final String[] PEDESTRIAN_ACTIONS;
	public static final EmptyBorder NO_BORDER;
	// public static final LwjglApplicationConfiguration STANDARD_CONFIG;

	public static final String BACKGROUNDS_PATH() {
		return ASSETS_PATH() + TEXTURES_FOLDER_NAME + FILE_SEPARATOR + "backgrounds" + FILE_SEPARATOR;
	}

	// public static final String ASSETS_PATH() { return ".." + FILE_SEPARATOR +
	// "core" + FILE_SEPARATOR + ASSETS_FOLDER_NAME + FILE_SEPARATOR; }
	// public static final String ASSETS_PATH() {return "bin" + FILE_SEPARATOR;
	// }
	public static final String ASSETS_PATH() {
		return ".." + FILE_SEPARATOR + "Shade" + FILE_SEPARATOR + "Resources" + FILE_SEPARATOR;
	}

	public static final String STAGES_PATH() {
		return ASSETS_PATH() + STAGES_FOLDER_NAME + FILE_SEPARATOR;
	}
	
	public static final String EDITOR_IMAGES_PATH() {
		return ASSETS_PATH() + "textures" + FILE_SEPARATOR + "editor" + FILE_SEPARATOR;
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

	public static final String STATIC_OBJECT_CONSTANTS_FILE_NAME() {
		return "static_objects." + CONSTANTS_FILE_EXTENSION;
	}
	
	public static final String STATIC_OBJECT_CONSTANTS_FILE_PATH() {
		return ASSETS_PATH() + CONSTANTS_FOLDER_NAME + FILE_SEPARATOR + STATIC_OBJECT_CONSTANTS_FILE_NAME();
	}
	
	public static final String STATIC_OBJECT_ASSETS_FILE_PATH() {
		return ASSETS_PATH() + TEXTURES_FOLDER_NAME + FILE_SEPARATOR + "static_objects" + FILE_SEPARATOR;
	}
	
	public static float yRatio() {
		return (float) Gdx.graphics.getHeight() / (float) STANDARD_HEIGHT;
	}

	public static float xRatio() {
		return (float) Gdx.graphics.getWidth() / (float) STANDARD_WIDTH;
	}

}