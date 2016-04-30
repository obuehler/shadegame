package groop.shade.editor;

public enum ObstacleType implements StageType {

	PEDESTRIAN {
		@Override
		public String getName() {
			return "Pedestrian";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.PEDESTRIAN;
		}

		@Override
		public String getIconPath() {
			return Constants.ASSETS_PATH() + "textures" + Constants.FILE_SEPARATOR + "Pedestrian.png";
		}

		@Override
		public String getIconShadowPath() {
			return Constants.ASSETS_PATH() + "textures" + Constants.FILE_SEPARATOR + "Pedestrian_S.png";
		}

		@Override
		public float getDefaultScale() {
			return 0.2f;
		}
	},
	
	CAR {
		@Override
		public String getName() {
			return "Car";
		}

		@Override
		public EditorItems getValue() {
			return EditorItems.CAR;
		}

		@Override
		public String getIconPath() {
			return Constants.ASSETS_PATH() + "textures" + Constants.FILE_SEPARATOR + "Car1.png";
		}

		@Override
		public String getIconShadowPath() {
			return Constants.ASSETS_PATH() + "textures" + Constants.FILE_SEPARATOR + "Car1_S.png";
		}

		@Override
		public float getDefaultScale() {
			return 0.2f;
		}
	}
}
