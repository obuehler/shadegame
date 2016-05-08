package groop.shade.editor;

import com.badlogic.gdx.math.Vector2;

@SuppressWarnings("serial")
public class PlayerSiteIcon extends StageObjectIcon {

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

	protected void setDisplayIcon(float scale) {
		setDisplayIcon(Constants.EDITOR_IMAGES_PATH() + "player.png", scale);
		editor.stage.playerSite.scale = scale;
	}

	@Override
	public void delete() {
		object = null;
		editor.backgroundPanel.remove(this);
		editor.playerSiteIcon = null;
		editor.stage.playerSite = null;
	}

	@Override
	public void changeEditor(StageEditor e) {
		if (editor.selectedIcon == this) {
			editor.deselect();
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
		editor.deselect();
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
