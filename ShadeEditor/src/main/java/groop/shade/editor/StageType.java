package groop.shade.editor;

public interface StageType {
	public abstract String getName();

	public abstract EditorItems getValue();

	public abstract String getIconPath();

	public abstract String getIconShadowPath();

	public abstract double getDefaultScale();
}