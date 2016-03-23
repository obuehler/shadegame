package org.gradle;

public class RockIcon extends StageIcon {

	public RockIcon(int x, int y, StageEditor s) {
		super(x, y, s);
	}
	
	public RockIcon(RockSite r, StageEditor s) {
		super(r, s);
	}

	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(ObstacleType.ROCK.getIconPath());
	}


	@Override
	public void delete() {
		editor.backgroundPanel.remove(this);
		editor.stage.rocks.remove(object);
	}

	@Override
	public void changeEditor(StageEditor e) {
		editor.stage.rocks.remove(object);
		editor = e;
		editor.stage.rocks.add((RockSite)object);
	}

	@Override
	protected void setObject(StageType e, int x, int y) {} // Never called

	@Override
	protected void setObject(int x, int y) {
		object = new RockSite(stagePosition());
		editor.stage.rocks.add((RockSite)object);
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(ObstacleType.ROCK.getName());		
	}

	@Override
	protected void setMenu() {
		// TODO Auto-generated method stub
		
	}

}
