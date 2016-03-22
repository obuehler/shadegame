package org.gradle;

import java.nio.file.Paths;

import model.bosses.Boss;
import model.bosses.Botrongo;
import model.bosses.Crudbane;
import model.bosses.Dummy;
import model.bosses.Skrang;
import model.bosses.Thimblewick;
import model.bosses.ThimblewickRedux;

import com.badlogic.gdx.math.Vector2;

import controller.StageSelectionController.BossButton;

public enum BossType implements StageType {
	DUMMY(0) {

		
		public String getName() {
			return "Dummy";
		}

		
		public EditorItems getValue() {
			return EditorItems.DUMMY;
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Dummy.IMAGE_PATH).toAbsolutePath().toString();
		}

		@SuppressWarnings("unchecked")
		
		public Dummy create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new Dummy(pos, bossHealth, bossSpeed);
		}

		
		public BossButton makeButton() {
			return null;
		}
		
	},
	THIMBLEWICK(0) {
		
		public String getName() {
			return "Thimblewick";
		}

		
		public EditorItems getValue() {
			return EditorItems.FIRSTBOSS;
		}

		@SuppressWarnings("unchecked")
		
		public Thimblewick create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new Thimblewick(pos, bossHealth, bossSpeed);
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Thimblewick.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

		
		public BossButton makeButton() {
			return new BossButton(Thimblewick.buttonImage, this);
		}

	},
	CRUDBANE(1) {

		
		public String getName() {
			return "Crudbane";
		}

		
		public EditorItems getValue() {
			return EditorItems.SECONDBOSS;
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Crudbane.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

		@SuppressWarnings("unchecked")
		
		public Crudbane create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new Crudbane(pos, bossHealth, bossSpeed);
		}

		
		public BossButton makeButton() {
			return new BossButton(Crudbane.buttonImage, this);
		}
	},
	THIMBLEWICK_REDUX(3) {

		
		public String getName() {
			return "Thimblewick - Redux";
		}

		
		public EditorItems getValue() {
			return EditorItems.THIRDBOSS;
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + ThimblewickRedux.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

		@SuppressWarnings("unchecked")
		
		public ThimblewickRedux create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new ThimblewickRedux(pos, bossHealth, bossSpeed);
		}

		
		public BossButton makeButton() {
			return new BossButton(ThimblewickRedux.buttonImage, this);
		}
	},
	SKRANG(2) {
		
		public String getName() {
			return "Skrang";
		}

		
		public EditorItems getValue() {
			return EditorItems.FOURTHBOSS;
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Skrang.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

		@SuppressWarnings("unchecked")
		
		public Skrang create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new Skrang(pos, bossHealth, bossSpeed);
		}
		
		
		public BossButton makeButton() {
			return new BossButton(Skrang.buttonImage, this);
		}
	},
	BOTRONGO(4) {
		
		public String getName() {
			return "Botrongo";
		}

		
		public EditorItems getValue() {
			return EditorItems.FIFTHBOSS;
		}

		
		public String getIconPath() {
			return Paths.get(Constants.ASSETS_PATH() + Botrongo.STATIC_IMAGE_PATH).toAbsolutePath().toString();
		}

		@SuppressWarnings("unchecked")
		
		public Botrongo create(Vector2 pos, int bossHealth, float bossSpeed) {
			return new Botrongo(pos, bossHealth, bossSpeed);
		}

		
		public BossButton makeButton() {
			return new BossButton(Botrongo.buttonImage, this);
		}
	};

	public abstract <T extends Boss> T create(Vector2 pos, int bossHealth, float bossSpeed);		
	
	public abstract BossButton makeButton();
	
	public int index;
	public boolean locked;
	
	private BossType(int i) {
		index = i;
		locked = false;
		// TODO locked = true;
	}
	
}