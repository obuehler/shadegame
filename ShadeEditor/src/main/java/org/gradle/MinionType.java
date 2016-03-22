package org.gradle;

import java.nio.file.Paths;

import model.birds.Bird;
import model.minions.ArrowMinion;
import model.minions.FireworksMinion;
import model.minions.Minion;
import model.minions.TrickShooter;

import com.badlogic.gdx.math.Vector2;

public enum MinionType implements StageType {
	
	ARROW {
		@SuppressWarnings("unchecked")
		@Override
		public ArrowMinion create(Vector2 camp, Bird bird) {
			return new ArrowMinion(camp, bird);
		}

		@Override
		public String getName() {
			return "Arrow Minion";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.ARROWMINION;
		}

		@Override
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + ArrowMinion.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

	},
	
	TRICKSHOOTER {
		@SuppressWarnings("unchecked")
		@Override
		public TrickShooter create(Vector2 camp, Bird bird) {
			return new TrickShooter(camp);
		}

		@Override
		public String getName() {
			return "Trick Shooter";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.TRICKSHOOTER;
		}

		@Override
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + TrickShooter.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

	},
	
	FIREWORKS {
		@SuppressWarnings("unchecked")
		@Override
		public FireworksMinion create(Vector2 camp, Bird bird) {
			return new FireworksMinion(camp);
		}

		@Override
		public String getName() {
			return "Fireworks Minion";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.FIREWORKSMINION;
		}

		@Override
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + FireworksMinion.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

	};

	public abstract <T extends Minion> T create(Vector2 camp, Bird bird);
}
