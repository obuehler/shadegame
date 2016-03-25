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
	}

}
