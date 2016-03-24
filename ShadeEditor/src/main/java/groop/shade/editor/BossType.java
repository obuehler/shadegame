package groop.shade.editor;

public enum BossType implements StageType {
	DUMMY(0) {

		public String getName() {
			return "Dummy";
		}

		public EditorItems getValue() {
			return EditorItems.DUMMY;
		}
	},
	THIMBLEWICK(0) {

		public String getName() {
			return "Thimblewick";
		}

		public EditorItems getValue() {
			return EditorItems.FIRSTBOSS;
		}
	},
	CRUDBANE(1) {

		public String getName() {
			return "Crudbane";
		}

		public EditorItems getValue() {
			return EditorItems.SECONDBOSS;
		}
	},
	THIMBLEWICK_REDUX(3) {

		public String getName() {
			return "Thimblewick - Redux";
		}

		public EditorItems getValue() {
			return EditorItems.THIRDBOSS;
		}
	},
	SKRANG(2) {

		public String getName() {
			return "Skrang";
		}

		public EditorItems getValue() {
			return EditorItems.FOURTHBOSS;
		}
	},
	BOTRONGO(4) {

		public String getName() {
			return "Botrongo";
		}

		public EditorItems getValue() {
			return EditorItems.FIFTHBOSS;
		}
	};

	public int index;
	public boolean locked;

	private BossType(int i) {
		index = i;
		locked = false;
		// TODO locked = true;
	}

}