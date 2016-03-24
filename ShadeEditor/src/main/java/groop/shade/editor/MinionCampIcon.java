package groop.shade.editor;

import java.awt.Dimension;

import javax.swing.ButtonGroup;
import javax.swing.JMenu;

public class MinionCampIcon extends StageIcon implements ChangeableIcon {

	public MinionCampIcon(MinionType e, int x, int y, StageEditor s) {
		super(e, x, y, s);
	}

	public MinionCampIcon(MinionCamp c, StageEditor s) {
		super(c, s);
	}

	@Override
	protected void setDisplayIcon() {
		setDisplayIcon(((MinionCamp) object).type.getIconPath());
	}

	@Override
	public void delete() {
		editor.backgroundPanel.remove(this);
		editor.stage.minionCamps.remove(object);
	}

	@Override
	public void changeEditor(StageEditor e) {
		editor.stage.minionCamps.remove(object);
		editor = e;
		editor.stage.minionCamps.add(((MinionCamp) object));
	}

	@Override
	protected void refreshToolTipText() {
		setToolTipText(((MinionCamp) object).type.getName());
	}

	@Override
	protected void setObject(StageType e, int x, int y) {
		object = new MinionCamp(stagePosition(), (MinionType) e);
		editor.stage.minionCamps.add((MinionCamp) object);
	}

	@Override
	protected void setObject(int x, int y) {
	} // Never called

	@Override
	protected void setMenu() {
		ButtonGroup group = new ButtonGroup();
		JMenu typeMenu = new JMenu("Type            >");
		typeMenu.setPreferredSize(new Dimension(200, 40));
		typeMenu.setMinimumSize(new Dimension(200, 30));
		typeMenu.setSize(typeMenu.getPreferredSize());
		typeMenu.setMaximumSize(new Dimension(2000, 50));
		for (MinionType minionType : MinionType.values()) {
			EditorRadioButton button = new EditorRadioButton(minionType, this);
			if (minionType == ((MinionCamp) object).type)
				button.setSelected(true);
			group.add(button);
			typeMenu.add(button);
		}
		menu.add(typeMenu);
	}

	public void changeType(StageType e) {
		((MinionCamp) object).type = (MinionType) e;
		setDisplayIcon();
		refreshMenu();
		refreshToolTipText();
	}

	@Override
	public void refreshMenu() {
		// TODO Auto-generated method stub

	}

}
