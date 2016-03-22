package org.gradle;

public class TreeIcon extends StageIcon {
	
	public TreeIcon(int x, int y, StageEditor s) {
		super(x, y, s);
	}
	
	public TreeIcon(TreeSite t, StageEditor s) {
		super(t, s);
	}
	
	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(ObstacleType.TREE.getIconPath());
	}

	@Override
	public void delete() {
		editor.backgroundPanel.remove(this);
		editor.stage.trees.remove(object);
	}

	@Override
	public void changeEditor(StageEditor e) {
		editor.stage.trees.remove(object);
		editor = e;
		editor.stage.trees.add((TreeSite)object);
	}

	@Override
	protected void setObject(StageType e, int x, int y) {} // Never called
	

	@Override
	protected void setObject(int x, int y) {
		object = new TreeSite(stagePosition());
		editor.stage.trees.add((TreeSite)object);	
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(ObstacleType.TREE.getName());
	}

	@Override
	protected void setMenu() {
		// TODO Auto-generated method stub
		
	}

}
