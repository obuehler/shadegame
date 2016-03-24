package groop.shade.editor;

import model.birds.Bird;
import model.minions.Minion;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class MinionCamp extends Vector2 {

	public MinionType type;

	/** Empty constructor for JSON parsing */
	public MinionCamp() {
	}

	public MinionCamp(Vector2 campSite, MinionType minionType) {
		super(campSite);
		type = minionType;
	}

	public <T extends Minion> T minion(Bird bird) {
		return type.create(this, bird);
	}

}
