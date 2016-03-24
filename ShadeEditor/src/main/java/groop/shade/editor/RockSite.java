package groop.shade.editor;

import model.obstacles.Rock;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class RockSite extends Vector2 {

	/** Empty constructor for JSON parsing */
	public RockSite() {
	}

	public RockSite(Vector2 site) {
		super(site);
	}

	public Rock createRock() {
		return new Rock(this);
	}

}
