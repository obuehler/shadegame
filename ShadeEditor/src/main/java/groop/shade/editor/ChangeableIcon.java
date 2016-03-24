package groop.shade.editor;

public interface ChangeableIcon {

	public void changeType(StageType e);

	/** Refreshes side menu when the type is changed */
	public void refreshMenu();

}