package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class BirdSite extends Vector2 {

	public static final String IMAGE_PATH = Constants.ASSETS_PATH() + "images/editor/birdsite.png";

	public int birdHealth;

	/** Empty constructor for JSON parsing */
	public BirdSite() {
	}

	public BirdSite(Vector2 site) {
		super(site);
	}
}
