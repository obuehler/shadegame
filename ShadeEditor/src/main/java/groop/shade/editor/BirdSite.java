package groop.shade.editor;

import model.birds.Bird;
import model.birds.Bird.BirdType;

import com.badlogic.gdx.math.Vector2;

public class BirdSite extends Vector2 {

	public static final String IMAGE_PATH = Constants.ASSETS_PATH() + "images/editor/birdsite.png";

	public int birdHealth;

	/** Empty constructor for JSON parsing */
	public BirdSite() {
	}

	public BirdSite(Vector2 site) {
		super(site);
	}

	public Bird createBird(BirdType type) {
		if (birdHealth > 0) {
			return type.create(position, birdHealth);
		} else {
			return type.create(position, GameStage.DEFAULT_BOSS_HEALTH);
		}
	}

}
