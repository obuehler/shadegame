package groop.shade.editor;

public interface StageType {
	public abstract String getName();

	public abstract EditorItems getValue();

	public default String getIconPath() {
		System.err.println("default icon path");
		return "assets/default.png";
	}
}