package groop.shade.editor;

public enum ObstacleType implements StageType {

	ROCK {
		@Override
		public String getName() {
			return "Rock";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.ROCK;
		}
	},

	TREE {
		@Override
		public String getName() {
			return "Tree";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.TREE;
		}
	};
}
