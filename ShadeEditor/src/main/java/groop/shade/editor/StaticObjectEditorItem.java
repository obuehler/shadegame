package groop.shade.editor;

public class StaticObjectEditorItem implements EditorItem {

	StaticObjectEditorItem(StaticObjectType t) {
		type = t;
	}
	
	/** The type of the static object, like building 1" */
	StaticObjectType type;
	
	@Override
	public void clickAction(int x, int y, StageEditor editor, StageObjectIcon icon) {
		if (icon == null) {
			editor.backgroundPanel.add(new StaticObjectIcon(x, y, editor, type));
		}
	}

	@Override
	public String getName() {
		return type.getName();
	}

}
