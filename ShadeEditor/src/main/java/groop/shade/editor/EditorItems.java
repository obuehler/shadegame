package groop.shade.editor;

import javax.swing.border.EmptyBorder;

public enum EditorItems {

	SELECTOR {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			deselect(editor);
			if (icon != null) {
				icon.select();
			}
		}

		@Override
		public String getName() {
			return "Selector";
		}
	},
	ERASER {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			icon.delete();
		}

		@Override
		public String getName() {
			return "Eraser";
		}

	},

	PLAYERSITE {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				PlayerSiteIcon sicon = editor.playerSiteIcon;
				if (sicon == null) {
					editor.playerSiteIcon = new PlayerSiteIcon(x, y, editor);
					editor.backgroundPanel.add(editor.playerSiteIcon);
				} else {
					sicon.moveIcon(x, y);
				}
			}
		}

		@Override
		public String getName() {
			return "Player Site";
		}

	},

	CASTERSITE {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				CasterSiteIcon sicon = editor.casterSiteIcon;
				if (sicon == null) {
					editor.casterSiteIcon = new CasterSiteIcon(x, y, editor);
					editor.backgroundPanel.add(editor.casterSiteIcon);
				} else {
					sicon.moveIcon(x, y);
				}
			}
		}

		@Override
		public String getName() {
			return "Caster Site";
		}

	},

	DUMMY {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.DUMMY, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return BossType.DUMMY.getName();
		}

	},
	FIRSTBOSS {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.THIMBLEWICK, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return BossType.THIMBLEWICK.getName();
		}

	},
	SECONDBOSS {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.CRUDBANE, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return BossType.CRUDBANE.getName();
		}

	},
	THIRDBOSS {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.THIMBLEWICK_REDUX, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return BossType.THIMBLEWICK_REDUX.getName();
		}
	},
	ARROWMINION {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {

			if (icon == null) {
				editor.backgroundPanel.add(new MinionCampIcon(MinionType.ARROW, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return MinionType.ARROW.getName();
		}

	},
	FIREWORKSMINION {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new MinionCampIcon(MinionType.FIREWORKS, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return MinionType.FIREWORKS.getName();
		}

	},
	TRICKSHOOTER {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new MinionCampIcon(MinionType.TRICKSHOOTER, x, y, editor));
			}
		}

		@Override
		public String getName() {
			return MinionType.TRICKSHOOTER.getName();
		}

	},
	ROCK {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new RockIcon(x, y, editor));
			}
		}

		@Override
		public String getName() {
			return ObstacleType.ROCK.getName();
		}

	},
	TREE {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new TreeIcon(x, y, editor));
			}
		}

		@Override
		public String getName() {
			return ObstacleType.TREE.getName();
		}

	},
	FOURTHBOSS {
		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.SKRANG, x, y, editor));
			}

		}

		@Override
		public String getName() {
			return BossType.SKRANG.getName();
		}
	},
	FIFTHBOSS {
		@Override
		public void clickAction(int x, int y, StageEditor editor, StageIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new BossSiteIcon(BossType.BOTRONGO, x, y, editor));
			}

		}

		@Override
		public String getName() {
			return BossType.BOTRONGO.getName();
		}
	};

	public abstract void clickAction(int x, int y, StageEditor editor, StageIcon icon);

	public abstract String getName();

	private static final EmptyBorder NO_BORDER = new EmptyBorder(0, 0, 0, 0);

	/** Deselects the currently selected object by removing its outline. */
	public static void deselect(StageEditor editor) {
		if (editor.selectedIcon != null) {
			editor.selectedIcon.setBorder(NO_BORDER);
			editor.selectedIcon.menu.setVisible(false);
			editor.selectedIcon = null;
		}
		editor.sideMenu.setVisible(true);
	}

}
