package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class CasterSiteIcon extends StageIcon {

	public CasterSiteIcon(int x, int y, StageEditor s) {
		super(x, y, s);
	}

	public CasterSiteIcon(CasterSite b, StageEditor s) {
		super(b, s);
	}

	@Override
	protected void setObject(int x, int y) {
		editor.stage.casterSite = new CasterSite(stagePosition());
		object = editor.stage.casterSite;
	}

	protected void setDisplayIcon() {
		setDisplayIcon(CasterSite.DEFAULT_SCALE);
	}

	protected void setDisplayIcon(float scale) {
		setDisplayIcon(CasterSite.IMAGE_PATH, scale);
		editor.stage.casterSite.scale = scale;
	}

	@Override
	public void delete() {
		object = null;
		editor.backgroundPanel.remove(this);
		editor.casterSiteIcon = null;
		editor.stage.casterSite = null;
	}

	@Override
	public void changeEditor(StageEditor e) {
		if (editor.selectedIcon == this) {
			editor.deselect();
		}
		Vector2 originalPosition = editor.stage.casterSite;
		editor.stage.casterSite = null;
		editor.casterSiteIcon = null;

		editor = e;
		try {
			editor.casterSiteIcon.setVisible(false);
		} catch (NullPointerException ex) {
		}
		editor.casterSiteIcon = this;
		editor.stage.casterSite = new CasterSite(originalPosition);
		editor.deselect();
		select();
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(EditorItems.CASTERSITE.getName());
	}

	@Override
	protected void setObject(StageType e, int x, int y) {
	}

	@Override
	protected void setMenu() {
	}

}
