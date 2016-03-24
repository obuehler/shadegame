package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class BossSite extends Vector2 {

	public BossType type;
	public int bossHealth;
	public float bossSpeed;

	/** Empty constructor for JSON parsing */
	public BossSite() {
	}

	public BossSite(Vector2 site, BossType bossType, float speed) {
		super(site);
		type = bossType;
		bossSpeed = speed;
	}
}
