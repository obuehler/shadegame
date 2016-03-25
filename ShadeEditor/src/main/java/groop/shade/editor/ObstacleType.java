package groop.shade.editor;

public enum ObstacleType implements StageType {

	TREE {
		@Override
		public String getName() {
			return "Tree";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.TREE;
		}

		@Override
		public String getIconPath() {
			return "assets/Plant1.png";
		}

		@Override
		public String getIconShadowPath() {
			return "assets/Plant1_S.png";
		}

		@Override
		public double getDefaultScale() {
			return 0.2;
		}
	};
}
