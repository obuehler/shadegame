package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class CasterSite extends Vector2 {
	
	public static final float DEFAULT_SCALE = 0.5f;

	public double scale;

	/** Empty constructor for JSON parsing */
	public CasterSite() {
	}

	public CasterSite(Vector2 site) {
		super(site);
	}
}
