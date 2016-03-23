package org.gradle;

import com.badlogic.gdx.math.Vector2;

public class BirdSiteIcon extends StageIcon {

	public BirdSiteIcon(int x, int y, StageEditor s) {
		super(x, y, s);
	}
	
	public BirdSiteIcon(BirdSite b, StageEditor s) {
		super(b, s);
	}

	@Override
	protected void setObject(int x, int y) {
		editor.stage.birdSite = new BirdSite(stagePosition());
		object = editor.stage.birdSite;
	}

	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(BirdSite.IMAGE_PATH);
	}

	@Override
	public void delete() {
		object = null;
		editor.backgroundPanel.remove(this);
		editor.stage.birdSite = null;
	}

	@Override
	public void changeEditor(StageEditor e) {
		if (editor.selectedIcon == this) {
			EditorItems.deselect(editor);
		}
		Vector2 originalPosition = editor.stage.birdSite;
		editor.stage.birdSite = null;
		editor.birdSiteIcon = null;
		
		editor = e;
		try {
			editor.birdSiteIcon.setVisible(false);
		} catch (NullPointerException ex) {}
		editor.birdSiteIcon = this;
		editor.stage.birdSite = new BirdSite(originalPosition);
		EditorItems.deselect(editor);
		select();
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(EditorItems.BIRDSITE.getName());
	}

	@Override
	protected void setObject(StageType e, int x, int y) {} // Never called

	@Override
	protected void setMenu() {
		AttributeSlider attributeSlider = new AttributeSlider(1, 100, "Health") {

			@Override
			public void setInitialValue() {
				if (((BirdSite)object).birdHealth > 0) {
					slider.setValue(((BirdSite)object).birdHealth);
				} else {
					slider.setValue(GameStage.DEFAULT_BIRD_HEALTH);
				}
			}

			@Override
			public void updateAttribute() {
				((BirdSite)object).birdHealth = slider.getValue();
			}
			
		};
		menu.add(attributeSlider);
	}
	
}
