package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class CasterSite extends Vector2 {

	public static final String IMAGE_PATH = "assets/caster.png";
	public static final double DEFAULT_SCALE = 0.5;

	public double scale;

	/** Empty constructor for JSON parsing */
	public CasterSite() {
	}

	public CasterSite(Vector2 site) {
		super(site);
	}
}
