package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class PlayerSiteIcon extends StageIcon {

	public PlayerSiteIcon(int x, int y, StageEditor s) {
		super(x, y, s);
	}

	public PlayerSiteIcon(PlayerSite b, StageEditor s) {
		super(b, s);
	}

	@Override
	protected void setObject(int x, int y) {
		editor.stage.playerSite = new PlayerSite(stagePosition());
		object = editor.stage.playerSite;
	}

	protected void setDisplayIcon() {
		setDisplayIcon(PlayerSite.DEFAULT_SCALE);
	}

	protected void setDisplayIcon(double scale) {
		setDisplayIcon(PlayerSite.IMAGE_PATH, scale);
		editor.stage.playerSite.scale = scale;
	}

	@Override
	public void delete() {
		object = null;
		editor.backgroundPanel.remove(this);
		editor.stage.playerSite = null;
	}

	@Override
	public void changeEditor(StageEditor e) {
		if (editor.selectedIcon == this) {
			EditorItems.deselect(editor);
		}
		Vector2 originalPosition = editor.stage.playerSite;
		editor.stage.playerSite = null;
		editor.playerSiteIcon = null;

		editor = e;
		try {
			editor.playerSiteIcon.setVisible(false);
		} catch (NullPointerException ex) {
		}
		editor.playerSiteIcon = this;
		editor.stage.playerSite = new PlayerSite(originalPosition);
		EditorItems.deselect(editor);
		select();
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(EditorItems.PLAYERSITE.getName());
	}

	@Override
	protected void setObject(StageType e, int x, int y) {
	}

	@Override
	protected void setMenu() {
	}

}
