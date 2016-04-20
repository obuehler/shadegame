package groop.shade.editor;

public interface EditorItem {
	public abstract void clickAction(int x, int y, StageEditor editor, StageIcon icon);

	public abstract String getName();
}
