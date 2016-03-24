package groop.shade.editor;

public enum MinionType implements StageType {

	ARROW {
		@Override
		public String getName() {
			return "Arrow Minion";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.ARROWMINION;
		}
	},

	TRICKSHOOTER {
		@Override
		public String getName() {
			return "Trick Shooter";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.TRICKSHOOTER;
		}

	},

	FIREWORKS {
		@Override
		public String getName() {
			return "Fireworks Minion";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.FIREWORKSMINION;
		}
	};
}
