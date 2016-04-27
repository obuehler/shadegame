package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class PlayerSite extends Vector2 {

	public static final String IMAGE_PATH = "assets/player.png";
	public static final double DEFAULT_SCALE = 1.0 / 7.0f;

	public double scale;

	/** Empty constructor for JSON parsing */
	public PlayerSite() {
	}

	public PlayerSite(Vector2 site) {
		super(site);
	}
}
