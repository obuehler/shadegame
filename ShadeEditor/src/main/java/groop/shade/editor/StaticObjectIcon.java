package groop.shade.editor;

import java.util.ArrayList;

import com.badlogic.gdx.math.Vector2;

public class StaticObjectIcon extends StageIcon {
	
	StaticObjectType staticObjectType;

	public StaticObjectIcon(int x, int y, StageEditor s, StaticObjectType t) {
		super(t, x, y, s);
	}
	
	public StaticObjectIcon(StaticObjectSite t, StageEditor s) {
		super(t, s);
	}
	
	
	
	@Override
	protected void setObject(StageType e, int x, int y) {
		staticObjectType = (StaticObjectType) e;
		setObject(x, y);
	}

	@Override
	protected void setObject(int x, int y) {
		object = new StaticObjectSite(stagePosition(), 1.0f, staticObjectType);
		editor.stage.staticObjects.add((StaticObjectSite)object);
	}
	
	@Override
	protected void setObject(Vector2 o) {
		super.setObject(o);
		staticObjectType = ((StaticObjectSite) o).type;
	}

	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(staticObjectType.getIconPath(), staticObjectType.getIconShadowPath(),
				staticObjectType.getDefaultScale());
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(staticObjectType.getName());
	}

	@Override
	public void delete() {
		editor.backgroundPanel.remove(this);
		editor.stage.staticObjects.remove(object);
	}

	@Override
	protected void setMenu() {}

	@Override
	public void changeEditor(StageEditor e) {
		editor.stage.staticObjects.remove(object);
		editor = e;
		editor.stage.staticObjects.add((StaticObjectSite) object);

	}

}
