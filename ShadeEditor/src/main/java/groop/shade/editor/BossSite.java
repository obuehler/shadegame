package groop.shade.editor;

import model.bosses.Boss;
import model.bosses.Boss.BossType;

import com.badlogic.gdx.math.Vector2;

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

	public Boss createBoss() {
		int health;
		float speed;
		if (bossHealth > 0) {
			health = bossHealth;
		} else {
			health = GameStage.DEFAULT_BOSS_HEALTH;
		}
		if (bossSpeed > 0) {
			speed = bossSpeed;
		} else {
			speed = 1f;
		}
		return type.create(position, health, speed);
	}

}
