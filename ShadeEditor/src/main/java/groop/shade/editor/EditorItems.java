package groop.shade.editor;

public enum EditorItems implements EditorItem {

	SELECTOR {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
			editor.deselect();
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
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
			icon.delete();
		}

		@Override
		public String getName() {
			return "Eraser";
		}

	},

	PLAYERSITE {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
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
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
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

	PEDESTRIAN {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new TreeIcon(x, y, editor, ObstacleType.PEDESTRIAN));
			}
		}

		@Override
		public String getName() {
			return ObstacleType.PEDESTRIAN.getName();
		}

	},
	CAR {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
			if (icon == null) {
				editor.backgroundPanel.add(new TreeIcon(x, y, editor, ObstacleType.CAR));
			}
		}

		@Override
		public String getName() {
			return ObstacleType.CAR.getName();
		}

	},
	TARGETER {

		@Override
		public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
			if (editor.selectedIcon != null) {
				editor.targetIcon.setLocation(x, y);
				editor.targetIcon.setVisible(true);
				((TreeIcon)(editor.selectedIcon)).targetFieldX.setText(Integer.toString(x));
				((TreeIcon)(editor.selectedIcon)).targetFieldY.setText(Integer.toString(y));
				editor.repaint();
			}
		}

		@Override
		public String getName() {
			return "Target Position";
		}
		
	}

}
