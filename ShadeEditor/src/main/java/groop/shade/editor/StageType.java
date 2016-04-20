package groop.shade.editor;

public interface StageType {
	public abstract String getName();

	public abstract EditorItem getValue();

	public abstract String getIconPath();

	public abstract String getIconShadowPath();

	public abstract double getDefaultScale();
}